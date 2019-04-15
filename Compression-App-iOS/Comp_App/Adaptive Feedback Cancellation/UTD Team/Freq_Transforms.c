#include "Freq_Transforms.h"

Transform*
newTransform(int points) {
	Transform* newTransform = (Transform*)malloc(sizeof(Transform));
	newTransform->points = points;
	newTransform->real = (float*)malloc(points * sizeof(float));
	newTransform->imaginary = (float*)malloc(points * sizeof(float));
	newTransform->sine = NULL;
	newTransform->cosine = NULL;
	newTransform->fftmagoutput = (float*)calloc(points, sizeof(float));
	newTransform->powerspec = (float*)calloc(points, sizeof(float));
	newTransform->sine = (float*)malloc((points / 2) * sizeof(float));
	newTransform->cosine = (float*)malloc((points / 2) * sizeof(float));

	//precompute twiddle factors
	float arg;
	int i;
	for (i = 0; i < points / 2; i++) {
		arg = -2 * M_PI * i / points;
		newTransform->cosine[i] = cos(arg);
		newTransform->sine[i] = sin(arg);
	}
	return newTransform;
}

void
FFT(Transform* fft, float* input, int inputlength) {
	int i, j, k, L, m, n, o, p, q, r;
	float tempReal, tempImaginary, cos, sin, xt, yt;
	k = fft->points;

	for (i = 0; i < k; i++) {
		fft->real[i] = 0;
		fft->imaginary[i] = 0;
	}
	for (i = 0; i < inputlength; i++) {
		fft->real[i] = input[i];
	}
	j = 0;
	m = k / 2;
	//bit reversal
	for (i = 1; i < (k - 1); i++) {
		L = m;

		while (j >= L) {
			j = j - L;
			L = L / 2;
		}

		j = j + L;

		if (i < j) {
			tempReal = fft->real[i];
			tempImaginary = fft->imaginary[i];
			fft->real[i] = fft->real[j];
			fft->imaginary[i] = fft->imaginary[j];
			fft->real[j] = tempReal;
			fft->imaginary[j] = tempImaginary;
		}
	}

	L = 0;
	m = 1;
	n = k / 2;

	//computation
	for (i = k; i > 1; i = (i >> 1)) {
		L = m;
		m = 2 * m;
		o = 0;

		for (j = 0; j < L; j++) {
			cos = fft->cosine[o];
			sin = fft->sine[o];
			o = o + n;

			for (p = j; p < k; p = p + m) {
				q = p + L;

				xt = cos * fft->real[q] - sin * fft->imaginary[q];
				yt = sin * fft->real[q] + cos * fft->imaginary[q];
				fft->real[q] = (fft->real[p] - xt);
				fft->imaginary[q] = (fft->imaginary[p] - yt);
				fft->real[p] = (fft->real[p] + xt);
				fft->imaginary[p] = (fft->imaginary[p] + yt);
			}
		}
		n = n >> 1;
	}
}

void
IFFT(Transform* ifft, float* real, float* imaginary) {
	int i, j, k, L, m, n, o, p, q, r;
	float tempReal, tempImaginary, cos, sin, xt, yt;
	k = ifft->points;
	for (i = 0; i<k; i++){
		ifft->real[i] = real[i];
		ifft->imaginary[i] = imaginary[i];
	}
	j = 0;
	m = k / 2;

	//bit reversal
	for (i = 1; i < (k - 1); i++) {
		L = m;

		while (j >= L) {
			j = j - L;
			L = L / 2;
		}

		j = j + L;

		if (i < j) {
			tempReal = ifft->real[i];
			tempImaginary = ifft->imaginary[i];
			ifft->real[i] = ifft->real[j];
			ifft->imaginary[i] = ifft->imaginary[j];
			ifft->real[j] = tempReal;
			ifft->imaginary[j] = tempImaginary;
		}
	}

	L = 0;
	m = 1;
	n = k / 2;

	//computation
	for (i = k; i > 1; i = (i >> 1)) {
		L = m;
		m = 2 * m;
		o = 0;

		for (j = 0; j < L; j++) {
			cos = ifft->cosine[o];
			sin = -ifft->sine[o];
			o = o + n;

			for (p = j; p < k; p = p + m) {
				q = p + L;

				xt = cos * ifft->real[q] - sin * ifft->imaginary[q];
				yt = sin * ifft->real[q] + cos * ifft->imaginary[q];
				ifft->real[q] = (ifft->real[p] - xt);
				ifft->imaginary[q] = (ifft->imaginary[p] - yt);
				ifft->real[p] = (ifft->real[p] + xt);
				ifft->imaginary[p] = (ifft->imaginary[p] + yt);
			}
		}
		n = n >> 1;
	}
	for (i = 0; i<k; i++){
		ifft->real[i] /= k; //IFFT Scaling
		//ifft->imaginary[i] /= k;
	}
}

void
transformMagnitude(Transform* transform) {
	int n;
	for (n = 0; n < transform->points; n++) {
		transform->fftmagoutput[n] = sqrt(
			transform->real[n] * transform->real[n]
			+ transform->imaginary[n] * transform->imaginary[n]);
	}
}

void
powerspec(Transform* transform){
	int n;
	for (n = 0; n < transform->points; n++){
		transform->powerspec[n] = transform->real[n] * transform->real[n]
			+ transform->imaginary[n] * transform->imaginary[n];
	}
}
void
destroyTransform(Transform** transform) {
	if (*transform != NULL) {
		if ((*transform)->cosine != NULL) {
			free((*transform)->cosine);
			(*transform)->cosine = NULL;
		}
		if ((*transform)->sine != NULL) {
			free((*transform)->sine);
			(*transform)->sine = NULL;
		}
		if ((*transform)->real != NULL) {
			free((*transform)->real);
			(*transform)->real = NULL;
		}
		if ((*transform)->fftmagoutput != NULL) {
			free((*transform)->fftmagoutput);
			(*transform)->fftmagoutput = NULL;
		}
		if ((*transform)->imaginary != NULL) {
			free((*transform)->imaginary);
			(*transform)->imaginary = NULL;
		}
		free(*transform);
		*transform = NULL;
	}
}
