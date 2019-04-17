//
// Created by Taghizadeh Alamdari, Nasim on 5/25/18.
//

// ART-2 network is an unsupervised neural network, based on the adaptive resonance theory (ART).
// The art-2 is an older model to study how the brain implements information.
// It has since been combined with art 1 and named ARTMAP.
//

#include "Art2FusionClassifier.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


/*Function to left rotate arr[] of size n by d*/
void leftRotatebyD(int* arr, const int d, const int n )
{

    int *I = (int *)malloc(n * sizeof(int));
    int i;
    for (i = 0; i < n; i++){
        I[i] = arr[i];}

    for (i = 0; i < d; i++) {
        int temp = I[0];
        int i;
        for (i = 0; i < n - 1; i++)
            I[i] = I[i + 1];

        I[i] = temp;
    }

    for (i = 0; i < n; i++)
        arr[i] = I[i];
}


int medianFilter1D(int* x, const int n)
{
    //****** Step1: Find median value **************************
    int *temp = (int *)malloc(n * sizeof(int));
    int i, j;
    for (i = 0; i < n; i++)
        temp[i] = x[i];

    int median;
    float tmp;
    // the following two loops sort the array x in ascending order
    for(i=0; i< n-1; i++) {
        for(j= i+1; j<n; j++) {
            if(temp[j] < temp[i]) {
                // swap elements
                tmp     = temp[i];
                temp[i] = temp[j];
                temp[j] = tmp;
            }
        }
    }

    if(n % 2==0) {
        // if there is an even number of elements, return mean of the two elements in the middle
        median = (int)((temp[n/2] + temp[n/2 - 1]) / 2.0);
    } else {
        // else return the element in the middle
        median = temp[ (n+1/ 2)-1];// -1 as array indexing in C starts from 0
    }

    //return median;

    int medVal = median;
    //****************************************************
    // Step2: find maximum value and replace it with median value
    int maxFilt = 0;
    int maxFilt_idx = 0;

    for (i = 0; i < 3; i++){
        if (x[i] > maxFilt){
            maxFilt     = x[i];
            maxFilt_idx = i;
        }
    }
    x [maxFilt_idx] = medVal;
    //***************************************************
    //Step3: Find mode value of the vector
    int modeValue = 0, maxCount = 0;
    for (i = 0; i < n; ++i) {
        int count = 0;
        for (j = 0; j < n; ++j) {
            if (x[j] == x[i])
                count++;
        }

        if (count > maxCount) {
            maxCount = count;
            modeValue = x[i];
        }
    }

    return modeValue;
}

