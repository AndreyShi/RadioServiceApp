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
int16vals = byte_array.view(dtype=np.int16)
real_parts = int16vals[::2]
imag_parts = int16vals[1::2]
complex_signal = real_parts.astype(float) + 1j * imag_parts.astype(float)

# === FFT и перевод в dBm ===
fft_result = np.fft.fftshift(np.fft.fft(complex_signal))
fft_mag = (4 / len(complex_signal)) * np.abs(fft_result[28:-28])
fft_dBm = 20 * np.log10(fft_mag / 1e6)

# === Частотная ось ===
N = len(fft_dBm)
f_start = 30      # МГц
f_end = 32        # МГц
freq = np.linspace(f_start, f_end, N)

# === Построение графика ===
plt.figure()
plt.plot(freq, fft_dBm, linewidth=1.5)
plt.xlabel('Частота (МГц)')
plt.ylabel('Амплитуда (dBm)')
plt.title('Спектр FFT (complex int16 LE)')
plt.grid(True)
plt.show()