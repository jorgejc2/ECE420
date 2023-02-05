import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

N = 256;                 # length of test signals
num_freqs = 100;         # number of frequencies to test

# Generate vector of frequencies to test
omega = np.pi/8 + np.linspace(0,num_freqs-1,num_freqs)/num_freqs*np.pi/4;


S = np.zeros([N,num_freqs]);                        # matrix to hold FFT results
S_padded = np.zeros([N*4, num_freqs])               # matrix to hold zero padded FFT results

for i in range(0,len(omega)):                       # loop through freq. vector
    s = np.sin(omega[i]*np.linspace(0,N-1,N));      # generate test sine wave
    s_padded = np.append(s, np.zeros(N*3))          # zero pad data
    win = signal.boxcar(N);                         # use rectangular window
    win_padded = signal.boxcar(N*4)
    s = s*win;                                      # multiply input by window
    s_padded = s_padded * win_padded
    S[:,i] = np.square(np.abs(np.fft.fft(s)));      # generate magnitude of FFT
    S_padded[:,i] = np.square(np.abs(np.fft.fft(s_padded)))
                                                    # and store as a column of S
plt.figure(figsize=(30,20))
plt.subplot(211)
plt.plot(S)                                         # plot all spectra on same graph
plt.title("Rectangular Window with 256 Samples")
plt.subplot(212)
plt.plot(S_padded)
plt.title("Rectangular Window Zero Padded to 1024 Samples")
plt.show()

# Hamming window with and without zero-padding
N = 256;                 # length of test signals
num_freqs = 100;         # number of frequencies to test

# Generate vector of frequencies to test
omega = np.pi/8 + np.linspace(0,num_freqs-1,num_freqs)/num_freqs*np.pi/4;


S = np.zeros([N,num_freqs]);                        # matrix to hold FFT results
S_padded = np.zeros([N*4, num_freqs])               # matrix to hold zero padded FFT results

for i in range(0,len(omega)):                       # loop through freq. vector
    s = np.sin(omega[i]*np.linspace(0,N-1,N));      # generate test sine wave
    s_padded = np.append(s, np.zeros(N*3))          # zero pad data
    win = signal.hamming(N);                         # use rectangular window
    win_padded = signal.hamming(N*4)
    s = s*win;                                      # multiply input by window
    s_padded = s_padded * win_padded
    S[:,i] = np.square(np.abs(np.fft.fft(s)));      # generate magnitude of FFT
    S_padded[:,i] = np.square(np.abs(np.fft.fft(s_padded)))
                                                    # and store as a column of S
plt.figure(figsize=(30,20))
plt.subplot(211)
plt.plot(S)                                         # plot all spectra on same graph
plt.title("Rectangular Window with 256 Samples")
plt.subplot(212)
plt.plot(S_padded)
plt.title("Rectangular Window Zero Padded to 1024 Samples")
plt.show()