//
// Created by daran on 1/12/2017 to be used in ECE420 Sp17 for the first time.
// Modified by dwang49 on 1/1/2018 to adapt to Android 7.0 and Shield Tablet updates.
//

#include <jni.h>
#include "ece420_main.h"
#include "ece420_lib.h"
#include "kiss_fft/kiss_fft.h"

// Declare JNI function
extern "C" {
JNIEXPORT void JNICALL
Java_com_ece420_lab3_MainActivity_getFftBuffer(JNIEnv *env, jclass, jobject bufferPtr);
}

// FRAME_SIZE is 1024 and we zero-pad it to 2048 to do FFT
#define FRAME_SIZE 1024
#define ZP_FACTOR 2
#define FFT_SIZE (FRAME_SIZE * ZP_FACTOR)
// Variable to store final FFT output
#define PI 3.141592653589793
float fftOut[FRAME_SIZE*2] = {}; // publishing 2 FFTs
bool isWritingFft = false;

// initialize a hamming window once to save comutation time
float hammingWindow[FRAME_SIZE];
bool hammingWindowInitialized = false;

float prev_samples[FRAME_SIZE / 2] = {}; // initialize previous samples to 0

// initalize kiss_fft parameters
kiss_fft_cfg kcfg;
kiss_fft_cpx fin[FFT_SIZE];
kiss_fft_cpx fout_1[FFT_SIZE]; // overlapped fft
kiss_fft_cpx fout_2[FFT_SIZE]; // only current samples fft


// function declarations
void generateHamming();

void ece420ProcessFrame(sample_buf *dataBuf) {
    isWritingFft = false;

    // Keep in mind, we only have 20ms to process each buffer!
    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);

    // Data is encoded in signed PCM-16, little-endian, mono channel
    float bufferIn[FRAME_SIZE];
    for (int i = 0; i < FRAME_SIZE; i++) {
        int16_t val = ((uint16_t) dataBuf->buf_[2 * i]) | (((uint16_t) dataBuf->buf_[2 * i + 1]) << 8);
        bufferIn[i] = (float) val;
    }

    // Spectrogram is just a fancy word for short time fourier transform
    // 1. Apply hamming window to the entire FRAME_SIZE
    // 2. Zero padding to FFT_SIZE = FRAME_SIZE * ZP_FACTOR
    // 3. Apply fft with KISS_FFT engine
    // 4. Scale fftOut[] to between 0 and 1 with log() and linear scaling
    // NOTE: This code block is a suggestion to get you started. You will have to
    // add/change code outside this block to implement FFT buffer overlapping (extra credit part).
    // Keep all of your code changes within java/MainActivity and cpp/ece420_*
    // ********************* START YOUR CODE HERE *********************** //

    /* initialize hamming window if not done so already */
    if (!hammingWindowInitialized) {
        /* fill in Hamming array */
        generateHamming();
        /* zero pad fin starting from idx 1024*/
        for (int i = FRAME_SIZE; i < FFT_SIZE; i++) {
            fin[i].r = 0.0;
            fin[i].i = 0.0;
        }
        kcfg = kiss_fft_alloc(FFT_SIZE,0, nullptr, nullptr);
        hammingWindowInitialized = true;
    }

    /* compute first fft which uses half of the previous samples and half of the new samples */

    /* load samples and apply window */
    for (int i = 0; i < FRAME_SIZE / 2; i++) {
        fin[i].r = prev_samples[i] * hammingWindow[i];
        fin[i].i = 0.0;
        fin[i + (FRAME_SIZE/2)].r = bufferIn[i] * hammingWindow[i + (FRAME_SIZE/2)];
        fin[i + (FRAME_SIZE/2)].i = 0.0;
    }

    /* compute first FFT */
    kiss_fft(kcfg,fin,fout_1);

    /* compute second fft which only uses the current samples */

    /* load samples, update prev_samples, and apply window */
    for (int i = 0; i < FRAME_SIZE; i++) {
        /* update prev_samples */
        if (i < (FRAME_SIZE/2))
            prev_samples[i] = bufferIn[i + (FRAME_SIZE/2)];
        /* fill in fft in buffer and apply window */
        fin[i].r = bufferIn[i] * hammingWindow[i];
        fin[i].i = 0.0;
    }

    /* compute second FFT */
    kiss_fft(kcfg,fin,fout_2);

    // thread-safe
    isWritingFft = true;
    // Currently set everything to 0 or 1 so the spectrogram will just be blue and red stripped

    /* write results of first fft */
    float max_val = 0.0;
    for (int i = 0; i < FRAME_SIZE; i++) {
        fftOut[i] = log10(fout_1[i].r*fout_1[i].r + fout_1[i].i*fout_1[i].i);
        if (fftOut[i] > max_val)
            max_val = fftOut[i];
    }
    /* linearly scale the values so that it is between 0 and 1, also only write the real values back */
    for (int i = 0; i < FRAME_SIZE; i++) {
        fftOut[i] = fftOut[i] / max_val;
    }

    /* write result of second fft */
    max_val = 0.0;
    for (int i = 0; i < FRAME_SIZE; i++) {
        fftOut[i + FRAME_SIZE] = log10(fout_2[i].r*fout_2[i].r + fout_2[i].i*fout_2[i].i);
        if (fftOut[i + FRAME_SIZE] > max_val)
            max_val = fftOut[i + FRAME_SIZE];
    }
    /* linearly scale the values so that it is between 0 and 1, also only write the real values back */
    for (int i = 0; i < FRAME_SIZE; i++) {
        fftOut[i + FRAME_SIZE] = fftOut[i + FRAME_SIZE] / max_val;
    }

    // ********************* END YOUR CODE HERE ************************* //
    // Flip the flag so that the JNI thread will update the buffer
    isWritingFft = false;

    gettimeofday(&end, NULL);
    LOGD("Time delay: %ld us, buf size %d, cap size %d",  ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)), dataBuf->size_,dataBuf->cap_);
}

void generateHamming() {
    for (int i = 0; i < FRAME_SIZE; i++) {
        hammingWindow[i] = 0.54 - 0.46 * cos((2 * PI * i)/(FRAME_SIZE - 1));
    }
}

// http://stackoverflow.com/questions/34168791/ndk-work-with-floatbuffer-as-parameter
JNIEXPORT void JNICALL
Java_com_ece420_lab3_MainActivity_getFftBuffer(JNIEnv *env, jclass, jobject bufferPtr) {
    jfloat *buffer = (jfloat *) env->GetDirectBufferAddress(bufferPtr);
    // thread-safe, kinda
    while (isWritingFft) {}
    // We will only fetch up to FRAME_SIZE data in fftOut[] to draw on to the screen
    for (int i = 0; i < FRAME_SIZE*2; i++) {
        buffer[i] = fftOut[i];
    }
}

