//
//  Settings.c
//  SPP_Integrated_App
//
//  Created by Akshay Chitale on 6/25/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
//

#import "Settings.h"

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
    
    newSettings->compressionOutputType = 0;
    newSettings->amplification = 1.0;
    
    newSettings->dbpower = 0;
    newSettings->quiet = 52.0f;//For iOS
    //newSettings->quiet = 60.0f; // For Android
    newSettings->calibration = -54;
    newSettings->dbUpdateInterval = 0.5;
    newSettings->averagedGainsTime = 2.0f;

//    newSettings->currentGains = 1;
    newSettings->alpha = 0.5;
    newSettings->beta = 0.5;
    newSettings->processTime = 0.0f;
    

    
    // Compression settings from .json file
    newSettings->compressionBand1 = (float*)calloc(4, sizeof(float));
    newSettings->compressionBand2 = (float*)calloc(4, sizeof(float));
    newSettings->compressionBand3 = (float*)calloc(4, sizeof(float));
    newSettings->compressionBand4 = (float*)calloc(4, sizeof(float));
    newSettings->compressionBand5 = (float*)calloc(4, sizeof(float));
    newSettings->jsonFileSelected = 0;

 
    // ****** Test only for JSON *****
    // Points to arrays from AudioSettingsController.swift, so no calloc/free needed here
    newSettings->lowDef = 55.0; // default of low
    newSettings->mediumDef = 65.0; // default of medium
    newSettings->highDef = 75.0; // default of high
    newSettings->lowGains = NULL;
    newSettings->mediumGains = NULL;
    newSettings->highGains = NULL;
    newSettings->gains = NULL;
    newSettings->numGains = 5;
    newSettings->autoGains = 1;
    newSettings->currentGains = 1;
    
    return newSettings;
}

void destroySettings(Settings* settings) {
    if(settings != NULL){
        free(settings);
        settings = NULL;
    }
}
