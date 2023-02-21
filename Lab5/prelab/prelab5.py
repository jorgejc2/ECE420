from scipy import signal
from scipy.io.wavfile import read
from IPython.display import Audio

Fs, data = read('test_audio.wav')
data = data[:, 0]

up_ratio = int(100*Fs)
down_ratio = int(50*Fs)

output = signal.resample_poly(data, up_ratio, down_ratio)
# output = np.fft.ifft(output)
Audio(output, rate=Fs)