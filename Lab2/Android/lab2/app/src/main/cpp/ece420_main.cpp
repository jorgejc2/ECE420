//
// Created by daran on 1/12/2017 to be used in ECE420 Sp17 for the first time.
// Modified by dwang49 on 1/1/2018 to adapt to Android 7.0 and Shield Tablet updates.
//

#include "ece420_main.h"

// Student Variables
#define FRAME_SIZE 128

// FIR Filter Function Defined here located at the bottom
int16_t firFilter(int16_t sample);

/* global variable holding a mask */
int16_t lower_mask = 0x00FF;

        void ece420ProcessFrame(sample_buf *dataBuf) {
    // Keep in mind, we only have a small amount of time to process each buffer!
    struct timeval start;
    gettimeofday(&start, NULL);

    // Using {} initializes all values in the array to zero
    int16_t bufferIn[FRAME_SIZE] = {};
    int16_t bufferOut[FRAME_SIZE] = {};

    // Your buffer conversion (unpacking) here
    // Fetch data sample from dataBuf->buf_[], unpack and put into bufferIn[]
    // ******************** START YOUR CODE HERE ******************** //

    /* unpack two 8 bit values as one 16 bit samples */
    for (int i = 0; i < FRAME_SIZE*2; i+=2) {
        bufferIn[i/2] = dataBuf->buf_[i + 1] << 8 | dataBuf->buf_[i];
    }

    // ********************* END YOUR CODE HERE ********************* //

    // Loop code provided as a suggestion. This loop simulates sample-by-sample processing.
    for (int sampleIdx = 0; sampleIdx < FRAME_SIZE; sampleIdx++) {
        // Grab one sample from bufferIn[]
        int16_t sample = bufferIn[sampleIdx];
        // Call your filFilter funcion
        int16_t output = firFilter(sample);
        // Grab result and put into bufferOut[]
        bufferOut[sampleIdx] = output;
    }

    // Your buffer conversion (packing) here
    // Fetch data sample from bufferOut[], pack them and put back into dataBuf->buf_[]
    // ******************** START YOUR CODE HERE ******************** //

    /* Note that buffer is little endian */
    for (int i = 0; i < FRAME_SIZE*2; i+= 2) {
        /* get the lowest 8 bits and store them */
        dataBuf->buf_[i] = lower_mask & bufferOut[i/2];
        /* right shift to get upper 8 bits */
        dataBuf->buf_[i+1] = bufferOut[i/2] >> 8;
    }

    // ********************* END YOUR CODE HERE ********************* //

	// Log the processing time to Android Monitor or Logcat window at the bottom
    struct timeval end;
    gettimeofday(&end, NULL);
    LOGD("Loop timer: %ld us",  ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

}

// TODO: Change N_TAPS to match your filter design
#define N_TAPS 71
// TODO: Change myfilter to contain the coefficients of your designed filter.
double myfilter[N_TAPS] = {0.005396031471292018, 0.005124158617769834, 0.004342393747667682, 0.003130440953013723, 0.0016225450825625503, -1.444300076318857e-18, -0.001521456400901391, -0.0027086255520274213, -0.0033310219131378617, -0.003182674968576813, -0.002103741293043269, -1.483983642745592e-19, 0.003141638038132469, 0.0072432505776113525, 0.012132913515885387, 0.017549343795154566, 0.02315431249063755, 0.02855231422962516, 0.03331634014026463, 0.037018030271488624, 0.03926002999427127, 0.03970808680477073, 0.03812032996498682, 0.03437129044705291, 0.028468539988971742, 0.02056033516890685, 0.010933308207510045, -4.2843512410515336e-18, -0.011723177600623502, -0.02364619876172064, -0.03514187479300596, -0.045585948767361426, -0.054397951293508454, -0.06107982731466832, -0.06524947165805225, 0.9333333333333331, -0.06524947165805225, -0.06107982731466832, -0.054397951293508454, -0.045585948767361426, -0.03514187479300596, -0.02364619876172064, -0.011723177600623502, -4.2843512410515336e-18, 0.010933308207510045, 0.02056033516890685, 0.028468539988971742, 0.03437129044705291, 0.03812032996498682, 0.03970808680477073, 0.03926002999427127, 0.037018030271488624, 0.03331634014026463, 0.02855231422962516, 0.02315431249063755, 0.017549343795154566, 0.012132913515885387, 0.0072432505776113525, 0.003141638038132469, -1.483983642745592e-19, -0.002103741293043269, -0.003182674968576813, -0.0033310219131378617, -0.0027086255520274213, -0.001521456400901391, -1.444300076318857e-18, 0.0016225450825625503, 0.003130440953013723, 0.004342393747667682, 0.005124158617769834, 0.005396031471292018};

// Circular Buffer
int16_t circBuf[N_TAPS] = {};
int16_t circBufIdx = 0;

// FirFilter Function
int16_t firFilter(int16_t sample) {
    // This function simulates sample-by-sample processing. Here you will
    // implement an FIR filter such as:
    //
    // y[n] = a x[n] + b x[n-1] + c x[n-2] + ...
    //
    // You will maintain a circular buffer to store your prior samples
    // x[n-1], x[n-2], ..., x[n-k]. Suggested initializations circBuf
    // and circBufIdx are given.
    //
    // Input 'sample' is the current sample x[n].
    // ******************** START YOUR CODE HERE ******************** //
    int16_t output = 0;
    double inter = 0.0; // to preserve accuracy during the calculation

    /* insert sample into buffer */
    circBuf[circBufIdx] = sample;

    /* perform convolution */
    for (int i = 0; i < N_TAPS; i++) {
        inter += myfilter[i] * circBuf[ (circBufIdx - i) % N_TAPS];
    }
    output = (int16_t)inter;
    /* update pointer */
    circBufIdx = (circBufIdx + 1) % N_TAPS;

    // ********************* END YOUR CODE HERE ********************* //
    return output;
}
