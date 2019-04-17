//
// SpeechProcessing.c
// Signal Processing Pipeline:
// Compression
//
// Created by Nasim Alamdari on 3/23/19
// Copyright © 2019 SIPLab. All rights reserved.
//

#include <sys/time.h>
#include "SpeechProcessing.h"
#include "DynamicRangeMultibandCompression/DynamicRangeFiveBandCompression.h"
#include "DynamicRangeMultibandCompression/DynamicRangeFiveBandCompression_initialize.h"
#include "DynamicRangeMultibandCompression/DynamicRangeFiveBandCompression_terminate.h"





//-----------Initiliazing parameteres for the overall pipeline //


CompressionVars* init_Compression(Settings* settings) {

    CompressionVars* inParam = (CompressionVars*)malloc(sizeof(CompressionVars));
    
    inParam->stepSize            = settings->stepSize;
    inParam->decimatedStepSize   = settings->stepSize/DECIMATION_FACTOR;
    inParam->decimatedWindowSize = settings->frameSize/DECIMATION_FACTOR;
    inParam->overlap             = inParam->decimatedWindowSize - inParam->decimatedStepSize;
    inParam->decimatedFs         = settings->fs/DECIMATION_FACTOR;
    inParam->input               = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->downsampled         = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->inputBuffer         = (float*)calloc(inParam->decimatedWindowSize,sizeof(float));
    inParam->decimated           = (float*)calloc(inParam->decimatedStepSize, sizeof(float));
    inParam->noise_reduced       = (float*)calloc(inParam->decimatedStepSize, sizeof(float));

    inParam->interpolated        = (float*)calloc(settings->stepSize, sizeof(float));
    inParam->output              = (float*)calloc(settings->stepSize, sizeof(float));
    
    inParam->downsampleFilter    = initFIR(settings->stepSize);
    inParam->interpolationFilter = initFIR(settings->stepSize);
    
    // For audio level
    inParam->fft                 = newTransform(inParam->decimatedWindowSize,
                                                (int)(inParam->decimatedFs/inParam->decimatedStepSize));
    
    inParam->spl                 = newSPLBuffer(settings->guiUpdateInterval * settings->fs / settings->stepSize,
                                                SPLBUFFER_MODE_FULL);

    //For Compression Initialization
    DynamicRangeFiveBandCompression_initialize();
    inParam->compressed          = (float*)calloc(inParam->decimatedStepSize, sizeof(float));

    return inParam;
}

//Notes:
//1. Here, 50% overlapping is performed after downsampling the audio signal and stored in 'inParam->inputBuffer'.
//   This is used for FFT calculation (Transform section) to calculate power.
//
//2. Compression module is placed after noise reduction module so that it can use the processed output from noise
//   reduction for compression as its input. This can also be used with Noise Reduction is turned off.
//

void perform_Compression(CompressionVars *_ptr, float *_in, float* _out, Settings* settings){

    CompressionVars *inParam = _ptr;
    
    int i,j;
    // Convert from Short to Float
    for (i = 0; i < inParam->stepSize; i++) {
        //inParam->input[i] = _in[i] * S2F;
        inParam->input[i] = _in[i];
    }

    // Downsample the audio and lowpass filter
    processFIRFilter(inParam->downsampleFilter, inParam->input, inParam->downsampled);
    
    // Decimate the audio
    for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j+= DECIMATION_FACTOR) {
        inParam->decimated[i] = inParam->downsampled[j];
    }

    //Performing 50% overlapping of input data for power calculation (used for Audio Level and VAD)
    for (i = 0; i < inParam->overlap; i++) {
        inParam->inputBuffer[i] = inParam->inputBuffer[inParam->decimatedStepSize + i];
    }
    for (i=0; i<inParam->decimatedStepSize; i++)
    {
        inParam->inputBuffer[inParam->overlap + i] = inParam->decimated[i];
    }

    //Calculate and Save Audio Level to Settings
    ForwardFFT(inParam->fft, inParam->inputBuffer, settings->calibration);
    inParam->spl->buffer(inParam->spl, inParam->fft->dbpower, settings);

    
    //when compression is on
    if(settings->compressionOutputType){

        /***--------------Compression---------------------------------**/

        DynamicRangeFiveBandCompression(inParam->decimated, inParam->decimatedFs, dataIn, inParam->compressed);

        /***-------------Compression ENDS: Remove This section if want to replace with another Compression---------------------------**/

        //Interpolate the Audio
        for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j+=3) {
            inParam->interpolated[j] = inParam->compressed[i];
        }
        // Low-Pass filter the Interpolated Audio
        processFIRFilter(inParam->interpolationFilter, inParam->interpolated, inParam->output);
        // Convert to Short from Float
        for (i = 0; i < inParam->stepSize; i++) {
            //_out[i] = (short)(settings->amplification*2*F2S * inParam->output[i]);
            _out[i] = (settings->amplification * 2 * inParam->output[i]);
        }
    }
    else {
        //memcpy(_out, _in, inParam->stepSize * sizeof(short));
        memcpy(_out, _in, inParam->stepSize * sizeof(float));
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
        if ((*_ptr)->noise_reduced != NULL){
            free((*_ptr)->noise_reduced);
            (*_ptr)->noise_reduced = NULL;
        }
        
        if ((*_ptr)->inputBuffer != NULL){
            free((*_ptr)->inputBuffer);
            (*_ptr)->inputBuffer = NULL;
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
        
        destroyFIR(&(*_ptr)->downsampleFilter);
        destroyFIR(&(*_ptr)->interpolationFilter);
        destroyTransform(&(*_ptr)->fft);
        destroySPLBuffer(&(*_ptr)->spl);
        
        free(*_ptr);
        *_ptr = NULL;
    }
}






