#ifndef LSWFINGERDEMO_COMMON_H
#define LSWFINGERDEMO_COMMON_H

#include <unistd.h>
#include <stdio.h>
#include <jni.h>
#include <string.h>
#include "libusb.h"
#include <android/log.h>

#define  LOG_TAG    "LswLibUsb"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)



#endif //LSWFINGERDEMO_COMMON_H