#include "stdafx.h"

#define F_START 30.0     // МГц
#define F_END 32.0       // МГц
#define TARGET_PEAK -54.0 // dBm

size_t read_hex_file(const char* filename, uint8_t** data);

int calculate_fft(CRadioServiceAppDlg* pDlgFrame){
	uint8_t* byte_array = NULL;
	size_t byte_count = 0;
	double f_start = F_START;
	double f_end = F_END;
	if (pDlgFrame->ptr_usb_data == NULL)
	{
		//byte_count = read_hex_file("30_5.txt", &byte_array); //load from file

		//load from internal buffer
		byte_count = sizeof(output30_5);
		byte_array = (uint8_t*)calloc(byte_count, sizeof(uint8_t));
		memcpy(byte_array, output30_5, byte_count);
	}
	else{
		byte_count = pDlgFrame->usbbytescount;
		f_start = pDlgFrame->dt.start_freq;
		f_end = pDlgFrame->dt.end_freq;
		byte_array = pDlgFrame->ptr_usb_data;
	}

	if (byte_array == NULL){
		fprintf(stderr, "The data pointer is 0 \n");
		return 1;
	}
	if (byte_count == 0){
		fprintf(stderr, "The data size is 0 \n");
		free(byte_array);
		return 1;
	}

	// Проверяем, что данных достаточно для комплексных чисел (кратно 4)
	if (byte_count % 4 != 0) {
		fprintf(stderr, "The data size must be a multiple of 4 (int16 complex)\n");
		free(byte_array);
		return 1;
	}

	size_t num_samples = byte_count / 2; // Количество 16-битных значений
	size_t complex_count = num_samples / 2; // Количество комплексных чисел

	// Создаем массив комплексных чисел
	fftw_complex* signal = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* complex_count);
	int16_t* int16_data = (int16_t*)byte_array;

	for (size_t i = 0; i < complex_count; i++) {
		signal[i][0] = (double)int16_data[2 * i];   // Real part
		signal[i][1] = (double)int16_data[2 * i + 1]; // Imaginary part
	}

	// Выделяем память для результата FFT
	fftw_complex* fft_result = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* complex_count);

	// Создаем план FFT
	fftw_plan plan = fftw_plan_dft_1d(complex_count, signal, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);


	// Вычисляем амплитуду и делаем fftshift
	double* fftMag = (double*)malloc(complex_count * sizeof(double));
	for (int i = 0; i < complex_count; i++) {
		fftMag[i] = hypot(fft_result[i][0], fft_result[i][1]);
	}

	// Применяем fftshift к амплитудам
	double* shiftedMag = (double*)malloc(complex_count * sizeof(double));
	int half = complex_count / 2;
	memcpy(shiftedMag, fftMag + half, half * sizeof(double));
	memcpy(shiftedMag + half, fftMag, half * sizeof(double));

	// Применяем оконную коррекцию (4/N) и обрезаем края (28 точек с каждой стороны)
	int start = 28;
	int end = complex_count - 28;
	int N = end - start;
	double scale = 4.0 / complex_count;

	// Выделяем память для результатов
	double* fft_dBm = (double*)malloc(N * sizeof(double));

	// Вычисляем dBm с новыми параметрами (20*log10 вместо 10*log10 и 1e-6 вместо 1e-8)
	for (int i = start; i < end; i++) {
		double mag = scale * shiftedMag[i];
		fft_dBm[i - start] = 20 * log10(mag / 1e-6);  // 20*log10(mag/1e-6) как в MATLAB
	}


	//FILE* out_file = fopen("fft_results.csv", "w");
	//fprintf(out_file, "Frequency(MHz),Amplitude(dBm)\n");
		for (size_t i = 0; i < N; i++) {
			//double freq = f_start + (f_end - f_start) * i / complex_count;
			double freq = f_start + (f_end - f_start) * i / (N - 1);
			//fprintf(out_file, "%f,%f,\n", freq, shifted_fft[i]);
			CRadioServiceAppDlg::FrequencyData data;
			data.frequency = freq;
			data.amplitude = fft_dBm[i];
			if (i < pDlgFrame->m_frequencyData.size())
			{
				pDlgFrame->m_frequencyData[i] = data;
			}
			else
			{
				pDlgFrame->m_frequencyData.push_back(data);
			}
		}

	//fclose(out_file);
	//printf("Processing complete. Results saved in fft_results.csv\n");

	// Освобождаем память
	fftw_destroy_plan(plan);
	fftw_free(signal);
	fftw_free(fft_result);
	free(fft_dBm);
	free(shiftedMag);
	free(byte_array);
	free(fftMag);


	return 0;
}