Art2FusionParallel* initArt2ParallelFusion(int SavingClusteringDataSwitch,
                                          int HybridClassificationSwitch,
                                          float vigilanceVal_1,
                                          float vigilanceVal_2,
                                          const char* pathHybridDir,
                                          int FeatureNum_1,
                                          int FeatureNum_2,
                                          int NewClusterCreationBufferSize,
                                          int DetectionDecisionBufferSize,
                                          int MaxClusterNum)
{

    int i = 0;

    Art2FusionParallel* newArt2 = (Art2FusionParallel*)malloc(sizeof(Art2FusionParallel));

    newArt2->MaxClusterNum = MaxClusterNum;
    newArt2->firstFrame    = 1;
    newArt2->Features      = (float*)calloc(FeatureNum_1+FeatureNum_2, sizeof(float));

    newArt2->FeatureNum_1  = FeatureNum_1;
    newArt2->FeatureNum_2  = FeatureNum_2;

    newArt2->MinDistVal_1  = 10000;
    newArt2->MinDistVal_2  = 10000;

    newArt2->FindMinDist1  = (float*)calloc(newArt2->MaxClusterNum,sizeof(float));
    newArt2->FindMinDist2  = (float*)calloc(newArt2->MaxClusterNum,sizeof(float));
    for ( i = 0; i < newArt2->MaxClusterNum; i++) {
        newArt2->FindMinDist1[i] = 0;
    }
    for ( i = 0; i < newArt2->MaxClusterNum; i++) {
        newArt2->FindMinDist2[i] = 0;
    }

    newArt2->winnerClass_1 = 0;
    newArt2->winnerClass_2 = 0;

    newArt2->prevWeights  = (float**)malloc(newArt2->MaxClusterNum*sizeof(float*));
    for ( i = 0; i < newArt2->MaxClusterNum; i++) {
        newArt2->prevWeights[i] = (float*)calloc(FeatureNum_1 + FeatureNum_2,sizeof(float));
    }
    newArt2->CurrWeights  = (float**)malloc(newArt2->MaxClusterNum*sizeof(float*));
    for ( i = 0; i < newArt2->MaxClusterNum; i++) {
        newArt2->CurrWeights[i] = (float*)calloc(FeatureNum_1 + FeatureNum_2,sizeof(float));
    }

    newArt2->CompareOldNewWeight = 0;


    newArt2->vigilanceVal_1 = vigilanceVal_1; // Vigilance Parameter
    newArt2->vigilanceVal_2 = vigilanceVal_2; // Vigilance Parameter

    newArt2->clusterMembers_1 = (int*)calloc(newArt2->MaxClusterNum,sizeof(int));
    for ( i = 0; i < newArt2->MaxClusterNum; i++) {
        newArt2->clusterMembers_1[i] = 0;
    }

    newArt2->clusterMembers_2 = (int*)calloc(newArt2->MaxClusterNum,sizeof(int));
    for ( i = 0; i < newArt2->MaxClusterNum; i++) {
        newArt2->clusterMembers_2[i] = 0;
    }


    newArt2->TotalDetectedClass         = 0;
    newArt2->detectedClass              = 0;
    newArt2->NewClusterCreationBufferSize = NewClusterCreationBufferSize;
    newArt2->FlagClusterCreation        = 0;
    newArt2->trainingCount              = 0;
    newArt2->SavingThreshold_1          = 0.001* FeatureNum_1; // for subband
    newArt2->SavingThreshold_2          = 0.001* FeatureNum_2; // for subband
    newArt2->SavingClusteringDataSwitch = SavingClusteringDataSwitch;
    newArt2->HybridClassificationSwitch = HybridClassificationSwitch;
    newArt2->pathHybridDir              = pathHybridDir;

    newArt2->classLabelName = (const char**)malloc(newArt2->MaxClusterNum*sizeof(const char*));
    for ( i = 0; i < newArt2->MaxClusterNum; i++)
    {
        newArt2->classLabelName[i] = (const char*)calloc(50,sizeof(const char));
    }
    newArt2->smoothingBufferLength          = DetectionDecisionBufferSize;//(int)(decisionRate*frequency)/(stepsize*inParam->decisionBufferLength);
    newArt2->DecisionSmoothingBuffer        = calloc(sizeof(int), newArt2->smoothingBufferLength);
    for (i = 0; i< newArt2->smoothingBufferLength; i++){
        newArt2->DecisionSmoothingBuffer[i] = 1;
    }
    // ==========================================================================
    // Hybrid Classification
    // Loading the previously created and saved clusters:
    //
    if (newArt2->HybridClassificationSwitch == 1) {
        FILE *file = fopen(pathHybridDir, "rb"); // binary read mode

        if (file != NULL){
            fread(&newArt2->TotalDetectedClass,sizeof(newArt2->TotalDetectedClass),1,file);
            fread(&(newArt2->MaxClusterNum),sizeof((newArt2->MaxClusterNum)),1,file);
            fread(&(newArt2->FeatureNum_1),sizeof((newArt2->FeatureNum_1)),1,file);
            fread(&(newArt2->FeatureNum_2),sizeof((newArt2->FeatureNum_2)),1,file);
            int rj, rk, rp;
            for( rj = 0; rj< newArt2->TotalDetectedClass; rj++){
                for( rk = 0; rk< (newArt2->FeatureNum_1 + newArt2->FeatureNum_2); rk++){
                    fread(&newArt2->CurrWeights[rj][rk],sizeof(newArt2->CurrWeights[0][0]),1,file);
                }
                fread(&newArt2->clusterMembers_1[rj],sizeof(newArt2->clusterMembers_1[0]),1,file);
                fread(&newArt2->clusterMembers_2[rj],sizeof(newArt2->clusterMembers_2[0]),1,file);

                for(rp = 0; rp < 25 ; rp++) {
                    //fwrite(&newArt2->classLabelName[rj][rp], sizeof(newArt2->classLabelName[rj][rp]), 1, file);
                }
            }
            fclose(file);
        }
        else{
            printf("\n Error! Can not open the file for reading \n");
        }
    } // end of reading file */
    //
    //==============================================================================
    return newArt2;
}

