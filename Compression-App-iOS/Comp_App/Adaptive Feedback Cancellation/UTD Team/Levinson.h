#ifndef _LEVINSON_H_
#define _LEVINSON_H_
#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <math.h>
#include "Freq_Transforms.h"

typedef struct LEVINSON{
	int p, nfft,L, N, maxlag, no_lags,AR_order;
	float  G, *K, *E, *R, *A;
	float *alpha, *pre_alpha, *R_reverse, *xcorr;
	Transform* doFFT,* doFFT1, *doIFFT;
}LEVINSON;

LEVINSON* newLevinson(int filter_order, int N);
void computeLevinson(LEVINSON* levinson, float* input);
void destroyLevinson(LEVINSON** levinson);

#endif
