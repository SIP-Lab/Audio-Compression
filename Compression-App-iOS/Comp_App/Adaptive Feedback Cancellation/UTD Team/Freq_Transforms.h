#ifndef FFT_H
#define FFT_H
#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <math.h>

typedef struct Transform {
	int points;
	float* sine, *cosine, *real;
	float* imaginary, *fftmagoutput, *powerspec;
} Transform;

Transform* newTransform(int points);
void transformMagnitude(Transform* transform);
void powerspec(Transform* transform);
void FFT(Transform* fft, float* input, int inputlength);
void IFFT(Transform* ifft, float* real, float* imaginary);
void destroyTransform(Transform** transform);

#endif