void computeArt2ParallelFusion(Art2FusionParallel* Art2Fusion,float* ExtractedFeaturesList, const char* classLabelName)
{
    Art2FusionParallel*  Art2Param = Art2Fusion;
    int i, j = 0;


    for (i = 0; i < (Art2Param->FeatureNum_1 + Art2Param->FeatureNum_2); i++)
    {
        Art2Param->Features[i] = ExtractedFeaturesList[i];
    }

    if (Art2Param->firstFrame == 1 && Art2Param->HybridClassificationSwitch == 0)  //Start the app and create the first cluster whenever hybrid mode is disable
    {
        for (i = 0; i < (Art2Param->FeatureNum_1 + Art2Param->FeatureNum_2); i++)
        {
            Art2Param->CurrWeights[Art2Param->detectedClass][i] = Art2Param->Features[i];
        }
        Art2Param->firstFrame         = 0;
        Art2Param->detectedClass      = 1;
        Art2Param->TotalDetectedClass = 1;
        Art2Param->clusterMembers_1[Art2Param->detectedClass - 1] =  1;
        Art2Param->clusterMembers_2[Art2Param->detectedClass - 1] =  1;
        Art2Param->classLabelName[Art2Param->detectedClass - 1] = classLabelName; //TODO Nasim
    }
    else
    {
        for (j = 0; j < Art2Param->TotalDetectedClass; j++)
        {
            Art2Param->FindMinDist1[j] = 0;
            Art2Param->FindMinDist2[j] = 0;

            for (i = 0; i < Art2Param->FeatureNum_1; i++) // compute distance of current frame with each cluster centroid
            {
                Art2Param->FindMinDist1[j] = Art2Param->FindMinDist1[j] + fabsf(Art2Param->Features[i] - Art2Param->CurrWeights[j][i])/Art2Param->FeatureNum_1;
            }

            for (i = Art2Param->FeatureNum_1; i < (Art2Param->FeatureNum_1 + Art2Param->FeatureNum_2) ; i++) // compute distance of current frame with each cluster centroid
            {
                Art2Param->FindMinDist2[j] = Art2Param->FindMinDist2[j] + fabsf(Art2Param->Features[i] - Art2Param->CurrWeights[j][i])/Art2Param->FeatureNum_2;
            }

            if (Art2Param->FindMinDist1[j] < Art2Param->MinDistVal_1) // find minimum distance
            {
                Art2Param->MinDistVal_1 = Art2Param->FindMinDist1[j];
                Art2Param->winnerClass_1 = j + 1;
            }

            if (Art2Param->FindMinDist2[j] < Art2Param->MinDistVal_2) // find minimum distance
            {
                Art2Param->MinDistVal_2 = Art2Param->FindMinDist2[j];
                Art2Param->winnerClass_2 = j + 1;
            }

        }

        if (  (Art2Param->MinDistVal_1 < Art2Param->vigilanceVal_1) | (Art2Param->MinDistVal_2 < Art2Param->vigilanceVal_2)  )
        {
            Art2Param->FlagClusterCreation = 0;

            if ((Art2Param->MinDistVal_1 < Art2Param->vigilanceVal_1) & (Art2Param->MinDistVal_2 < Art2Param->vigilanceVal_2)  ){

                for (i = 0; i < (Art2Param->FeatureNum_1 ); i++)
                {
                    //Art2Param->prevWeights[Art2Param->detectedClass - 1][i] = Art2Param->CurrWeights[Art2Param->detectedClass - 1][i];
                    Art2Param->CurrWeights[Art2Param->winnerClass_1 - 1][i] = (Art2Param->Features[i] + (Art2Param->CurrWeights[Art2Param->winnerClass_1 - 1][i] * Art2Param->clusterMembers_1[Art2Param->winnerClass_1 - 1])) / (Art2Param->clusterMembers_1[Art2Param->winnerClass_1 - 1] + 1);
                    //Art2Param->testWeight = Art2Param->testWeight + fabsf(Art2Param->prevWeights[Art2Param->detectedClass - 1][i] - Art2Param->CurrWeights[Art2Param->detectedClass - 1][i]);
                }
                for (i = Art2Param->FeatureNum_1; i < (Art2Param->FeatureNum_1 + Art2Param->FeatureNum_2); i++)
                {
                    //Art2Param->prevWeights[Art2Param->detectedClass - 1][i] = Art2Param->CurrWeights[Art2Param->detectedClass - 1][i];
                    Art2Param->CurrWeights[Art2Param->winnerClass_2 - 1][i] = (Art2Param->Features[i] + (Art2Param->CurrWeights[Art2Param->winnerClass_2 - 1][i] * Art2Param->clusterMembers_2[Art2Param->winnerClass_2 - 1])) / (Art2Param->clusterMembers_2[Art2Param->winnerClass_2 - 1] + 1);
                    //Art2Param->testWeight = Art2Param->testWeight + fabsf(Art2Param->prevWeights[Art2Param->detectedClass - 1][i] - Art2Param->CurrWeights[Art2Param->detectedClass - 1][i]);
                }
                Art2Param->clusterMembers_1[Art2Param->winnerClass_1 - 1] = Art2Param->clusterMembers_1[Art2Param->winnerClass_1 - 1] + 1;
                Art2Param->clusterMembers_2[Art2Param->winnerClass_2 - 1] = Art2Param->clusterMembers_2[Art2Param->winnerClass_2 - 1] + 1;

                //%------- Smoothing using median filter ------------------
                leftRotatebyD(Art2Param->DecisionSmoothingBuffer, 2, Art2Param->smoothingBufferLength); // shift left by two
                Art2Param->DecisionSmoothingBuffer[Art2Param->smoothingBufferLength- 2] = Art2Param->winnerClass_1;
                Art2Param->DecisionSmoothingBuffer[Art2Param->smoothingBufferLength- 1] = Art2Param->winnerClass_2;
                Art2Param->detectedClass = medianFilter1D(Art2Param->DecisionSmoothingBuffer, Art2Param->smoothingBufferLength); // apply 1D median filter
                leftRotatebyD(Art2Param->DecisionSmoothingBuffer, 1, Art2Param->smoothingBufferLength); // shift left by one
                Art2Param->DecisionSmoothingBuffer[Art2Param->smoothingBufferLength- 1] = Art2Param->detectedClass;
                //%--------------------------------------------------------
            }


            else if((Art2Param->MinDistVal_1 < Art2Param->vigilanceVal_1) & (Art2Param->MinDistVal_2 > Art2Param->vigilanceVal_2)  ){
                //update only first Art2
                for (i = 0; i < (Art2Param->FeatureNum_1 ); i++)
                {
                    //Art2Param->prevWeights[Art2Param->detectedClass - 1][i] = Art2Param->CurrWeights[Art2Param->detectedClass - 1][i];
                    Art2Param->CurrWeights[Art2Param->winnerClass_1 - 1][i] = (Art2Param->Features[i] + (Art2Param->CurrWeights[Art2Param->winnerClass_1 - 1][i] * Art2Param->clusterMembers_1[Art2Param->winnerClass_1 - 1])) / (Art2Param->clusterMembers_1[Art2Param->winnerClass_1 - 1] + 1);
                    //Art2Param->testWeight = Art2Param->testWeight + fabsf(Art2Param->prevWeights[Art2Param->detectedClass - 1][i] - Art2Param->CurrWeights[Art2Param->detectedClass - 1][i]);
                }
                Art2Param->clusterMembers_1[Art2Param->winnerClass_1 - 1] = Art2Param->clusterMembers_1[Art2Param->winnerClass_1 - 1] + 1;
                //%------- Smoothing using median filter ------------------
                leftRotatebyD(Art2Param->DecisionSmoothingBuffer, 1, Art2Param->smoothingBufferLength); // shift left by one
                Art2Param->DecisionSmoothingBuffer[Art2Param->smoothingBufferLength- 1] = Art2Param->winnerClass_1;
                Art2Param->detectedClass = medianFilter1D(Art2Param->DecisionSmoothingBuffer, Art2Param->smoothingBufferLength); // apply 1D median filter
                leftRotatebyD(Art2Param->DecisionSmoothingBuffer, 1, Art2Param->smoothingBufferLength); // shift left by one
                Art2Param->DecisionSmoothingBuffer[Art2Param->smoothingBufferLength- 1] = Art2Param->detectedClass;
                //%--------------------------------------------------------

            }
            else if ((Art2Param->MinDistVal_1 > Art2Param->vigilanceVal_1) & (Art2Param->MinDistVal_2 < Art2Param->vigilanceVal_2)  ){
                // update only second Art2
                for (i = Art2Param->FeatureNum_1; i < (Art2Param->FeatureNum_1 + Art2Param->FeatureNum_2); i++)
                {
                    //Art2Param->prevWeights[Art2Param->detectedClass - 1][i] = Art2Param->CurrWeights[Art2Param->detectedClass - 1][i];
                    Art2Param->CurrWeights[Art2Param->winnerClass_2 - 1][i] = (Art2Param->Features[i] + (Art2Param->CurrWeights[Art2Param->winnerClass_2 - 1][i] * Art2Param->clusterMembers_2[Art2Param->winnerClass_2 - 1])) / (Art2Param->clusterMembers_2[Art2Param->winnerClass_2 - 1] + 1);
                    //Art2Param->testWeight = Art2Param->testWeight + fabsf(Art2Param->prevWeights[Art2Param->detectedClass - 1][i] - Art2Param->CurrWeights[Art2Param->detectedClass - 1][i]);
                }
                Art2Param->clusterMembers_2[Art2Param->winnerClass_2 - 1] = Art2Param->clusterMembers_2[Art2Param->winnerClass_2 - 1] + 1;

                //%------- Smoothing using median filter ------------------
                leftRotatebyD(Art2Param->DecisionSmoothingBuffer, 1, Art2Param->smoothingBufferLength); // shift left by one
                Art2Param->DecisionSmoothingBuffer[Art2Param->smoothingBufferLength- 1] = Art2Param->winnerClass_2;
                Art2Param->detectedClass = medianFilter1D(Art2Param->DecisionSmoothingBuffer, Art2Param->smoothingBufferLength); // apply 1D median filter
                leftRotatebyD(Art2Param->DecisionSmoothingBuffer, 1, Art2Param->smoothingBufferLength); // shift left by one
                Art2Param->DecisionSmoothingBuffer[Art2Param->smoothingBufferLength- 1] = Art2Param->detectedClass;
                //%--------------------------------------------------------
            }


        } // end of updating weights

        else if ((Art2Param->MinDistVal_1 > Art2Param->vigilanceVal_1) & (Art2Param->MinDistVal_2 > Art2Param->vigilanceVal_2) )
        {
            if (Art2Param->TotalDetectedClass < Art2Param->MaxClusterNum)  {
                if (Art2Param->FlagClusterCreation == Art2Param->NewClusterCreationBufferSize) {

                    Art2Param->TotalDetectedClass = Art2Param->TotalDetectedClass + 1;
                    Art2Param->detectedClass = Art2Param->TotalDetectedClass;

                    for (i = 0; i < (Art2Param->FeatureNum_1 + Art2Param->FeatureNum_2); i++) {
                        Art2Param->CurrWeights[Art2Param->detectedClass - 1][i] = Art2Param->Features[i];
                    }
                    Art2Param->clusterMembers_1[Art2Param->detectedClass - 1] = 1;
                    Art2Param->clusterMembers_2[Art2Param->detectedClass - 1] = 1;
                    Art2Param->FlagClusterCreation = 0;
                    //%------- Smoothing using median filter ------------------
                    leftRotatebyD(Art2Param->DecisionSmoothingBuffer, 1, Art2Param->smoothingBufferLength); // shift left by one
                    Art2Param->DecisionSmoothingBuffer[Art2Param->smoothingBufferLength- 1] = Art2Param->detectedClass;
                    //%--------------------------------------------------------
                    if (Art2Param->HybridClassificationSwitch == 1 & Art2Param->SavingClusteringDataSwitch == 0) // TODO Nasim Add this
                    {
                        Art2Param->classLabelName[Art2Param->detectedClass - 1] = "SaveNewClass";
                    }
                    else {
                        Art2Param->classLabelName[Art2Param->detectedClass - 1] = classLabelName;
                        //TODO Nasim: only one class at a time
                    }
                } else // if cluster creation chunk is still filling
                {
                    Art2Param->FlagClusterCreation++;
                }
            }
        } // end of else if
    }

    if (Art2Param->SavingClusteringDataSwitch == 1 ) {
        FILE *file = fopen(Art2Param->pathHybridDir, "wb");
        //A DAT file is a generic data file created by a specific application.  DAT files are typically accessed only by the application that created them are not meant to be opened manually by the user.

        if (file != NULL) {
            fwrite(&Art2Param->TotalDetectedClass, sizeof(Art2Param->TotalDetectedClass), 1, file);
            fwrite(&(Art2Param->MaxClusterNum), sizeof((Art2Param->MaxClusterNum)), 1, file);
            fwrite(&Art2Param->FeatureNum_1, sizeof((Art2Param->FeatureNum_1)), 1, file);
            fwrite(&Art2Param->FeatureNum_2, sizeof(Art2Param->FeatureNum_2), 1, file);
            int wi, wj, wk;
            for (wi = 0; wi < Art2Param->TotalDetectedClass; wi++) {
                for (wj = 0; wj < (Art2Param->FeatureNum_1 + Art2Param->FeatureNum_2); wj++) {
                    fwrite(&Art2Param->CurrWeights[wi][wj], sizeof(Art2Param->CurrWeights[wi][wj]), 1, file);
                }
                fwrite(&Art2Param->clusterMembers_1[wi], sizeof(Art2Param->clusterMembers_1[wi]), 1, file);
                fwrite(&Art2Param->clusterMembers_2[wi], sizeof(Art2Param->clusterMembers_2[wi]), 1, file);

                for(wk = 0; wk < 50 ; wk++) {
                    //fwrite(&Art2Param->classLabelName[wi][wk], sizeof(Art2Param->classLabelName[wi][wk]), 1, file);
                }
            }
            fclose(file);
        }
        else{
            printf("\n Error! Can not open the file for writing \n");
        }
    }

}

