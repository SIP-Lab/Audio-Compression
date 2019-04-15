//
//  compute_Comp.h
//  Integrated_App
//
//  Created by Taghizadeh Alamdari, Nasim on 3/23/19.
//  Copyright © 2019 UT Dallas. All rights reserved.
//

#ifndef compute_Comp_h
#define compute_Comp_h


#include <stdio.h>
#include <math.h>
#include "Other/FIRFilter.h"
#include "Other/Transforms.h"
#include "Other/SPLBuffer.h"
#include "Settings.h"
#include "rt_nonfinite.h"
#include "DynamicRangeFiveBandCompression.h"
#include "DynamicRangeFiveBandCompression_initialize.h"
#include "DynamicRangeFiveBandCompression_terminate.h"



extern float dataIn[20];

#define DECIMATION_FACTOR 3
#define EPS 1.0e-7
#define S2F 3.051757812500000e-05f
#define F2S 32768

#define NFILT               40
#define FREQLOW             300
#define FREQHIGH            8000

typedef struct CompressionVars {
    
    int stepSize;
    int decimatedStepSize;
    int samplingFrequency;
    
    float* input;
    float* downsampled;
    float* decimated;
    float* processed;
    float* frame;
    
    float* compressed;
    
    float* interpolated;
    float* output;
    
    long* memoryPointer;
    
    Transform* fft;
    
    
    FIR* downsampleFilter;
    FIR* interpolationFilter;
    // For audio level
    SPLBuffer* spl;
    
    //----------------------------------------
    // Compression
    float* compressionParams;
    
    
    
} CompressionVars;


CompressionVars* init_Compression(Settings* settings);
void perform_Compression(CompressionVars *_ptr, short *_in, short* _out, Settings* settings);
void destroy_Compression(CompressionVars** _ptr);



#endif /* compute_Comp_h */
