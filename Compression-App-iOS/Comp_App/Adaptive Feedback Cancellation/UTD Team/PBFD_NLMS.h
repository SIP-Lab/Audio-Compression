#ifndef _PBFD_NLMS_H_
#define _PBFD_NLMS_H_
#define _USE_MATH_DEFINES

#include "stdlib.h"
#include "math.h"
#include "Freq_Transforms.h"
#include "Levinson.h"
#include "constants.h"
//#include "Timer.h"

typedef struct PBFD_NLMS{
	float Mu, G;
	int u_bufferlen, dec, id, sub_filters, dec1, index2;
	int P, L, M, N,fftsize, filter_length, AR_order, gain, gain_delay;

	float *e, *z, *u, *y, *u_f, *y_f,  *E_f, *vec_u, *vec_uf, *buf_u, *buf_y, *buf_u_fb ;
	float *zp_real, *zp_image, *zfp_real, *zfp_image;
	float *F_hat_real_temp, *F_hat_image_temp, *f_hat;
	float *power, *delta;
	float *temp4, *temp5, *temp6, *temp7, *temp9_real, *temp9_image;

	float **g, **temp_mat, **temp_mat_t, **F_hat_real, **F_hat_image;
	float ***U_f_real, ***U_f_image;

	float *u_buffer, *u_buffer1, *y_buffer, *u_f_buffer,*e_buffer, *in;
	float *last_u_buffer1, *last_y_buffer;
	float *A, *A_previous;
	float *MISA, *MISA_db;

	LEVINSON *doLevinson;
	//Timer* t1;
	Transform *doFFT, *doFFT1, *doFFT3, *doFFT4, *doFFT5, *doFFT6;
	Transform *doIFFT, *doIFFT1, *doIFFT2, *doIFFT3 ;

} PBFD_NLMS;

PBFD_NLMS* newPBFD(int L, int M, int N, float Mu, float G, int filter_length, int AR_order, int gain, int gain_delay);
void processfirstframe(PBFD_NLMS* PBFD, float* input);
void computePBFD(PBFD_NLMS* PBFD, float* input,int n);
float norm_vector(float* real, float* image, int m);
void complex_mul_matrix(float** ans_real, float** ans_image, float** a_real, float** a_image, float** b_real, float** b_image, int a_rows, int a_col, int b_col);
float mag2db(float x);
void destroyPBFD(PBFD_NLMS** pbfd);
#endif
