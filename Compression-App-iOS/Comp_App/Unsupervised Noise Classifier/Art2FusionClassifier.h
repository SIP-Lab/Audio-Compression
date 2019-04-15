//
// Created by Taghizadeh Alamdari, Nasim on 5/25/18.
//

#ifndef CNN_VAD_OFC_ART2_ANDROID_2_ART2FUSIONCLASSIFIER_H
#define CNN_VAD_OFC_ART2_ANDROID_2_ART2FUSIONCLASSIFIER_H

typedef struct Art2FusionParallel {
    float* Features; // input

    float* FindMinDist1; // a parameter to find minimum distance of the input with existing clusters's centroid
    float* FindMinDist2; // a parameter to find minimum distance of the input with existing clusters's centroid

    float MinDistVal_1;
    float MinDistVal_2;

    float vigilanceVal_1;
    float vigilanceVal_2;

    int FeatureNum_1; // number of features in the first Art2
    int FeatureNum_2;  // number of features in the second Art2

    float CompareOldNewWeight;
    float** prevWeights;
    float** CurrWeights;

    int* clusterMembers_1; // #frames in each cluster in the first Art2
    int* clusterMembers_2; // #frames in each cluster in the 2nd Art2

    int winnerClass_1;
    int winnerClass_2;

    int firstFrame;
    int NewClusterCreationBufferSize; // new cluster creation buffer
    int FlagClusterCreation;
    int trainingCount;
    int detectedClass;
    int TotalDetectedClass;
    int MaxClusterNum; // maximum number of classes we want to detect
    int HybridClassificationSwitch;
    int SavingClusteringDataSwitch;
    const char** classLabelName;
    int SavingThreshold_1;
    int SavingThreshold_2;
    const char* pathHybridDir;
    int smoothingBufferLength;
    int* DecisionSmoothingBuffer;

} Art2FusionParallel;

Art2FusionParallel* initArt2ParallelFusion(int SavingClusteringDataSwitch,
                                          int HybridClassificationSwitch,
                                          float vigilanceVal_1,
                                          float vigilanceVal_2,
                                          const char* pathHybridDir,
                                          int FeatureNum_1,
                                          int FeatureNum_2,
                                          int NewClusterCreationBufferSize,
                                          int DetectionDecisionBufferSize,
                                          int MaxClusterNum);
void computeArt2ParallelFusion(Art2FusionParallel* Art2Fusion, float* ExtractedFeaturesList, const char* classLabelName);
void destroyArt2ParallelFusion(Art2FusionParallel** Art2Fusion);
#endif //CNN_VAD_OFC_ART2_ANDROID_2_ART2FUSIONCLASSIFIER_H
