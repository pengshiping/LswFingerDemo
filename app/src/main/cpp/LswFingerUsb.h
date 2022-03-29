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

unsigned char* FingerApiGatherDelBgFinger();

int FingerApiGatherFingerDelBg();

int FingerApiCalibration();

int FingerApiVersion();


#endif //LSWFINGERDEMO_LSWFINGERUSB_H
