//
// Created by Administrator on 2022/3/25.
//


#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <cstdlib>
#include "common.h"
#include "LswFingerUsb.h"
#include <malloc.h>




extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerApiInit(JNIEnv *env, jclass clazz, jint fd) {
    LOGD("lswFingerApiInit, fd:%d", fd);
    FingerApiInit(fd);
    return 0;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerApiOpen(JNIEnv *env, jclass clazz) {
    LOGD("lswFingerApiOpen");
    return FingerApiOpen();
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerApiClose(JNIEnv *env, jclass clazz) {
    LOGD("lswFingerApiClose");
    return FingerApiClose();
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerApiTest(JNIEnv *env, jclass clazz) {
    LOGD("lswFingerApiTest");
    return FingerApiTest();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerApiCalibration(JNIEnv *env, jclass clazz) {
    LOGD("lswFingerApiCalibration");
    return FingerApiCalibration();
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerApiVersion(JNIEnv *env, jclass clazz) {
    LOGD("lswFingerApiVersion");
    return 0;
}
extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerApiGatherRawFinger(JNIEnv *env, jclass clazz) {
    //LOGD("lswFingerApiGatherRawFinger");
    jbyteArray dataArray = NULL;
    unsigned char* fingerBuffer = FingerApiGatherRawFinger();
    if (fingerBuffer==NULL) {
       // LOGE("FingerApiGatherRawFinger error");
    } else {
       // LOGI("FingerApiGatherRawFinger success");
        dataArray = env->NewByteArray(92160);
        env->SetByteArrayRegion(dataArray,0,92160, (jbyte *)fingerBuffer);
        free(fingerBuffer);
    }
    return dataArray;
}
extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerApiGatherDelBgFinger(JNIEnv *env, jclass clazz) {
    LOGD("lswFingerApiGatherDelBgFinger");
    jbyteArray dataArray = NULL;
    unsigned char* fingerBuffer = FingerApiGatherDelBgFinger();
    if (fingerBuffer==NULL) {
        LOGE("FingerApiGatherRawFinger error");
    } else {
        LOGI("FingerApiGatherRawFinger success");
        dataArray = env->NewByteArray(92160);
        env->SetByteArrayRegion(dataArray,0,92160, (jbyte *)fingerBuffer);
        free(fingerBuffer);
    }
    return dataArray;
}