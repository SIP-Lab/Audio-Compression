//
//  SpeechProcessing.h
//
//
//  Created by Nasim Alamdari on 3/23/19.
//  Copyright © 2019 SIPLab. All rights reserved.
//

#ifndef __Speech_Processing_Pipeline__SpeechProcessing__
#define __Speech_Processing_Pipeline__SpeechProcessing__

#include <stdio.h>
#include <math.h>
#include "Other/FIRFilter.h"
#include "Other/Transforms.h"
#include "Other/SPLBuffer.h"
#include "Settings.h"
#include "CompressionSettingController.h"
#include "rt_nonfinite.h"
#include "DynamicRangeFiveBandCompression.h"
#include "DynamicRangeFiveBandCompression_initialize.h"
#include "DynamicRangeFiveBandCompression_terminate.h"


#define NFILT               40
#define FREQLOW             300
#define FREQHIGH            8000
#define DECIMATION_FACTOR 3
#define EPS 1.0e-7
#define S2F 3.051757812500000e-05f
#define F2S 32768

typedef struct CompressionVars {
    
    int stepSize;
    int decimatedStepSize;
    int decimatedWindowSize;
    int overlap;
    float decimatedFs;
    float* input;
    float* downsampled;
    float* decimated;
    float* noise_reduced;
    float* inputBuffer;
    float* compressed;
    
    float* interpolated;
    float* output;

    FIR* downsampleFilter;
    FIR* interpolationFilter;
    
    // For audio level
    Transform* fft;
    SPLBuffer* spl;

    
} CompressionVars;


CompressionVars* init_Compression(Settings* settings);
void perform_Compression(CompressionVars *_ptr, float *_in, float* _out, Settings* settings);
void destroy_Compression(CompressionVars** _ptr);

//****iOS takes short as Input buffer***//
//***SuperpoweredSDK(used in Android) handles short to float conversion input buffer***//
#endif /* defined(__Speech_Processing_Pipeline__SpeechProcessing__) */

