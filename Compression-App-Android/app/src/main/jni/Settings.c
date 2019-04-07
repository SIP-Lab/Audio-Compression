//
//  Settings.c
//
// Created by Nasim Alamdari on 3/23/19
// Copyright © 2019 SIPLab. All rights reserved.
//

#include "Settings.h"

Settings* newSettings() {
    
    Settings* newSettings = (Settings*)malloc(sizeof(Settings));
    
    // Set defaults
    newSettings->fs =  48000;
    newSettings->frameSize = 1200; //64;
    newSettings->stepSize = 600; //600;
    newSettings->doSaveFile = 0;
    newSettings->micStatus = 1;
    newSettings->playAudio = 0;
    newSettings->classLabel = -1;
    
    newSettings->noiseReductionOutputType = 0;
    newSettings->compressionOutputType = 0;
    newSettings->amplification = 1.0f;
    newSettings->decisionBufferLength = 16;
    newSettings->decisionRate = 1.0f;
    
    newSettings->dbpower = 0;
    //newSettings->quiet = 52.0f;//For iOS
    newSettings->quiet = 60.0f; // For Android
    newSettings->calibration = -54.0f;//-93.9794f;
    newSettings->autoGains = 1;
    newSettings->guiUpdateInterval = 0.15f;
    newSettings->noiseEstimateTime = 0.4f;
    newSettings->processTime = 0.0f;
    newSettings->noiseEstimationFrame = newSettings->noiseEstimateTime*(float)(newSettings->fs)/(float)(newSettings->stepSize);

    int i;
    for(i =0; i < 5; i++){
        newSettings->userBandGains[i] = 1.0f;
    }

    //For noise classifier
    newSettings->saveData = 0;
    newSettings->hybridMode = 0;
    newSettings->vigilance1 = 0.02;
    newSettings->vigilance2 = 0.9;
    newSettings->FeatAvgBufferLength = 80;
    newSettings->NewClusterCreationBufferTime = 3.0f;
    newSettings->DecisionSmoothingBufferTime = 3.0f;

    // for VAD output
    newSettings->melImgFormationSteps = 13;
    newSettings->VADBufferLength = floor((2*1000)/12.5); // if overlapp window size is 12.5 ms with FrameStepForMel= 13 waiting for 2 ms , then VADbufferSize is 160
    newSettings->VADSmoothBuffer = 0;
    newSettings->finalVADLabel = 0;

    // For Adaptive feedback cancelation
    newSettings->afcSwitch = 0;

    // Compression settings from .json file
    newSettings->compressionBand1 = (float*)calloc(4, sizeof(float));
    newSettings->compressionBand2 = (float*)calloc(4, sizeof(float));
    newSettings->compressionBand3 = (float*)calloc(4, sizeof(float));
    newSettings->compressionBand4 = (float*)calloc(4, sizeof(float));
    newSettings->compressionBand5 = (float*)calloc(4, sizeof(float));
    newSettings->jsonFileSelected = 0;

    return newSettings;
}

void destroySettings(Settings* settings) {
    if(settings != NULL){
        free(settings);
        settings = NULL;
    }
}

