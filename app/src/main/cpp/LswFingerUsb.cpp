//
// Created by Administrator on 2022/3/25.
//

#include "LswFingerUsb.h"
#include "common.h"
#include <jni.h>
#include <string.h>
#include "libusb.h"
#include <android/log.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>


int FingerApiInit()
{
    LOGD("FingerApiInit\n");
    libusb_context *ctx = NULL;
    libusb_device_handle *devh = NULL;
    int r = 0;
    r = libusb_set_option(NULL, LIBUSB_OPTION_NO_DEVICE_DISCOVERY, NULL);
    if (r != LIBUSB_SUCCESS) {
        LOGD("libusb_set_option failed: %d\n", r);
        return -1;
    }
    LOGD("libusb_set_option success.");
    r = libusb_init(&ctx);
    if (r < 0) {
        LOGD("libusb_init failed: %d\n", r);
        return r;
    }
    LOGD("libusb_init success.");
    libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_WARNING);
}

int FingerApiOpen()
{

}

int FingerApiClose()
{

}

int FingerApiTest()
{

}

int FingerApiGatherRawFinger()
{

}

int FingerApiGatherFingerDelBg()
{

}

int FingerApiCalibration()
{

}

int FingerApiVersion()
{

}