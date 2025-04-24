import numpy as np
import matplotlib.pyplot as plt

# === Путь к файлу ===
filename = '30_5.txt'  # Заменить на имя файла

# === Считывание текстовых hex-данных ===
with open(filename, 'r') as f:
    hex_str = f.read().strip().replace(' ', '').replace('\n', '').replace('\r', '')

# === Разделим по байтам (по 2 символа) и преобразуем в uint8 ===
hex_bytes = [hex_str[i:i+2] for i in range(0, len(hex_str), 2)]
byte_array = np.array([int(byte, 16) for byte in hex_bytes], dtype=np.uint8)

# === Преобразуем в int16 (Little Endian) и формируем комплексные точки ===
int16vals = np.frombuffer(byte_array.tobytes(), dtype=np.int16)
real_parts = int16vals[::2].astype(float)
imag_parts = int16vals[1::2].astype(float)
complex_signal = real_parts + 1j * imag_parts

# === FFT и перевод в dBm ===
fft_result = np.fft.fftshift(np.fft.fft(complex_signal))
fft_mag = np.abs(fft_result)
fft_dBm = 20 * np.log10(fft_mag + 1e-12)  # Добавим защиту от log(0)

# === Частотная ось ===
N = len(fft_dBm)
f_start = 30      # МГц
f_end = 32        # МГц
freq = np.linspace(f_start, f_end, N)

# === Нормировка: максимум = -54 dBm ===
fft_dBm = fft_dBm - np.max(fft_dBm) - 54

# === Построение графика ===
plt.figure()
plt.plot(freq, fft_dBm, linewidth=1.5)
plt.xlabel('Частота (МГц)')
plt.ylabel('Амплитуда (dBm)')
plt.title('Спектр FFT (complex int16 LE)')
plt.grid(True)
plt.show()