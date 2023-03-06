# ECE420 - Spring2017
# Lab6 - Part 1: Histogram Equilization

import numpy as np
import imageio
import matplotlib.pyplot as plt
import copy

# Implement This Function
def histeq(pic):
    # Follow the procedures of Histogram Equalizaion
    # Modify the pixel value of pic directly

    N,M = pic.shape
    L = 65535
    hist = np.zeros(L) # histogram
    # cdf = np.zeros(L)
    
    # fill in the histogram
    for i in range(pic.shape[0]):
        for j in range(pic.shape[1]):
            hist[pic[i,j]] += 1

    cdf_min = 0
    for i in range(len(hist)):
        # fill in the cdf
        if i == 0:
            hist[i] = hist[i]
        else:
            hist[i] = hist[i] + hist[i - 1]

        # find cdf_min
        if (hist[i] != 0) and (cdf_min == 0):
            cdf_min = hist[i]
    
    # normalize the histogram
    for i in range(len(hist)):
        hist[i] = int(((hist[i] - cdf_min)/(M*N - 1))*(L-1))

    # use equalized histogram to redistribute values in the original image
    for i in range(pic.shape[0]):
        for j in range(pic.shape[1]):
            pic[i,j] = hist[pic[i,j]]

    return pic;

# Histogram Equilization
eco_origin = imageio.imread('eco.tif');
eco_histeq = copy.deepcopy(eco_origin);
# Call to histeq to perform Histogram Equilization
eco_histeq = histeq(eco_histeq);
# Show the result in two windows
fig_eco_origin = plt.figure(1);
fig_eco_origin.suptitle('Original eco.tif', fontsize=14, fontweight='bold');
plt.imshow(eco_origin,cmap='gray',vmin = 0, vmax = 65535);
fig_eco_histeq = plt.figure(2)
fig_eco_histeq.suptitle('Histrogram Equalized eco.tif', fontsize=14, fontweight='bold');
plt.imshow(eco_histeq,cmap='gray',vmin = 0, vmax = 65535);
plt.show()