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
    unsigned char *fingerBuffer = FingerApiGatherRawFinger();
    if (fingerBuffer == NULL) {
        // LOGE("FingerApiGatherRawFinger error");
    } else {
        // LOGI("FingerApiGatherRawFinger success");
        dataArray = env->NewByteArray(92160);
        env->SetByteArrayRegion(dataArray, 0, 92160, (jbyte *) fingerBuffer);
        free(fingerBuffer);
    }
    return dataArray;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerDownloadImage(JNIEnv *env, jclass clazz,
                                                            jbyteArray image_buffer) {
    jbyte* bBuffer = env->GetByteArrayElements(image_buffer, 0);
    return FingerDownloadImage((unsigned char *) bBuffer);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerDownloadFeature0(JNIEnv *env, jclass clazz,
                                                               jbyteArray feature_buffer,
                                                               jint feature_length) {
    jbyte* bBuffer = env->GetByteArrayElements(feature_buffer, 0);
    int ret = FingerDownloadFeature0((unsigned char *) bBuffer, feature_length);
    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerDownloadFeature1(JNIEnv *env, jclass clazz,
                                                               jbyteArray feature_buffer) {
    jbyte* bBuffer = env->GetByteArrayElements(feature_buffer, 0);
    return FingerDownloadFeature1((unsigned char *) bBuffer);
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerFeatureMatch(JNIEnv *env, jclass clazz) {
    unsigned char matchResult[1];
    memset(matchResult, 0, 1);
    int ret = FingerFeatureMatch((unsigned char *) matchResult);
    if (ret == 0) {
        jbyteArray dataArray = env->NewByteArray(1);
        env->SetByteArrayRegion(dataArray, 0, 1, (jbyte *) matchResult);
        return dataArray;
    } else {
        return NULL;
    }
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_lsw_fingerdemo_LswFingerApi_lswFingerImageMatch(JNIEnv *env, jclass clazz) {
    unsigned char matchResult[2];
    memset(matchResult, 0, 2);
    int ret = FingerImageMatch((unsigned char *) matchResult);
    if (ret == 0) {
        jbyteArray dataArray = env->NewByteArray(2);
        env->SetByteArrayRegion(dataArray, 0, 2, (jbyte *) matchResult);
        return dataArray;
    } else {
        return NULL;
    }
}