void destroyArt2ParallelFusion(Art2FusionParallel** Art2Fusion){

    
    int i;
    if(*Art2Fusion != NULL){
        for (i=0;i< 10;i++)//Maximum nimber of class creation hard coded as 10: MaxClusterNum
        {
            if((*Art2Fusion)->prevWeights[i] != NULL){
                free((*Art2Fusion)->prevWeights[i]);
                (*Art2Fusion)->prevWeights[i]=NULL;
            }
            if((*Art2Fusion)->CurrWeights[i] != NULL){
                free((*Art2Fusion)->CurrWeights[i]);
                (*Art2Fusion)->CurrWeights[i]=NULL;
            }
        }
     
        if((*Art2Fusion)->Features != NULL){
            free((*Art2Fusion)->Features);
            (*Art2Fusion)->Features = NULL;
        }
        if((*Art2Fusion)->FindMinDist1 != NULL){
            free((*Art2Fusion)->FindMinDist1);
            (*Art2Fusion)->FindMinDist1 = NULL;
        }
        if((*Art2Fusion)->FindMinDist2 != NULL){
            free((*Art2Fusion)->FindMinDist2);
            (*Art2Fusion)->FindMinDist2 = NULL;
        }
        if((*Art2Fusion)->clusterMembers_1 != NULL){
            free((*Art2Fusion)->clusterMembers_1);
            (*Art2Fusion)->clusterMembers_1 = NULL;
        }
        if((*Art2Fusion)->clusterMembers_2 != NULL){
            free((*Art2Fusion)->clusterMembers_2);
            (*Art2Fusion)->clusterMembers_2 = NULL;
        }
        if((*Art2Fusion)->DecisionSmoothingBuffer != NULL){
            free((*Art2Fusion)->DecisionSmoothingBuffer);
            (*Art2Fusion)->DecisionSmoothingBuffer = NULL;
        }

        free(*Art2Fusion);
        *Art2Fusion=NULL;
        
    }
}


