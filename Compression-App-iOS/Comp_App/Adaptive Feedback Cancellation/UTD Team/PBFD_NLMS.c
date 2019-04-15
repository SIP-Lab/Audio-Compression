#include "PBFD_NLMS.h"
#include<stdio.h>
#include<conio.h>
//#include "android/log.h"

PBFD_NLMS*newPBFD(int L, int M, int N, float Mu, float G,int filter_length,int AR_order, int gain, int gain_delay)
{
	PBFD_NLMS* newPBFD = (PBFD_NLMS*)malloc(sizeof(PBFD_NLMS));
	// L : Block Size, M : Point of FFT, N : Frame Length
	// P : Length of each Sub-band Filter
	int i,j;
	//newPBFD->t1 = newTimer();
	newPBFD->doLevinson = newLevinson(AR_order, N);
	newPBFD->doFFT = newTransform(M); newPBFD->doIFFT = newTransform(M);
	newPBFD->doFFT1 = newTransform(M); newPBFD->doIFFT1 = newTransform(M);
	newPBFD->doFFT3 = newTransform(M);
	newPBFD->doIFFT2 = newTransform(M);
	newPBFD->doFFT5 = newTransform(M);
	newPBFD->G = G;
	newPBFD->Mu = Mu;
	newPBFD->L = L;
	newPBFD->M = M;
	newPBFD->N = N;
	newPBFD->id = N/L;
	newPBFD->P = L;
	newPBFD->sub_filters = (int)(filter_length / newPBFD->P);
	newPBFD->u_bufferlen = M + filter_length - newPBFD->P;
	newPBFD->gain = gain;
	newPBFD->dec = 0;
	newPBFD->dec1 = 0;
	newPBFD->AR_order = AR_order;
	newPBFD->gain_delay = gain_delay;
	newPBFD->filter_length = filter_length;
	newPBFD->index2 = L - (gain_delay - L* floorf(gain_delay/L));

	newPBFD->e = (float*) calloc(N, sizeof(float));
	newPBFD->z = (float*) calloc(L, sizeof(float));
	newPBFD->u = (float*) calloc(N, sizeof(float));
	newPBFD->y = (float*) calloc(N, sizeof(float));
	newPBFD->u_f = (float*) calloc(N, sizeof(float));
	newPBFD->y_f = (float*) calloc(N, sizeof(float));
	newPBFD->power = (float*)calloc(newPBFD->sub_filters, sizeof(float));
	newPBFD->delta = (float*)calloc(newPBFD->sub_filters, sizeof(float));
	newPBFD->vec_u = (float*)calloc(M, sizeof(float));
	newPBFD->vec_uf = (float*)calloc(M, sizeof(float));
	newPBFD->E_f = (float*)calloc(M, sizeof(float));
	newPBFD->buf_u = (float*)calloc((AR_order + 1), sizeof(float));
	newPBFD->buf_y = (float*)calloc((AR_order + 1), sizeof(float));
	newPBFD->temp4 = (float*)calloc(L, sizeof(float));
	newPBFD->temp5 = (float*)calloc(M, sizeof(float));
	newPBFD->temp6 = (float*)calloc(M, sizeof(float));
	newPBFD->temp7 = (float*)calloc(M, sizeof(float));

	newPBFD->buf_u_fb = (float*)calloc(88, sizeof(float));
	newPBFD->zp_real = (float*)calloc(M, sizeof(float));
	newPBFD->zp_image = (float*)calloc(M, sizeof(float));
	newPBFD->zfp_real = (float*)calloc(M, sizeof(float));
	newPBFD->zfp_image = (float*)calloc(M, sizeof(float));
	newPBFD->F_hat_real_temp = (float*)calloc(M, sizeof(float));
	newPBFD->F_hat_image_temp = (float*)calloc(M, sizeof(float));
	newPBFD->f_hat = (float*)calloc(M,sizeof(float));
	newPBFD->u_buffer = (float*)calloc(newPBFD->u_bufferlen, sizeof(float));
	newPBFD->u_f_buffer = (float*)calloc(newPBFD->u_bufferlen, sizeof(float));
	newPBFD->u_buffer1 = (float*)calloc(N, sizeof(float));
	newPBFD->y_buffer = (float*)calloc(N, sizeof(float));
	newPBFD->e_buffer = (float*)calloc(gain_delay, sizeof(float));
	newPBFD->in = (float*)calloc(N,sizeof(float));
	newPBFD->last_u_buffer1 = (float*)calloc(L,sizeof(float));
	newPBFD->last_y_buffer = (float*)calloc(L,sizeof(float));
	newPBFD->A = (float*)calloc(AR_order+1,sizeof(float));
	newPBFD->A_previous = (float*)calloc(AR_order+1,sizeof(float));

	//2D  Array Dynamic Memory Allocation - Matrix g
	// g = [eye(P) zeros(P,M-p);zeros(M-P,P) zeros(M-P,M-P)];
	newPBFD->g = (float**)calloc(M, sizeof(float*));
	for (i = 0; i < M; i++) {
		newPBFD->g[i] = (float*)calloc(M, sizeof(float));
	}
	//Identity Matrix
	for (i = 0; i < newPBFD->P; i++) {
		newPBFD->g[i][i] = 1.0f;
	}

	// Matrix temp_mat= [zeros(L,M-L) eye(L)]---------------------------------------used in line 88 in matlab code
	newPBFD->temp_mat = (float**)calloc(L , sizeof(float*));
	for (i = 0; i < L; i++) {
		newPBFD->temp_mat[i] = (float*)calloc(M , sizeof(float));
	}
	for (i = 0; i < L; i++) {
		newPBFD->temp_mat[i][M-L+i] = 1.0f;
	}

	// Matrix temp_mat_t= [zeros(M-L,L);eye(L)]
	newPBFD->temp_mat_t = (float**)calloc(M , sizeof(float*));
	for (i = 0; i < M; i++) {
		newPBFD->temp_mat_t[i] = (float*)calloc(L , sizeof(float));
	}
	for (i = M - L; i < M; i++) { //  lower part : Identity matrix
		newPBFD->temp_mat_t[i][i-(M-L)] = 1.0f;
	}

	newPBFD->F_hat_real = (float**)calloc(M,sizeof(float*));
	newPBFD->F_hat_image = (float**)calloc(M,sizeof(float*));
	for (i = 0; i < M; i++) {
		newPBFD->F_hat_real[i] = (float*)calloc(newPBFD->sub_filters , sizeof(float));
		newPBFD->F_hat_image[i] = (float*)calloc(newPBFD->sub_filters , sizeof(float));
	}

	//3D Array dynamic memory allocation
	newPBFD->U_f_real = (float***)calloc(M,sizeof(float**));
	for (i = 0; i<M; i++){
		newPBFD->U_f_real[i] = (float**)calloc(M,sizeof(float*));
		for (j = 0; j<M; j++){
			newPBFD->U_f_real[i][j] = (float*)calloc(newPBFD->sub_filters, sizeof(float));
		}
	}
	newPBFD->U_f_image = (float***)calloc(M,sizeof(float**));
	for (i = 0; i<M; i++){
		newPBFD->U_f_image[i] = (float**)calloc(M,sizeof(float*));
		for (j = 0; j<M; j++){
			newPBFD->U_f_image[i][j] = (float*)calloc(newPBFD->sub_filters, sizeof(float));
		}
	}
	return newPBFD;
}




