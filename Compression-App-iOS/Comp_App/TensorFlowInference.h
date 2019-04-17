//
//  TensorFlowInference.h
//  Integrated_App
//
//  Created by Abhishek Sehgal on 7/20/18.
//  Copyright © 2018 UT Dallas. All rights reserved.
//

#import <Foundation/Foundation.h>
#ifdef __cplusplus
extern "C"{
#endif
#import "Settings.h"
#import "compute_Comp.h"
#ifdef __cplusplus
}
#endif

@interface TensorFlowInference : NSObject
- (BOOL)loadGraphFromPath:(NSString *)path;
- (BOOL)createSession;
- (void)predict: (CompressionVars *) vadNoiseRedCom  settings: (Settings *) settings;
@end
