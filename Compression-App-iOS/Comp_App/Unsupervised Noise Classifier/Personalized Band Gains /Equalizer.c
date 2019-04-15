//
// Created by shashank on 6/7/2018.
//
#include "Equalizer.h"
#include "stdio.h"

Equalizer* initEqualizer(int SaveDataSwitch, int HybridModeSwitch, const char* pathHybridDir, int MaxClusterNum, int TotalDetectedClass){
    Equalizer* Equalization = (Equalizer*)malloc(sizeof(Equalizer));
    int i,j;
    Equalization->userBandGains = (float**)calloc(sizeof(float*), MaxClusterNum+1);
    for (i = 0; i < MaxClusterNum+1; i++) {
        Equalization->userBandGains[i] = (float*)calloc(sizeof(float), 7);
        for(j = 0; j < 7; j++){
            Equalization->userBandGains[i][j] = 1.0;
        }
    }
    Equalization->smoothBandGains = (float*)calloc(513, sizeof(float));
    for(i = 0; i < 513; i++){
        Equalization->smoothBandGains[i] = 1.0;
    }
    Equalization->saveUserBandGains = SaveDataSwitch;
    Equalization->pathHybridDir = pathHybridDir;
    Equalization->TotalDetectedNoiseClass = TotalDetectedClass;
    Equalization->prevDetectedNoiseClass = 11;
    Equalization->count = 0;

    //Hybrid mode
    if(HybridModeSwitch == 1){
        FILE *file = fopen(pathHybridDir, "rb");
        if (file != NULL){
            for( i = 0; i < Equalization->TotalDetectedNoiseClass; i++){
                for( j = 0; j < 7; j++){
                    fread(&Equalization->userBandGains[i][j],sizeof(Equalization->userBandGains[0][0]),1,file);
                }
            }
            fclose(file);
        }
        else{
            printf("\n Error! Can not open the file for writing \n");
        }
    }
    return Equalization;
}


void getSmoothBandGains(Equalizer* Equalization, float* userBandGains, int ClusterLabel, int prevClusterLabel, int totalClusters){

    if(ClusterLabel != 0) {
        if(prevClusterLabel != ClusterLabel){
            userBandGains[0] = Equalization->userBandGains[ClusterLabel - 1][1];
            userBandGains[1] = Equalization->userBandGains[ClusterLabel - 1][2];
            userBandGains[2] = Equalization->userBandGains[ClusterLabel - 1][3];
            userBandGains[3] = Equalization->userBandGains[ClusterLabel - 1][4];
            userBandGains[4] = Equalization->userBandGains[ClusterLabel - 1][5];
        }
        Equalization->userBandGains[ClusterLabel - 1][0] = Equalization->userBandGains[
                ClusterLabel - 1][1] = userBandGains[0];
        Equalization->userBandGains[ClusterLabel - 1][2] = userBandGains[1];
        Equalization->userBandGains[ClusterLabel - 1][3] = userBandGains[2];
        Equalization->userBandGains[ClusterLabel - 1][4] = userBandGains[3];
        Equalization->userBandGains[ClusterLabel - 1][5] = Equalization->userBandGains[
                ClusterLabel - 1][6] = userBandGains[4];
        Equalization->prevDetectedNoiseClass = ClusterLabel;
    }

    int i,j,b;
    float x, y;
    for(i = 0; i < 6; i++){
        b = a[i+1] - a[i];
        for(j = 0; j < b; j++){
            x = (Equalization->userBandGains[Equalization->prevDetectedNoiseClass-1][i] + Equalization->userBandGains[Equalization->prevDetectedNoiseClass-1][i+1])/2;
            y = (Equalization->userBandGains[Equalization->prevDetectedNoiseClass-1][i] - Equalization->userBandGains[Equalization->prevDetectedNoiseClass-1][i+1])/2;
            Equalization->smoothBandGains[a[i] + j] = (x) + (y * (float)cosTable[a[i] + j]);
            Equalization->smoothBandGains[512 - a[i] - j] = Equalization->smoothBandGains[a[i] + j];
        }
    }
    Equalization->smoothBandGains[256] = Equalization->smoothBandGains[255];

    Equalization->TotalDetectedNoiseClass = totalClusters;

    //Save the bandgains to the file
    if(Equalization->saveUserBandGains == 1){
        FILE *file = fopen(Equalization->pathHybridDir, "wb");
        if (file != NULL){
            for( i = 0; i < Equalization->TotalDetectedNoiseClass; i++){
                for( j = 0; j < 7; j++){
                    fwrite(&Equalization->userBandGains[i][j],sizeof(Equalization->userBandGains[0][0]),1,file);
                }
            }
            fclose(file);
        }
        else{
            printf("\n Error! Can not open the file for reading \n");
        }
    }

}


void destroyEqualizer(Equalizer** Equalization){

    int i;
    if(*Equalization != NULL){
        for (i=0;i< 10;i++)//Maximum nimber of class creation hard coded as 10: MaxClusterNum
        {
            if((*Equalization)->userBandGains[i] != NULL){
                free((*Equalization)->userBandGains[i]);
                (*Equalization)->userBandGains[i]=NULL;
            }
        }
        if((*Equalization)->smoothBandGains != NULL){
            free((*Equalization)->smoothBandGains);
            (*Equalization)->smoothBandGains = NULL;
        }
        free(*Equalization);
        *Equalization=NULL;

    }

}



