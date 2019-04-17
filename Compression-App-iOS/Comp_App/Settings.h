//
//  Settings.h
//  SPP_Integrated_App
//
//  Created by Akshay Chitale on 6/25/17.
//  Copyright © 2017 UT Dallas. All rights reserved.
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
    int compressionOutputType;
    int doSaveFile;
    int classLabel;
    float amplification;
    
    // For audio level based gains switching
    float dbpower;
    float quiet;
    float calibration;
    //int autoGains;
    float dbUpdateInterval; // How long to wait (in seconds) before updating dbpower
    float averagedGainsTime;

    float alpha;            // Between 0 and 1, only used if SPLBuffer mode is SPLBUFFER_MODE_RANGE
    float beta;             // Between 0 and 1, only used if SPLBuffer mode is SPLBUFFER_MODE_RANGE
    float processTime;
    
    // Compression settings
    float* compressionBand1;
    float* compressionBand2;
    float* compressionBand3;
    float* compressionBand4;
    float* compressionBand5;
    int jsonFileSelected;
    

    // ****** Test only for JSON *****
    // For keeping track of gains and multiplier
    // Definitions of low, medium, and high gains
    float lowDef;
    float mediumDef;
    float highDef;
    // Pointers to arrays of gains
    float* lowGains;
    float* mediumGains;
    float* highGains;
    float* gains;
    // Other
    int numGains;
    int autoGains;
    int currentGains;       // 0 is low, 1 is medium, 2 is high
    
} Settings;

Settings* newSettings(void);
void destroySettings(Settings* settings);

#endif /* Settings_h */
