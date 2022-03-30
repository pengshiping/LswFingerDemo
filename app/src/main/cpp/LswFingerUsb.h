//
// Created by Administrator on 2022/3/25.
//

#ifndef LSWFINGERDEMO_LSWFINGERUSB_H
#define LSWFINGERDEMO_LSWFINGERUSB_H

#include "common.h"

int FingerApiInit(int fd);

int FingerApiOpen();

int FingerApiClose();

int FingerApiTest();

unsigned char* FingerApiGatherRawFinger();

int FingerApiCalibration();

int FingerApiVersion();


//以下是指纹比对的
int FingerDownloadImage(unsigned char * imageBuffer);

int FingerDownloadFeature0(unsigned char* featureBuffer, int featureLength);

int FingerDownloadFeature1(unsigned char* featureBuffer);

int FingerFeatureMatch(unsigned char* match_result);

int FingerImageMatch(unsigned char* match_result);


#endif //LSWFINGERDEMO_LSWFINGERUSB_H