int calculate_fft_new(CRadioServiceAppDlg* pDlgFrame){
#ifndef nofunc
	uint8_t* byte_array = NULL;
	size_t byte_count = 0;
	double f_start = F_START;
	double f_end = F_END;
	if (pDlgFrame->ptr_usb_data == NULL)
	{
		//byte_count = read_hex_file("30_5.txt", &byte_array); //load from file

		//load from internal buffer
		byte_count = sizeof(output30_5);
		byte_array = (uint8_t*)calloc(byte_count, sizeof(uint8_t));
		memcpy(byte_array, output30_5, byte_count);
	}
	else{
		byte_count = pDlgFrame->usbbytescount;
		f_start = pDlgFrame->dt.start_freq;
		f_end = pDlgFrame->dt.end_freq;
		byte_array = pDlgFrame->ptr_usb_data;
	}

	if (byte_array == NULL){
		fprintf(stderr, "The data pointer is 0 \n");
		return 1;
	}
	if (byte_count == 0){
		fprintf(stderr, "The data size is 0 \n");
		free(byte_array);
		return 1;
	}

	// Проверяем, что данных достаточно для комплексных чисел (кратно 4)
	if (byte_count % 4 != 0) {
		fprintf(stderr, "The data size must be a multiple of 4 (int16 complex)\n");
		free(byte_array);
		return 1;
	}

	size_t num_samples = byte_count / 2; // Количество 16-битных значений
	size_t complex_count = num_samples / 2; // Количество комплексных чисел

	// Создаем массив комплексных чисел
	fftw_complex* signal = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* complex_count);
	int16_t* int16_data = (int16_t*)byte_array;

	for (size_t i = 0; i < complex_count; i++) {
		signal[i][0] = (double)int16_data[2 * i];   // Real part
		signal[i][1] = (double)int16_data[2 * i + 1]; // Imaginary part
	}

	// Выделяем память для результата FFT
	fftw_complex* fft_result = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* complex_count);
#define chunk        256

	size_t cnt_chanks = complex_count / chunk;
	//обрезаем края (28 точек с каждой стороны)
	int start = 28;
	int end = chunk - 28;
	int chunk_cutted = end - start; //200 chunk_cutted	
	pDlgFrame->m_frequencyData.resize(cnt_chanks * chunk_cutted);

	for (int i = 0; i < cnt_chanks; i++){
		//определяем начальную/конечную частоту
		double f_start_chunk = f_start + i * _2MHz;
		double f_end_chunk = f_start_chunk + _2MHz;
		// Создаем план FFT
		//fftw_plan plan = fftw_plan_dft_1d(complex_count, signal, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
		fftw_plan plan = fftw_plan_dft_1d(chunk, &signal[i * chunk], &fft_result[i * chunk], FFTW_FORWARD, FFTW_ESTIMATE);
		fftw_execute(plan);

		// Вычисляем амплитуду и делаем fftshift
		double* fftMag = (double*)malloc(chunk * sizeof(double));
		for (int j = 0; j < chunk; j++) {
			fftMag[j] = hypot(fft_result[j + i * chunk][0], fft_result[j + i * chunk][1]);
		}

		// Применяем fftshift к амплитудам
		double* shiftedMag = (double*)malloc(chunk * sizeof(double));
		int half = chunk / 2;
		memcpy(shiftedMag, fftMag + half, half * sizeof(double));
		memcpy(shiftedMag + half, fftMag, half * sizeof(double));

		double scale = 4.0 / chunk;
		// Выделяем память для результатов
		double* fft_dBm = (double*)malloc(chunk_cutted * sizeof(double));
		// Вычисляем dBm с новыми параметрами (20*log10 вместо 10*log10 и 1e6 вместо 1e8)
		for (int j = start; j < end; j++) {
			double mag = scale * shiftedMag[j];
			fft_dBm[j - start] = 20 * log10(mag / 1e6);  // 20*log10(mag/1e6) как в MATLAB
		}
		//FILE* out_file = fopen("fft_results.csv", "w");
		//fprintf(out_file, "Frequency(MHz),Amplitude(dBm)\n");
		for (size_t j = 0; j < chunk_cutted; j++) {
			//double freq = f_start + (f_end - f_start) * i / complex_count;
			double freq = f_start_chunk + (f_end_chunk - f_start_chunk) * j / (chunk_cutted - 1);
			//fprintf(out_file, "%f,%f,\n", freq, shifted_fft[i]);
			CRadioServiceAppDlg::FrequencyData data;
			data.frequency = freq;
			data.amplitude = fft_dBm[j];
			if (j + i * chunk_cutted < pDlgFrame->m_frequencyData.size())
			    { pDlgFrame->m_frequencyData[j + i * chunk_cutted] = data;}
			else
			    { pDlgFrame->m_frequencyData.push_back(data);}
		}
		//fclose(out_file);
		//printf("Processing complete. Results saved in fft_results.csv\n");
		// Освобождаем память
		fftw_destroy_plan(plan);
		free(fft_dBm);
		free(shiftedMag);
		free(fftMag);
	}

	fftw_free(signal);
	fftw_free(fft_result);
	free(byte_array);
#endif
		return 0;
}

// Функция для чтения hex-файла и преобразования в массив uint8_t
size_t read_hex_file(const char* filename, uint8_t** data) {

	FILE* file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Failed to open file %s\n", filename);
		exit(1);
	}

	// Определяем размер файла
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Читаем весь файл
	char* content = (char*)malloc(file_size + 1);
	fread(content, 1, file_size, file);
	content[file_size] = '\0';
	fclose(file);

	// Удаляем пробелы и переводы строк
	size_t content_len = 0;
	for (long i = 0; i < file_size; i++) {
		if (!isspace(content[i])) {
			content[content_len++] = content[i];
		}
	}
	content[content_len] = '\0';

	// Проверяем четное количество символов
	if (content_len % 2 != 0) {
		fprintf(stderr, "Odd number of hex characters\n");
		free(content);
		exit(1);
	}

	// Преобразуем hex-строку в массив байтов
	size_t byte_count = content_len / 2;
	*data = (uint8_t*)malloc(byte_count);

	for (size_t i = 0; i < byte_count; i++) {
		char byte_str[3] = { content[2 * i], content[2 * i + 1], '\0' };
		(*data)[i] = (uint8_t)strtoul(byte_str, NULL, 16);
	}

	free(content);
	return byte_count;
}