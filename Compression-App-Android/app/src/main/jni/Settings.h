//
// Settings.h
// Created by Nasim Alamdari on 3/23/19
// Copyright © 2019 SIPLab. All rights reserved.
//

#import <stdlib.h>

#ifndef Settings_h
#define Settings_h

typedef struct Settings {
    // Core values
    int fs;
    int frameSize;
    int stepSize;
    
    // Audio status flags
    int micStatus;
    int playAudio;
    
    // To modify output audio type
    int noiseReductionOutputType;
    int compressionOutputType;
    int doSaveFile;
    int classLabel;
    int decisionBufferLength;
    float decisionRate;
    float amplification;
    
    // For audio level based gains switching
    float dbpower;
    float quiet;
    float calibration;
    int autoGains;
    float guiUpdateInterval; // How long to wait (in seconds) before updating dbpower
    float noiseEstimateTime; //Noise estimation time (in seconds)
    
    float processTime;
    float noiseEstimationFrame;


    float userBandGains[5];

    //For noise classifier
    int saveData;
    int hybridMode;
    float vigilance1;
    float vigilance2;
    int FeatAvgBufferLength;
    float NewClusterCreationBufferTime;
    float DecisionSmoothingBufferTime;


    // for VAD output
    int melImgFormationSteps;
    int VADBufferLength;
    int VADSmoothBuffer;
    int finalVADLabel;

    // For Adaptive feedback cancelation
    int afcSwitch;

    // Compression settings
    float* compressionBand1;
    float* compressionBand2;
    float* compressionBand3;
    float* compressionBand4;
    float* compressionBand5;
    int jsonFileSelected;
} Settings;

Settings* newSettings();
void destroySettings(Settings* settings);

#endif /* Settings_h */