void processfirstframe(PBFD_NLMS* PBFD, float* input)
{
	int p, i, j, m, id1,id2,id3,id4 ;
	float sum_real = 0.0f, sum_image = 0.0f, sum_u = 0.0f, sum_y = 0.0f;
	float sum = 0.0f, u = 0.0f, u_f = 0.0f, y_f = 0.0f;
	int L = PBFD->L, M = PBFD->M, P = PBFD->P, N = PBFD->N, AR_order = PBFD->AR_order;

	/////////////////////////////////////////////////////////////////////
	for (m = 0; m <PBFD->id; m++)//for (m = 0; m < PBFD->id; m++)
	
 {          // Each Block has L samples and N/L number of blocks in each frame
		      //start(PBFD->t1);
		int d = 0;
		for (i = 0; i < L; i++)
		{
			if (m*L+i < PBFD->gain_delay)
			{
				PBFD->dec = 0;
				u = 0.0f;
			}
			else
			{
				PBFD->dec = 1;
				u = PBFD->gain * PBFD->e_buffer[d];
				d++;
			}
			PBFD->u[m*L + i] = u;
			//printf("u[m*L + i]   %.10f , %d\n", PBFD->u[m*L + i],           m*L + i);

			// Logic for buf_u_FB = [u buf_u_FB(1:end-1)];
			for (j = 86; j >= 0; j--)
			{
				PBFD->buf_u_fb[j + 1] = PBFD->buf_u_fb[j];
		      //  printf("buf_u_fb  %.10f , %d\n", PBFD->buf_u_fb[j + 1], j + 1);
			}

			PBFD->buf_u_fb[0] = u;

			for (j = 0; j < 88; j++)  //--------------here 88 is starkey length
			
			{
				sum = sum + PBFD->buf_u_fb[j] * starkeymodelBTE[j];
			}

			PBFD->y[m*L + i] = sum + input[m*L + i];// --------------feedback signal + desired input
			//printf("%.10f\n", PBFD->y[m*L + i]);
			sum = 0.0f;
		}
		//stop(PBFD->t1);
		//tellTime(PBFD->t1);
		for(i = 0 ; i < PBFD->u_bufferlen-L ; i++)
		{ //left shit by L
			PBFD->u_buffer[i] = PBFD->u_buffer[i+L]; //u_buffer = u2
		}
		for(i = 0 ; i < L ; i++)
		{
			PBFD->u_buffer[PBFD->u_bufferlen-L+i] = PBFD->u[m*L+i];
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(m==PBFD->id-1)
		{ //save the full PBFD->u for the future use
			for(i=0;i<N;i++)
			{
				PBFD->u_buffer1[i] = PBFD->u[i];
				PBFD->y_buffer[i] = PBFD->y[i];
			}
		}
		//////////////////////////////////////////////////////////////////////////
		for (p = 0; p < PBFD->sub_filters; p++)
		{
			for (j = 0; j < M; j++)
			{
				id1 = ((m + 1)*L - p*P - M + j);
				if (id1>= 0)
				{
					PBFD->vec_u[j] = PBFD->u[id1];
					//printf("%.10f\n ,%d\n", PBFD->vec_u[j],j);
				}
				else
				{
					PBFD->vec_u[j] = 0.0f;
					//printf("vec_u %.10f\n,%d\n", PBFD->vec_u[j], j);
				}
			}
			FFT(PBFD->doFFT, PBFD->vec_u, M);
			for (i = 0; i < M; i++) 
			{
				PBFD->zp_real[i] += (PBFD->doFFT->real[i] * PBFD->F_hat_real[i][p]) - (PBFD->doFFT->imaginary[i] * PBFD->F_hat_image[i][p]);
				PBFD->zp_image[i]+= sum_image + (PBFD->doFFT->real[i] * PBFD->F_hat_image[i][p]) + (PBFD->F_hat_real[i][p] * PBFD->doFFT->imaginary[i]);
				//printf("%.10freal\n,%d\n", PBFD->zp_real[i], i); printf("%.10fimage\n,%d\n", PBFD->zp_image[i], i);
			}
		}
		///////////////////////////////////////////////////////////////////////////
		IFFT(PBFD->doIFFT, PBFD->zp_real, PBFD->zp_image);
		sum = 0.0f;
		for (i = 0; i < L; i++)
		{
			for (j = 0; j < M; j++)
			{
				sum = sum + PBFD->temp_mat[i][j] * PBFD->doIFFT->real[j]; // IFFT won't have imaginary part for real input
				//printf("z = %.10f,%d\n", sum, j);
			}
			PBFD->z[i] = sum;//-----------------------------Output of the feedback canceller
			
 			sum = 0.0f;
		}
		for (i = 0; i < M; i++)
		{
			PBFD->zp_real[i] = 0.0f;
			PBFD->zp_image[i] = 0.0f;
		}
		//////////////////////////////////////////////////////////////////////////
		for (i = 0; i < L; i++)
		{
			PBFD->e[m*L + i] = PBFD->y[m*L + i] - PBFD->z[i];
			//printf("e = %.10f,%d\n", PBFD->e[m*L + i],       m*L + i);
		}////////////////////////////////////////////////////////////////////ask abdullah
		if (PBFD->dec == 0) 
		{
			for (i = 0; i < L; i++) 
			{
				PBFD->e_buffer[m * L + i] = PBFD->e[m * L + i];
			}
		}
		if (PBFD->dec == 1)
		{
			if (PBFD->dec1==0)
			{
				PBFD->dec1 = 1;
				for (i = 0; i < PBFD->gain_delay - PBFD->index2; i++) 
				{ //left shit by L
					PBFD->e_buffer[i] = PBFD->e_buffer[i + PBFD->index2];
				}
			}
			else
			{
				for (i = 0; i < PBFD->gain_delay - L; i++) 
				{ //left shit by L
					PBFD->e_buffer[i] = PBFD->e_buffer[i + L];
				}
			}
			for (i = 0; i < L; i++)
			{
				PBFD->e_buffer[PBFD->gain_delay - L + i] = PBFD->e[m * L + i];
			}
		}

		for (i = 0; i < L; i++)
		{
			int A_temp = floorf((m*L + i + 1) / N) ;//---------------matlab line 99
			if (A_temp < 1) {
				u_f = 0.0f;
				y_f = 0.0f;
			} else{
				for (j = 0; j < AR_order + 1; j++){
					sum_u += PBFD->A_previous[j] * PBFD->buf_u[j];
					sum_y += PBFD->A_previous[j] * PBFD->buf_y[j];
				}
				u_f = (float)((1.0 / PBFD->G) * sum_u);
				y_f = (float)((1.0 / PBFD->G) * sum_y);
				

				sum_u = 0.0f;
				sum_y = 0.0f;
			}
			if ((m*L + i + 1) % N == 0){
				computeLevinson(PBFD->doLevinson, PBFD->e);
				for (j = 0; j < AR_order + 1; j++){
					PBFD->A[j] = PBFD->doLevinson->A[j];
				}
				PBFD->G = PBFD->doLevinson->G;
			}
			PBFD->u_f[m*L + i] = u_f;
			PBFD->y_f[m*L + i] = y_f;
			//printf("u_f = %.10f,%d\n", PBFD->u_f[m*L + i], m*L + i);
		}

		for(i = 0 ; i < PBFD->u_bufferlen-L ; i++){ //left shit by L
			PBFD->u_f_buffer[i] = PBFD->u_f_buffer[i+L];
		}
		for(i = 0 ; i < L ; i++){
			PBFD->u_f_buffer[PBFD->u_bufferlen-L+i] = PBFD->u_f[m*L+i];
		}
		/////////////////////////////////////////////////////////////////////
		for (p = 0; p < PBFD->sub_filters; p++) {
			for (j = 0; j < M; j++){
				id3 = ((m + 1)*L - p*P - M + j);
				if ( id3>= 0){
					PBFD->vec_uf[j] = PBFD->u_f[id3];
					//printf("vec_uf = %.10f,%d\n", PBFD->vec_uf[j], j);
				}
				else{
					PBFD->vec_uf[j] = 0.0f;
					//printf("vec_uf = %.10f,%d\n", PBFD->vec_uf[j], j);
				}
			}
			FFT(PBFD->doFFT1, PBFD->vec_uf, M);
			for (i = 0; i < M; i++) {
				for (j = 0; j < M; j++) {
					PBFD->U_f_real[i][j][p] = 0.0f;
					PBFD->U_f_image[i][j][p] = 0.0f;
				}
			}
			for (i = 0; i < M; i++) {
				PBFD->U_f_real[i][i][p] = PBFD->doFFT1->real[i];
				PBFD->U_f_image[i][i][p] = PBFD->doFFT1->imaginary[i];
			}
			float ans = norm_vector(PBFD->doFFT1->real, PBFD->doFFT1->imaginary, M);
			PBFD->power[p] = (float)(ans*ans);
			for (i = 0; i < M; i++) {
				for (j = 0; j < M; j++) {
					sum_real += (PBFD->U_f_real[i][j][p] * PBFD->F_hat_real[j][p]) - (PBFD->U_f_image[i][j][p] * PBFD->F_hat_image[j][p]);
					sum_image += (PBFD->U_f_real[i][j][p]*PBFD->F_hat_image[j][p]) + (PBFD->F_hat_real[j][p] * PBFD->U_f_image[i][j][p]);
				}
				PBFD->zfp_real[i] += sum_real;
				PBFD->zfp_image[i] += sum_image;
				//printf("zfp_real = %.10f,%d\n", PBFD->zfp_real[i], i);
				sum_real = 0.0f;
				sum_image = 0.0f;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		IFFT(PBFD->doIFFT1, PBFD->zfp_real, PBFD->zfp_image);
		sum = 0.0f;
		for (i = 0; i < L; i++) {
			for (j = 0; j < M; j++) {
				sum += PBFD->temp_mat[i][j] * PBFD->doIFFT1->real[j]; // IFFT won't have imaginary part as our input signal is real
			}
			PBFD->temp4[i] = PBFD->y_f[m*L + i] - sum;
			sum = 0.0f;
		}
		for (i = 0; i < M; i++){
			for (j = 0; j < L; j++){
				sum += PBFD->temp_mat_t[i][j] * PBFD->temp4[j];
			}
			PBFD->E_f[i] = sum; //this E_f is not same as MATLAB code..(minor changes -> FFT not applied)
			// MATLAB E_f variable is : PBFD->doFFT5->real and PBFD->doFFT5->imaginary
			sum = 0.0f;
		}
		FFT(PBFD->doFFT5, PBFD->E_f, M);
		for (i = 0; i < M; i++) {
			PBFD->zfp_real[i] = 0.0f;
			PBFD->zfp_image[i] = 0.0f;
		}
		///////////////////////////////////////////////////////////////////////////////matlab line 136
		for (p = 0; p < PBFD->sub_filters; p++){
			PBFD->delta[p] = (float)(PBFD->Mu) / (float)((1e-6) + PBFD->power[p]);
			for (i = 0; i < M; i++){
				for (j = 0; j < M; j++){
					sum_u = sum_u + (PBFD->U_f_real[j][i][p] * PBFD->doFFT5->real[j]) -
						(-PBFD->U_f_image[j][i][p] * PBFD->doFFT5->imaginary[j]); //value of E_f according to matlab code
					sum_y = sum_y + (-PBFD->U_f_image[j][i][p] * PBFD->doFFT5->real[j]) +
						(PBFD->U_f_real[j][i][p] * PBFD->doFFT5->imaginary[j]);
				}
				PBFD->temp5[i] = sum_u; sum_u = 0.0f;
				PBFD->temp6[i] = sum_y; sum_y = 0.0f;
			}
			IFFT(PBFD->doIFFT2, PBFD->temp5, PBFD->temp6);
			for (i = 0; i < M; i++) {
				for (j = 0; j < M; j++) {
					sum += PBFD->g[i][j] * PBFD->doIFFT2->real[j];
				}
				PBFD->temp7[i] = sum;
				sum = 0.0f;
			}
			FFT(PBFD->doFFT3, PBFD->temp7, M);
			for (i = 0; i < M; i++)
			{
				PBFD->F_hat_real[i][p] +=  PBFD->delta[p] * PBFD->doFFT3->real[i];
				PBFD->F_hat_image[i][p] += PBFD->delta[p] * PBFD->doFFT3->imaginary[i];
				printf("F_hat real   %f, %d\n", PBFD->F_hat_real[i][p],         i);
			}
		}
	}
}

void computePBFD(PBFD_NLMS* PBFD, float* input, int n){
	int p, i, j, m, id1,id2,id3,id4 ;
	float sum_real = 0.0f, sum_image = 0.0f, sum_u = 0.0f, sum_y = 0.0f;
	float sum = 0.0f, u = 0.0f, u_f = 0.0f, y_f = 0.0f;
	int L = PBFD->L, M = PBFD->M, P = PBFD->P, N = PBFD->N, AR_order = PBFD->AR_order;

	/////////////////////////////////////////////////////////////////////
	for (m = 0; m < PBFD->id; m++){ // Each Block has L samples and N/L number of blocks in each frame
		int d = 0;
		for (i = 0; i < L; i++){
			if (m*L+(n-1)*N+i < PBFD->gain_delay)
			{
				printf("%d", m*L + (n - 1)*N + i);
				PBFD->dec = 0;
				u = 0.0f;
			}
			else
			{
				PBFD->dec = 1;
				u = PBFD->gain * PBFD->e_buffer[d];
				printf("%lf",u);
				d++;
			}
			PBFD->u[m*L + i] = u;

			// Logic for buf_u_FB = [u buf_u_FB(1:end-1)];
			for (j = 86; j >= 0; j--){
				PBFD->buf_u_fb[j + 1] = PBFD->buf_u_fb[j];
			}
			PBFD->buf_u_fb[0] = u;
			for (j = 0; j < 88; j++){
				sum = sum + PBFD->buf_u_fb[j] * starkeymodelBTE[j];
			}
			PBFD->y[m*L + i] = sum + input[m*L + i];// feedback signal + desired input
			sum = 0.0f;
		}
		//sum_buf = 0.0f;
		// We only need total M+P*(L-1) elements of a previous frame to get vec_u(64 elements) for each sample
		// Logic to fill vec_u is : Left shift the contents of the u_buffer(previous frame's PBFD->u) and copy current frame's PBFD->u's elements at the end
		// and last M values of the resulting u_buffer go into vec_u of size M (keep p=1 to L loop in mind, i.e. subtract P from the index accordingly)

		for(i = 0 ; i < PBFD->u_bufferlen-L ; i++){ //left shit by L
			PBFD->u_buffer[i] = PBFD->u_buffer[i+L]; //u_buffer = u2
		}
		for(i = 0 ; i < L ; i++){
			PBFD->u_buffer[PBFD->u_bufferlen-L+i] = PBFD->u[m*L+i];
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(m==PBFD->id-1){ //save the full PBFD->u for the future use
			for(i=0;i<N;i++){
				PBFD->u_buffer1[i] = PBFD->u[i];
				PBFD->y_buffer[i] = PBFD->y[i];
			}
		}
		//////////////////////////////////////////////////////////////////////////
		for (p = 0; p < PBFD->sub_filters; p++){
			for (j = 0; j < M; j++){
				id2 = PBFD->u_bufferlen - p*P - M + j;
				if(id2>=0){
					PBFD->vec_u[j] = PBFD->u_buffer[id2];
				}else{
					PBFD->vec_u[j] = 0;
				}
			}
			FFT(PBFD->doFFT, PBFD->vec_u, M);
			for (i = 0; i < M; i++) {
				PBFD->zp_real[i] += (PBFD->doFFT->real[i] * PBFD->F_hat_real[i][p]) - (PBFD->doFFT->imaginary[i] * PBFD->F_hat_image[i][p]);
				PBFD->zp_image[i]+= sum_image + (PBFD->doFFT->real[i] * PBFD->F_hat_image[i][p]) + (PBFD->F_hat_real[i][p] * PBFD->doFFT->imaginary[i]);
			}
		}
		///////////////////////////////////////////////////////////////////////////
		IFFT(PBFD->doIFFT, PBFD->zp_real, PBFD->zp_image);
		sum = 0.0f;
		for (i = 0; i < L; i++){
			for (j = 0; j < M; j++){
				sum = sum + PBFD->temp_mat[i][j] * PBFD->doIFFT->real[j]; // IFFT won't have imaginary part for real input
			}
			PBFD->z[i] = sum;
			sum = 0.0f;
		}
		for (i = 0; i < M; i++){
			PBFD->zp_real[i] = 0.0f;
			PBFD->zp_image[i] = 0.0f;
		}
		//////////////////////////////////////////////////////////////////////////
		for (i = 0; i < L; i++) {
			PBFD->e[m * L + i] = PBFD->y[m * L + i] - PBFD->z[i];
		}
		if (PBFD->dec == 0) {
			for (i = 0; i < L; i++) {
				PBFD->e_buffer[m * L + (n - 1) * N + i] = PBFD->e[m * L + i];
			}
		}
		if (PBFD->dec == 1) {
			if(PBFD->dec1==0){
				PBFD->dec1 = 1;
				for (i = 0; i < PBFD->gain_delay - PBFD->index2; i++) { //left shit by L
					PBFD->e_buffer[i] = PBFD->e_buffer[i + PBFD->index2];
				}
			}else{
				for (i = 0; i < PBFD->gain_delay - L; i++) { //left shit by L
					PBFD->e_buffer[i] = PBFD->e_buffer[i + L];
				}
			}
			for (i = 0; i < L; i++) {
				PBFD->e_buffer[PBFD->gain_delay - L + i] = PBFD->e[m * L + i];
			}
		}

		for (i = 0; i < L; i++){
			for (j = AR_order - 1; j >= 0; j--){
				PBFD->buf_u[j + 1] = PBFD->buf_u[j];
				PBFD->buf_y[j + 1] = PBFD->buf_y[j];
			}
			PBFD->buf_u[0] = PBFD->u_buffer1[m*L + i];
			PBFD->buf_y[0] = PBFD->y_buffer[m*L + i];
			if(m==0){
				for(j=0;j<L;j++){
					PBFD->last_u_buffer1[j] = PBFD->u_buffer1[N-L+j];
					PBFD->last_y_buffer[j] = PBFD->y_buffer[N-L+j];
				}
			}
			if(m==PBFD->id-1){
				PBFD->buf_u[0] = PBFD->last_u_buffer1[i];
				PBFD->buf_y[0] = PBFD->last_y_buffer[i];
				/*if(i==L-1){
					for (j = 0; j < AR_order + 1; j++) {
						PBFD->A_previous[j] = PBFD->A[j];
					}
				}*/
			}
			if (m==PBFD->id-1 && i==L-1){
				for(j=0;j<AR_order+1;j++){
					PBFD->A_previous[j] = PBFD->A[j];
				}
			}
			for (j = 0; j < AR_order + 1; j++){
				sum_u +=  PBFD->A_previous[j] * PBFD->buf_u[j];
				sum_y +=  PBFD->A_previous[j] * PBFD->buf_y[j];
			}
			u_f = (float)((1.0 / PBFD->G) * sum_u);
			y_f = (float)((1.0 / PBFD->G) * sum_y);
			sum_u = 0.0f;
			sum_y = 0.0f;

			if ((m*L + i + 1) % N == 0){
				computeLevinson(PBFD->doLevinson, PBFD->e);
				for (j = 0; j < AR_order + 1; j++){
					PBFD->A[j] = PBFD->doLevinson->A[j];
				}
				PBFD->G = PBFD->doLevinson->G;
			}
			PBFD->u_f[m*L + i] = u_f;
			PBFD->y_f[m*L + i] = y_f;
		}

		for(i = 0 ; i < PBFD->u_bufferlen-L ; i++){ //left shit by L
			PBFD->u_f_buffer[i] = PBFD->u_f_buffer[i+L];
		}
		for(i = 0 ; i < L ; i++){
			PBFD->u_f_buffer[PBFD->u_bufferlen-L+i] = PBFD->u_f[m*L+i];
		}
		/////////////////////////////////////////////////////////////////////
		for (p = 0; p < PBFD->sub_filters; p++) {
			for (j = 0; j < M; j++){
				id4 = PBFD->u_bufferlen - M - p*P + j;
				if(id4 >= 0){
					PBFD->vec_uf[j] = PBFD->u_f_buffer[id4];
				}else{
					PBFD->vec_uf[j]=0;
				}
			}
			FFT(PBFD->doFFT1, PBFD->vec_uf, M);
			for (i = 0; i < M; i++) {
				for (j = 0; j < M; j++) {
					PBFD->U_f_real[i][j][p] = 0.0f;
					PBFD->U_f_image[i][j][p] = 0.0f;
				}
			}
			for (i = 0; i < M; i++) {
				PBFD->U_f_real[i][i][p] = PBFD->doFFT1->real[i];
				PBFD->U_f_image[i][i][p] = PBFD->doFFT1->imaginary[i];
			}
			float ans = norm_vector(PBFD->doFFT1->real, PBFD->doFFT1->imaginary, M);
			PBFD->power[p] = (float)(ans*ans);
			for (i = 0; i < M; i++) {
				for (j = 0; j < M; j++) {
					sum_real += (PBFD->U_f_real[i][j][p] * PBFD->F_hat_real[j][p]) - (PBFD->U_f_image[i][j][p] * PBFD->F_hat_image[j][p]);
					sum_image += (PBFD->U_f_real[i][j][p]*PBFD->F_hat_image[j][p]) + (PBFD->F_hat_real[j][p] * PBFD->U_f_image[i][j][p]);
				}
				PBFD->zfp_real[i] += sum_real;
				PBFD->zfp_image[i] += sum_image;
				sum_real = 0.0f;
				sum_image = 0.0f;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		IFFT(PBFD->doIFFT1, PBFD->zfp_real, PBFD->zfp_image);
		sum = 0.0f;
		for (i = 0; i < L; i++)
		{
			for (j = 0; j < M; j++)
			{
				sum += PBFD->temp_mat[i][j] * PBFD->doIFFT1->real[j]; // IFFT won't have imaginary part as our input signal is real
			}
			PBFD->temp4[i] = PBFD->y_f[m*L + i] - sum;
			sum = 0.0f;
		}
		for (i = 0; i < M; i++){
			for (j = 0; j < L; j++)
			{
				sum += PBFD->temp_mat_t[i][j] * PBFD->temp4[j];
			}
			PBFD->E_f[i] = sum; //this E_f is not same as MATLAB code..(minor changes -> FFT not applied)
			// MATLAB E_f variable is : PBFD->doFFT5->real and PBFD->doFFT5->imaginary
			sum = 0.0f;
		}
		FFT(PBFD->doFFT5, PBFD->E_f, M);
		for (i = 0; i < M; i++) 
		{
			PBFD->zfp_real[i] = 0.0f;
			PBFD->zfp_image[i] = 0.0f;
		}
	//	///////////////////////////////////////////////////////////////////////////////
		for (p = 0; p < PBFD->sub_filters; p++)
		{
			PBFD->delta[p] = (float)(PBFD->Mu) / (float)((1e-6) + PBFD->power[p]);
			// Transpose
			// next for loop logic is for multiplication of two complex matrices
			// plus notice that transpose of U_f_real and U_f_image are used (which is not related to the logic of the
			// complex matrix multiplication) plus Transpose of any complex matrix retruns unchanged real VALUES and conjugated imaginary VALUES
			for (i = 0; i < M; i++){
				for (j = 0; j < M; j++){
					sum_u = sum_u + (PBFD->U_f_real[j][i][p] * PBFD->doFFT5->real[j]) -
						(-PBFD->U_f_image[j][i][p] * PBFD->doFFT5->imaginary[j]); //value of E_f according to matlab code
					sum_y = sum_y + (-PBFD->U_f_image[j][i][p] * PBFD->doFFT5->real[j]) +
						(PBFD->U_f_real[j][i][p] * PBFD->doFFT5->imaginary[j]);
				}
				PBFD->temp5[i] = sum_u; sum_u = 0.0f;
				PBFD->temp6[i] = sum_y; sum_y = 0.0f;
			}
			IFFT(PBFD->doIFFT2, PBFD->temp5, PBFD->temp6);
			for (i = 0; i < M; i++) {
				for (j = 0; j < M; j++) {
					sum += PBFD->g[i][j] * PBFD->doIFFT2->real[j];
				}
				PBFD->temp7[i] = sum;
				sum = 0.0f;
			}
			FFT(PBFD->doFFT3, PBFD->temp7, M);
			for (i = 0; i < M; i++){
				PBFD->F_hat_real[i][p] +=  PBFD->delta[p] * PBFD->doFFT3->real[i];
				PBFD->F_hat_image[i][p] += PBFD->delta[p] * PBFD->doFFT3->imaginary[i];
				printf("///////F_hat real   %f, %d\n", PBFD->F_hat_real[i][p], i);//------------------------------------------------------------------------
			}
		}
	}
}

float norm_vector(float* in_real, float* in_image, int len){
		// function to find a norm of a complex vector
		// Size of in_real and in_image must be equal to m
		int i;
		float sum = 0.0f;
		for (i = 0; i<len; i++){
			sum = sum + powf(in_real[i], 2) + powf(in_image[i], 2);
		}
		return sqrtf(sum);
}

void complex_mul_matrix(float** ans_real, float** ans_image, float** a_real, float** a_image, float** b_real, float** b_image, int a_rows, int a_col, int b_col){
	// a and b are complex matrices..we are implementing A * B
	// This function works correctly but not being used because our logic in the algorithm takes more processing time so directly 'for' loops are implemented
	float sum_Real = 0.0f;
	float sum_Image = 0.0f;
	int i, j, k;
	for (i = 0; i < a_rows; i++){
		for (j = 0; j < b_col; j++){
			for (k = 0; k < a_col; k++){
				sum_Real = sum_Real + (a_real[i][k] * b_real[k][j]) - (a_image[i][k] * b_image[k][j]);
				sum_Image = sum_Image + (a_real[i][k] * b_image[k][j]) + (b_real[k][j] * a_image[i][k]);
			}
			ans_real[i][j] = sum_Real; sum_Real = 0.0f;
			ans_image[i][j] = sum_Image; sum_Image = 0.0f;
		}
	}
}

float mag2db(float x){
	return 20 * logf(x) * 0.4342944819032518; // 20*(log(x)/log(10));
}
/*/
void destroyPBFD(PBFD_NLMS** pbfd){
	if (*pbfd != NULL){
		destroyTimer(&((*pbfd)->t1));
		if ((*pbfd)->power != NULL){
			free((*pbfd)->power);
			(*pbfd)->power = NULL;
		}if ((*pbfd)->delta != NULL){
			free((*pbfd)->delta);
			(*pbfd)->delta = NULL;
		}if ((*pbfd)->vec_u != NULL){
			free((*pbfd)->vec_u);
			(*pbfd)->vec_u = NULL;
		}if ((*pbfd)->vec_uf != NULL){
			free((*pbfd)->vec_uf);
			(*pbfd)->vec_uf = NULL;
		}if ((*pbfd)->E_f != NULL){
			free((*pbfd)->E_f);
			(*pbfd)->E_f = NULL;
		}if ((*pbfd)->A != NULL){
			free((*pbfd)->A);
			(*pbfd)->A = NULL;
		}if ((*pbfd)->A_previous != NULL){
			free((*pbfd)->A_previous);
			(*pbfd)->A_previous = NULL;
		}if ((*pbfd)->buf_u != NULL){
			free((*pbfd)->buf_u);
			(*pbfd)->buf_u = NULL;
		}if ((*pbfd)->buf_u_fb != NULL){
			free((*pbfd)->buf_u_fb);
			(*pbfd)->buf_u_fb = NULL;
		}if ((*pbfd)->buf_y != NULL){
			free((*pbfd)->buf_y);
			(*pbfd)->buf_y = NULL;
		}if ((*pbfd)->u_buffer != NULL){
			free((*pbfd)->u_buffer);
			(*pbfd)->u_buffer = NULL;
		}if ((*pbfd)->u_buffer1 != NULL){
			free((*pbfd)->u_buffer1);
			(*pbfd)->u_buffer1 = NULL;
		}if ((*pbfd)->y_buffer != NULL){
			free((*pbfd)->y_buffer);
			(*pbfd)->y_buffer = NULL;
		}if ((*pbfd)->u_f_buffer != NULL){
			free((*pbfd)->u_f_buffer);
			(*pbfd)->u_f_buffer = NULL;
		}if ((*pbfd)->temp4 != NULL){
			free((*pbfd)->temp4);
			(*pbfd)->temp4 = NULL;
		}if ((*pbfd)->temp5 != NULL){
			free((*pbfd)->temp5);
			(*pbfd)->temp5 = NULL;
		}if ((*pbfd)->temp6 != NULL){
			free((*pbfd)->temp6);
			(*pbfd)->temp6 = NULL;
		}if ((*pbfd)->temp7 != NULL){
			free((*pbfd)->temp7);
			(*pbfd)->temp7 = NULL;
		}if ((*pbfd)->in != NULL){
			free((*pbfd)->in);
			(*pbfd)->in = NULL;
		}if ((*pbfd)->zp_real != NULL){
			free((*pbfd)->zp_real);
			(*pbfd)->zp_real = NULL;
		}if ((*pbfd)->zp_image != NULL){
			free((*pbfd)->zp_image);
			(*pbfd)->zp_image = NULL;
		}if ((*pbfd)->zfp_real != NULL){
			free((*pbfd)->zfp_real);
			(*pbfd)->zfp_real = NULL;
		}if ((*pbfd)->zfp_image != NULL){
			free((*pbfd)->zfp_image);
			(*pbfd)->zfp_image = NULL;
		}if ((*pbfd)->z != NULL){
			free((*pbfd)->z);
			(*pbfd)->z = NULL;
		}if ((*pbfd)->y != NULL){
			free((*pbfd)->y);
			(*pbfd)->y = NULL;
		}if ((*pbfd)->u != NULL){
			free((*pbfd)->u);
			(*pbfd)->u = NULL;
		}if ((*pbfd)->e != NULL){
			free((*pbfd)->e);
			(*pbfd)->e = NULL;
		}if ((*pbfd)->u_f != NULL){
			free((*pbfd)->u_f);
			(*pbfd)->u_f = NULL;
		}if ((*pbfd)->y_f != NULL){
			free((*pbfd)->y_f);
			(*pbfd)->y_f = NULL;
		}if ((*pbfd)->e_buffer != NULL){
			free((*pbfd)->e_buffer);
			(*pbfd)->e_buffer = NULL;
		}if ((*pbfd)->F_hat_real_temp != NULL){
			free((*pbfd)->F_hat_real_temp);
			(*pbfd)->F_hat_real_temp = NULL;
		}if ((*pbfd)->F_hat_image_temp != NULL){
			free((*pbfd)->F_hat_image_temp);
			(*pbfd)->F_hat_image_temp = NULL;
		}if ((*pbfd)->f_hat != NULL){
			free((*pbfd)->f_hat);
			(*pbfd)->f_hat = NULL;
		}if ((*pbfd)->last_u_buffer1 != NULL){
			free((*pbfd)->last_u_buffer1);
			(*pbfd)->last_u_buffer1 = NULL;
		}if ((*pbfd)->last_y_buffer != NULL){
			free((*pbfd)->last_y_buffer);
			(*pbfd)->last_y_buffer = NULL;
		}
		// 2D Memory deallocation
		int i, j;
		if ((*pbfd)->g != NULL){
			for (i = 0; i < (*pbfd)->M; i++){
				free((*pbfd)->g[i]);
			}
			free((*pbfd)->g);
		}if ((*pbfd)->temp_mat != NULL){
			for (i = 0; i < (*pbfd)->L; i++){
				free((*pbfd)->temp_mat[i]);
			}
			free((*pbfd)->temp_mat);
		}if ((*pbfd)->temp_mat_t != NULL){
			for (i = 0; i < (*pbfd)->M; i++){
				free((*pbfd)->temp_mat_t[i]);
			}
			free((*pbfd)->temp_mat_t);
		} if ((*pbfd)->F_hat_real != NULL){
			for (i = 0; i < (*pbfd)->M; i++){
				free((*pbfd)->F_hat_real[i]);
			}
			free((*pbfd)->F_hat_real);
		}if ((*pbfd)->F_hat_image != NULL){
			for (i = 0; i < (*pbfd)->M; i++){
				free((*pbfd)->F_hat_image[i]);
			}
			free((*pbfd)->F_hat_image);
		}
		// 3D memory deallocation
		if ((*pbfd)->U_f_real != NULL){
			for (i = 0; i < (*pbfd)->M; i++){
				for (j = 0; j < (*pbfd)->M; j++){
					free((*pbfd)->U_f_real[i][j]);
				}
				free((*pbfd)->U_f_real[i]);
			}
			free((*pbfd)->U_f_real);
		}if ((*pbfd)->U_f_image != NULL){
			for (i = 0; i < (*pbfd)->M; i++){
				for (j = 0; j < (*pbfd)->M; j++){
					free((*pbfd)->U_f_image[i][j]);
				}
				free((*pbfd)->U_f_image[i]);
			}
			free((*pbfd)->U_f_image);
		}
		destroyTransform(&(*pbfd)->doFFT);
		destroyTransform(&(*pbfd)->doFFT1);
		destroyTransform(&(*pbfd)->doFFT3);
		destroyTransform(&(*pbfd)->doFFT5);
		destroyTransform(&(*pbfd)->doIFFT);
		destroyTransform(&(*pbfd)->doIFFT1);
		destroyTransform(&(*pbfd)->doIFFT2);
		destroyLevinson(&(*pbfd)->doLevinson);

	free(*pbfd);
	*pbfd = NULL;
	}
}
/*/

int main()
{
	int L = 16;// L : Block Size
	int M = 32;//M point FFT
	int N = 80;//
	float Mu = 0.1;
	float G = 1;
	int filter_length = 64; //LF_hat
	int AR_order = 20;//AR_Order
	int gain = 15;//forward path gain
	int gain_delay = 80;//forward delay

	// L : Block Size, M : Point of FFT, N : Frame Length
	// P : Length of each Sub-band Filter

	
	PBFD_NLMS *Abdullah;
	Abdullah = newPBFD( L,  M,  N,  Mu,  G,  filter_length,  AR_order,  gain,  gain_delay);


	processfirstframe(Abdullah,input);
	//for (int i = 1; i < 2000;i++)
	computePBFD(Abdullah, input, 5);
	// printf("%f", PBFD->F_hat_real[i][p]);

	_getch();

}

/*
int length_c = 160000;
int fs = 16000;
int n = 20 * 1e-3*fs;/*% 20ms frames//frame length
					 int n_F = floor(length_c / n); // no. of frames


					 int *F_start = (int*)malloc(n_F * sizeof(int));
					 int *F_end = (int*)malloc(n_F * sizeof(int));

					 float *frame = (float*)malloc(n_F * sizeof(float));


					 for (int i = 0; i < n_F; i++)
					 {
					 F_start[i] = (i)*n;

					 F_end[i] = (i + 1)*n-1;

					 int y = 0;
					 for (int ii = F_start[i]; ii <= F_end[i]; ii++)
					 {
					 frame[y] = input[ii];
					 y++;

					 }
					 */