//
// Created by Administrator on 2022/3/25.
//


#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "common.h"
#include "LswFingerUsb.h"




extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerApiInit(JNIEnv *env, jclass clazz, jint fd) {
    LOGD("lswFingerApiInit");
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
    FingerApiClose();
    return 0;
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
    LOGD("lswFingerApiGatherRawFinger");
    jbyteArray dataArray = NULL;
    unsigned char* buffer = FingerApiGatherRawFinger();
    return dataArray;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerApiGatherFingerDelBg(JNIEnv *env, jclass clazz) {
    LOGD("lswFingerApiGatherFingerDelBg");
    return 0;
}