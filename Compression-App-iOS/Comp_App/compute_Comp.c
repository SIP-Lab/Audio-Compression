//
//  VAD_NR_Com.c
//  Voice Activity Detection with NoiseReduction and compression
//
//  Created by Abhishek Sehgal on 8/14/17.
//  Modified by Nasim Alamdari on 07/30/2018
//  Copyright © 2017 SIPLab. All rights reserved.
//

#include "compute_Comp.h"
#include <sys/time.h>


struct timeval t1, t2;
double elapsedTime;
float dataIn[20] = {5, -5, 5, 100,
    5, -10, 5, 100,
    5, -20, 2, 50,
    4, -25, 2, 50,
    4, -35, 2, 100};

CompressionVars* init_Compression(Settings* settings) {
    
    CompressionVars* inParam = (CompressionVars*)malloc(sizeof(CompressionVars));
    
    inParam->stepSize           = settings->stepSize;
    inParam->decimatedStepSize  = settings->stepSize/DECIMATION_FACTOR;
    inParam->samplingFrequency  = settings->fs/DECIMATION_FACTOR;
    
    inParam->input          = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->downsampled    = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->decimated      = (float*)calloc(inParam->decimatedStepSize, sizeof(float));
    inParam->processed      = (float*)calloc(inParam->decimatedStepSize, sizeof(float));
    inParam->frame          = (float*)calloc(inParam->decimatedStepSize*2, sizeof(float));
    
    inParam->interpolated   = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->output         = (float*)calloc(settings->stepSize, sizeof(float));
    
    inParam->fft            = newTransform(2*inParam->decimatedStepSize,
                                (int)(inParam->samplingFrequency/inParam->decimatedStepSize));

    inParam->downsampleFilter       = initFIR(settings->stepSize);
    inParam->interpolationFilter    = initFIR(settings->stepSize);
    
    // For audio level
    inParam->spl = newSPLBuffer(settings->dbUpdateInterval * settings->fs / settings->stepSize, SPLBUFFER_MODE_FULL);
    
    //===== For compression ====================================
    DynamicRangeFiveBandCompression_initialize();
    inParam->compressed      = (float*)calloc(inParam->decimatedStepSize, sizeof(float));
    
    return inParam;
}


void perform_Compression(CompressionVars *_ptr,
                                          short *_in,
                                          short* _out,
                                          Settings* settings){
    
    
    CompressionVars *inParam = _ptr;
    int i,j;
    
    
    
    for (i = 0; i < inParam->stepSize; i++) {
        inParam->input[i] = _in[i] * S2F;
    }
    
    //Downsample the Audio
    processFIRFilter(inParam->downsampleFilter, inParam->input, inParam->downsampled);
    
    //Decimate the Audio
    for(i = 0, j = 0; i < inParam->decimatedStepSize; i++, j+= 3){
        inParam->decimated[i] = inParam->downsampled[j];
        
        inParam->frame[i] = inParam->frame[i+inParam->decimatedStepSize];
        inParam->frame[i+inParam->decimatedStepSize] = inParam->decimated[i];
    }
    
    
    ForwardFFT(inParam->fft, inParam->frame, settings->calibration);
    settings->dbpower = inParam->fft->dbpower;
    
    
    // set compression settings from  .json (if any)
    if (settings->jsonFileSelected == 1 & settings->compressionBand1 != NULL & settings->compressionBand2 != NULL & settings->compressionBand3 != NULL & settings->compressionBand4 != NULL & settings->compressionBand5 != NULL){
        dataIn[0]  = settings->compressionBand1[0];
        dataIn[1]  = settings->compressionBand1[1];
        dataIn[2]  = settings->compressionBand1[2];
        dataIn[3]  = settings->compressionBand1[3];
        
        dataIn[4]  = settings->compressionBand2[0];
        dataIn[5]  = settings->compressionBand2[1];
        dataIn[6]  = settings->compressionBand2[2];
        dataIn[7]  = settings->compressionBand2[3];
        
        dataIn[8]  = settings->compressionBand3[0];
        dataIn[9]  = settings->compressionBand3[1];
        dataIn[10] = settings->compressionBand3[2];
        dataIn[11] = settings->compressionBand3[3];
        
        dataIn[12] = settings->compressionBand4[0];
        dataIn[13] = settings->compressionBand4[1];
        dataIn[14] = settings->compressionBand4[2];
        dataIn[15] = settings->compressionBand4[3];
        
        dataIn[16] = settings->compressionBand5[0];
        dataIn[17] = settings->compressionBand5[1];
        dataIn[18] = settings->compressionBand5[2];
        dataIn[19] = settings->compressionBand5[3];
        
        settings->jsonFileSelected = 0;
    }
    
    
    //when compression is on
    if(settings->compressionOutputType){
        // Compression
        DynamicRangeFiveBandCompression(inParam->decimated, 16000, dataIn, 48, 0, inParam->compressed);
        //Interpolate the Audio
        for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j+=3) {
            inParam->interpolated[j] = inParam->compressed[i];
        }
        // Low-Pass filter the Interpolated Audio
        processFIRFilter(inParam->interpolationFilter, inParam->interpolated, inParam->output);
        // Convert to Short from Float
        for (i = 0; i < inParam->stepSize; i++) {
            _out[i] = (short)(settings->amplification*2*F2S * inParam->output[i]);
        }
    }
    else {
        memcpy(_out, _in, inParam->stepSize * sizeof(short));
    }
}

void destroy_Compression(CompressionVars** _ptr) {
    if (*_ptr != NULL) {
        
        
        DynamicRangeFiveBandCompression_terminate();
        
        if ((*_ptr)->input != NULL){
            free((*_ptr)->input);
            (*_ptr)->input = NULL;
        }
        if ((*_ptr)->downsampled != NULL){
            free((*_ptr)->downsampled);
            (*_ptr)->downsampled = NULL;
        }
        if ((*_ptr)->decimated != NULL){
            free((*_ptr)->decimated);
            (*_ptr)->decimated = NULL;
        }
        if ((*_ptr)->processed != NULL){
            free((*_ptr)->processed);
            (*_ptr)->processed = NULL;
        }
        
        /*for compression*/
        if ((*_ptr)->compressed != NULL){
            free((*_ptr)->compressed);
            (*_ptr)->compressed = NULL;
        }
        /**/
        
        if ((*_ptr)->interpolated != NULL){
            free((*_ptr)->interpolated);
            (*_ptr)->interpolated = NULL;
        }
        if ((*_ptr)->output != NULL){
            free((*_ptr)->output);
            (*_ptr)->output = NULL;
        }
        
        /*for VAD Parameters
        if ((*_ptr)->memoryPointer != NULL){
            free((*_ptr)->memoryPointer);
            (*_ptr)->memoryPointer = NULL;
        }
        */
        
        
        if ((*_ptr)->spl != NULL){
            free((*_ptr)->spl);
            (*_ptr)->spl = NULL;
        }
        
        destroyFIR(&(*_ptr)->downsampleFilter);
        destroyFIR(&(*_ptr)->interpolationFilter);
        destroyTransform(&(*_ptr)->fft);

        free(*_ptr);
        *_ptr = NULL;
    }
}



