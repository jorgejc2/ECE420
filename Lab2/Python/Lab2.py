import numpy as np
import matplotlib.pyplot as plt
from scipy import signal


# Your filter design here
# firls() can be called via signal.firls()
fs = 48000
numtaps = 151
# low_band = [1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000]
# low_desired = [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1]
# bands = [0, 1000, 1250, 1500, 1750, 2000, 2100, 24000]
bands = [0, 1000, 1000, 1100, 1100, 1200, 1200, 1300, 1300, 1400, 1400, 1500, 1500, 1600, 1600, 1700, 1700, 1800, 1800, 1900, 1900, 2000, 2000, 24000]
print(len(bands))
desired = [1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1]
b = signal.firls(numtaps, bands, desired, weight=None, nyq=None, fs=fs)

coef_str = "{" 
for val in b: 
    coef_str += str(val) + ", " 
coef_str = coef_str[:-2] 
coef_str += "};" 
print(coef_str) 

# Signal analysis
w, h = signal.freqz(b)
# w, h = signal.freqz(b, fs=fs)

plt.figure(figsize=(13,6))
plt.subplot(2,1,1)
plt.title('Digital filter frequency response, N = ' + str(len(b)))
plt.plot(w / np.pi, 20 * np.log10(abs(h)), 'b')
# plt.plot(w, 20 * np.log10(abs(h)), 'b')
plt.ylabel('Amplitude [dB]', color='b')
plt.grid()
plt.axis('tight')

plt.subplot(2,1,2)
angles = np.unwrap(np.angle(h))
plt.plot(w / np.pi, angles, 'g')
plt.ylabel('Angle (radians)', color='g')
plt.grid()
plt.axis('tight')
plt.xlabel('Frequency [0 to Nyquist Hz, normalized]')
plt.show()

F_s = 48000
t = [i / F_s for i in range(2 * F_s)]

test_data = signal.chirp(t, 1, t[-1], 24000, method='logarithmic')
filtered_data = np.zeros(len(test_data))

circular_buffer = np.zeros(numtaps)
pointer = 0

for i in range(len(test_data)):
    circular_buffer[pointer] = test_data[i]

    sum = 0
    for j in range(numtaps):
        sum += b[j] * circular_buffer[ (pointer - j)%numtaps]

    filtered_data[i] = sum
    pointer = (pointer + 1) % len(circular_buffer)

