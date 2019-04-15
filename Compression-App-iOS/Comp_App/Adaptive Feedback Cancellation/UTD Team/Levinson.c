#include "Levinson.h"

LEVINSON*
newLevinson(int AR_order, int N) {
	LEVINSON* newLevinson = (LEVINSON*)malloc(sizeof(LEVINSON));
	// N : size of an input
	// Check the MATLAB functions Levinson_alg and xcorr functions for details
	// Results are stored as discussed below: (MATLAB function gives 4 outputs: alpha,G,A,H)
	// alpha : levinson->alpha
	// G     : levinson->G
	// A	 : levinson->A
	// H	 : Not needed in the code : ignored to reduce the processing time
	newLevinson->nfft = 2*powf(2, ceil(logf(N) / logf(2))); //for L = 4 -> nfft is 8 point
	newLevinson->doFFT = newTransform(newLevinson->nfft);
	newLevinson->doIFFT = newTransform(newLevinson->nfft);
	newLevinson->N = N;
	newLevinson->AR_order = AR_order;
	newLevinson->maxlag = N - 1;
	newLevinson->no_lags = 2*N - 1;		// total number of lags of interest
	newLevinson->K = (float*)calloc(AR_order,sizeof(float));
	newLevinson->E = (float*)calloc(AR_order,sizeof(float));
	newLevinson->R = (float*)calloc(AR_order,sizeof(float));
	newLevinson->R_reverse = (float*)calloc(AR_order,sizeof(float));
	newLevinson->alpha = (float*)calloc(AR_order,sizeof(float));
	newLevinson->pre_alpha = (float*)calloc(AR_order,sizeof(float));
	newLevinson->A = (float*)calloc((AR_order + 1),sizeof(float));
	newLevinson->xcorr = (float*)calloc(newLevinson->no_lags, sizeof(float));
	return newLevinson;
}

void computeLevinson(LEVINSON* levinson, float* input){
	int i, j, k, m, n;
	float sum = 0.0f;

	// Autocorrelation Using FFT - see the MATLAB function xcorr code for details
	// This liens of code preforms autocorrelation on Vector(array) only
	//input[159] = -0.139904022216797;
	FFT(levinson->doFFT, input, levinson->N);
	powerspec(levinson->doFFT);
	for (i = 0; i < levinson->nfft; i++){
		levinson->doFFT->imaginary[i] = 0.0f;
	}
	IFFT(levinson->doIFFT, levinson->doFFT->powerspec, levinson->doFFT->imaginary);
	int id = levinson->nfft - levinson->N + 1;
	for (i = 0; i < levinson->maxlag; i++){
		levinson->xcorr[i] = levinson->doIFFT->real[id + i];
	}
	for (i = levinson->maxlag; i < levinson->no_lags; i++){
		levinson->xcorr[i] = levinson->doIFFT->real[i - levinson->maxlag];
	}
	// Autocorrelation Ends - Result is stored in levinson->xcorr

	int idx = levinson->N + levinson->AR_order - 1;
	for (i = idx, j = levinson->AR_order-1; i >= levinson->N; i--, j--){
		levinson->R[j] = levinson->xcorr[i];
		levinson->R_reverse[idx-i] = levinson->xcorr[i];
	}
	float E0 = levinson->xcorr[levinson->N - 1];
	for (i = 0; i < levinson->AR_order; i++){
		if (i == 0){
			levinson->K[i] = levinson->R[i] / E0;
			levinson->alpha[i] = levinson->K[i];
			levinson->E[i] = (float)(E0 * (1 - (levinson->K[i])*(levinson->K[i])));
		}
		else{
			for (k = 0, j = levinson->AR_order-i ; k < i, j<levinson->AR_order ; k++,j++){
				sum = sum + levinson->pre_alpha[k] * levinson->R_reverse[j];
			}
			levinson->K[i] = (levinson->R[i] - sum) / levinson->E[i - 1];
			sum = 0.0f;
			levinson->alpha[i] = levinson->K[i];
			for (j = 0; j < i ; j++){
				levinson->alpha[j] = levinson->pre_alpha[j] - (levinson->K[i] * levinson->pre_alpha[i - j - 1]);
			}
			levinson->E[i] = (float)(levinson->E[i - 1] * (1 - (levinson->K[i])*(levinson->K[i])));
		}
		for (j = 0; j < levinson->AR_order; j++){
			levinson->pre_alpha[j] = levinson->alpha[j];
		}
	}
	levinson->G = sqrt(levinson->E[levinson->AR_order-1]);
	levinson->A[0] = 1;
	for (i = 0; i<levinson->AR_order; i++){
		levinson->A[i + 1] = (-1)*levinson->alpha[i];
	}
}

void destroyLevinson(LEVINSON** levinson){
	if (*levinson != NULL){
		if ((*levinson)->alpha != NULL){
			free((*levinson)->alpha);
			(*levinson)->alpha = NULL;
		} if ((*levinson)->pre_alpha != NULL){
			free((*levinson)->pre_alpha);
			(*levinson)->pre_alpha = NULL;
		} if ((*levinson)->K != NULL){
			free((*levinson)->K);
			(*levinson)->K = NULL;
		} if ((*levinson)->E != NULL){
			free((*levinson)->E);
			(*levinson)->E = NULL;
		} if ((*levinson)->R != NULL){
			free((*levinson)->R);
			(*levinson)->R = NULL;
		}  if ((*levinson)->R_reverse != NULL){
			free((*levinson)->R_reverse);
			(*levinson)->R_reverse = NULL;
		} if ((*levinson)->A != NULL){
			free((*levinson)->A);
			(*levinson)->A = NULL;
		} if ((*levinson)->xcorr != NULL){
			free((*levinson)->xcorr);
			(*levinson)->xcorr = NULL;
		}

		destroyTransform(&(*levinson)->doFFT);
		destroyTransform(&(*levinson)->doIFFT);
		free(*levinson);
		*levinson = NULL;
	}

}
