import numpy as np
from numpy.fft import fft
import matplotlib.pyplot as plt
import scipy.io.wavfile as spwav
#from mpldatacursor import datacursor
import sys

plt.style.use('ggplot')

# Note: this epoch list only holds for "test_vector_all_voiced.wav"
epoch_marks_orig = np.load("test_vector_all_voiced_epochs.npy")
F_s, audio_data = spwav.read("test_vector_all_voiced.wav")
N = len(audio_data)

######################## YOUR CODE HERE ##############################

F_new = 420
new_epoch_spacing = int(F_s / F_new)
# new_epochs = np.arange(new_epoch_spacing, len(audio_data), new_epoch_spacing)

audio_out = np.zeros(N)

print(epoch_marks_orig)
plt.figure()
plt.subplot(121)
plt.plot(audio_data)
plt.scatter(epoch_marks_orig, audio_data[epoch_marks_orig], c='blue')
# plt.show()

# Suggested loop
curr_epoch = 0 # idx of epoch in original epoch array, not idx of epoch in original data
for i in range(0, N, new_epoch_spacing):

    # https://courses.engr.illinois.edu/ece420/lab5/lab/#overlap-add-algorithm
    # Your OLA code here
    first_epoch_idx = epoch_marks_orig[curr_epoch]
    second_epoch_idx = epoch_marks_orig[curr_epoch + 1] if curr_epoch < len(epoch_marks_orig) - 1 else -1
    curr_epoch_idx = 0 # corresponds to the original epoch's sample idx in the original audio data

    # determine based on distance if we need to map to a new epoch
    if abs(first_epoch_idx - i) < abs(second_epoch_idx - i):
        curr_epoch_idx = first_epoch_idx
    else:
        curr_epoch_idx = second_epoch_idx
        # curr_epoch += 1
    
    left_epoch_idx = 0
    right_epoch_idx = 0
    # calculate P0 for current original epoch
    if curr_epoch == 0:
        left_epoch_idx = 0
    else:
        left_epoch_idx = epoch_marks_orig[curr_epoch - 1]
    
    if curr_epoch == len(epoch_marks_orig)-1:
        right_epoch_idx = len(audio_data)-1
    else:
        right_epoch_idx = epoch_marks_orig[curr_epoch + 1]

    p0 = int((right_epoch_idx - left_epoch_idx) / 2)

    # now apply hanning window and add to output
    window_len = int(2*p0 + 1)
    window = [0.5*(1 - np.cos((2*np.pi*l) / window_len)) for l in range(window_len)]
    for j in range(curr_epoch_idx - p0, curr_epoch_idx + p0 + 1, 1):
        windowed_idx = j - (curr_epoch_idx - p0)
        audio_out[j] += window[windowed_idx] * audio_data[j]

    if curr_epoch_idx == second_epoch_idx:
        curr_epoch += 1

plt.subplot(122)
plt.plot(audio_out)
plt.scatter(epoch_marks_orig, audio_out[epoch_marks_orig], c='blue')
plt.show()