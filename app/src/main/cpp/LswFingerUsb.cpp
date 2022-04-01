//
// Created by Administrator on 2022/3/25.
//

#include "LswFingerUsb.h"
#include <jni.h>
#include <string.h>
#include "libusb.h"
#include <android/log.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <malloc.h>

#define BULK_RECV_EP    0x81
#define BULK_SEND_EP    0x01

static int image_number = 0;

// 指纹图像
#define  IMAGElENGTH 92160
unsigned char finger_img_buffer[IMAGElENGTH];//256 * 360 = 92160字节

libusb_device_handle *dev_handle;

unsigned char cal_xor_checkcode(unsigned char *pBuf, int nLen) {
    int i;
    unsigned char ret = 0;
    for (i = 0; i < nLen; i++) {
        ret ^= pBuf[i];
    }
    return ret;
}

unsigned char cal_sum_checkcode(unsigned char *pBuf, int nLen) {
    int i;
    unsigned char ret = 0;
    for (i = 0; i < nLen; i++) {
        ret += pBuf[i];
    }
    ret = ~ret;
    return ret;
}

void printfarray(unsigned char *array, int length) {
    char printstr[512];
    memset(printstr, 0, 512);
    sprintf(printstr, "[%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x]", array[0], array[1], array[2],
            array[3],
            array[4], array[5], array[6], array[7], array[8], array[9], array[10], array[11],
            array[12]);\
    LOGD("%s", printstr);
}

//采用bulk端点发送数据
static int bulk_send(unsigned char *buf, int num) {
    int size;
    int rec;
    // LOGD("bulk send length: %d\n", num);
    rec = libusb_bulk_transfer(dev_handle, BULK_SEND_EP, buf, num, &size, 1000);
    // LOGD("libusb_bulk_transfer return: %d\n", rec);
    if (rec == 0) {
        // LOGD("libusb_bulk_transfer send sucess,length: %d bytes\n", size);
    } else {
        //LOGD("libusb_bulk_transfer send faild, err: %s\n", libusb_error_name(rec));
    }
    return rec;
}

static int cmdSwap(unsigned char *buf, int num) {
    int ret = -1;
    unsigned char recvBuf[512];
    int retSize = 0;
    memset(recvBuf, 0, 512);
   // LOGD("cmdSwap send.");
   // printfarray(buf, 13);
    ret = libusb_bulk_transfer(dev_handle, BULK_SEND_EP, buf, num, &retSize, 1000);
    if (ret == 0) {
        //LOGD("cmdSwap send sucess,length: %d bytes\n", retSize);
    } else {
        LOGE("cmdSwap send faild, err: %s\n", libusb_error_name(ret));
        return -1;
    }
    //LOGD("cmdSwap recv.");
    ret = libusb_bulk_transfer(dev_handle, BULK_RECV_EP, recvBuf, 512, &retSize, 1000);
    if (ret == 0) {
       // LOGD("cmdSwap recv sucess, length: %d bytes. \n", retSize);
        //printfarray(recvBuf, 13);
        if ((recvBuf[0] == 0xf0) && (recvBuf[1] == 0x00) && (recvBuf[2] == 0x08) && (recvBuf[3] == 0x01 || recvBuf[3] == 0x02)) {
            if (buf[2] == recvBuf[4] && buf[3] == recvBuf[5] &&buf[4] == recvBuf[6]) {
                //LOGD("cmdSwap send and recv is equals.");
                return 0;
            } else {
                LOGE("cmdSwap send and recv is not equals.");
                return -1;
            }
        } else {
            LOGE("cmdSwap recv sucess, but recv value is error.");
        }
        return -1;
    } else {
        LOGE("cmdSwap recv faild, err: %s\n", libusb_error_name(ret));
    }
    return ret;
}


static int cmdSwap(unsigned char *buf, int num, unsigned char *recvBuf, int* retSize) {
    int ret = -1;
    //LOGD("cmdSwap send.");
    //printfarray(buf, 13);
    ret = libusb_bulk_transfer(dev_handle, BULK_SEND_EP, buf, num, retSize, 1000);
    if (ret == 0) {
       // LOGD("cmdSwap send sucess,length: %d bytes\n", *retSize);
    } else {
        LOGE("cmdSwap send faild, err: %s\n", libusb_error_name(ret));
        return -1;
    }
    //LOGD("cmdSwap recv.");
    ret = libusb_bulk_transfer(dev_handle, BULK_RECV_EP, recvBuf, 512, retSize, 1000);
    if (ret == 0) {
        //LOGD("cmdSwap recv sucess, length: %d bytes. \n", *retSize);
        //printfarray(recvBuf, 13);
        if ((recvBuf[0] == 0xf0) && (recvBuf[1] == 0x00) && (recvBuf[2] == 0x08) && (recvBuf[3] == 0x01)) {
            if (buf[2] == recvBuf[4] && buf[3] == recvBuf[5] &&buf[4] == recvBuf[6] &&buf[5] == recvBuf[7]) {
                //LOGD("cmdSwap send and recv is equals.");
                return 0;
            } else {
                LOGE("cmdSwap send and recv is not equals.");
                return -1;
            }
        } else {
            LOGE("cmdSwap recv sucess, but recv value is error.");
        }
        return -1;
    } else {
        LOGE("cmdSwap recv faild, err: %s\n", libusb_error_name(ret));
    }
    return ret;
}


static void print_endpoint_comp(const struct libusb_ss_endpoint_companion_descriptor *ep_comp) {
    LOGD("      USB 3.0 Endpoint Companion:\n");
    LOGD("        bMaxBurst:           %u\n", ep_comp->bMaxBurst);
    LOGD("        bmAttributes:        %02xh\n", ep_comp->bmAttributes);
    LOGD("        wBytesPerInterval:   %u\n", ep_comp->wBytesPerInterval);
}

static void print_endpoint(const struct libusb_endpoint_descriptor *endpoint) {
    int i, ret;

    LOGD("      Endpoint:\n");
    LOGD("        bEndpointAddress:    %02xh\n", endpoint->bEndpointAddress);
    LOGD("        bmAttributes:        %02xh\n", endpoint->bmAttributes);
    LOGD("        wMaxPacketSize:      %u\n", endpoint->wMaxPacketSize);
    LOGD("        bInterval:           %u\n", endpoint->bInterval);
    LOGD("        bRefresh:            %u\n", endpoint->bRefresh);
    LOGD("        bSynchAddress:       %u\n", endpoint->bSynchAddress);

    for (i = 0; i < endpoint->extra_length;) {
        if (LIBUSB_DT_SS_ENDPOINT_COMPANION == endpoint->extra[i + 1]) {
            struct libusb_ss_endpoint_companion_descriptor *ep_comp;

            ret = libusb_get_ss_endpoint_companion_descriptor(NULL, endpoint, &ep_comp);
            if (LIBUSB_SUCCESS != ret)
                continue;

            print_endpoint_comp(ep_comp);

            libusb_free_ss_endpoint_companion_descriptor(ep_comp);
        }

        i += endpoint->extra[i];
    }
}

static void print_altsetting(const struct libusb_interface_descriptor *interface) {
    uint8_t i;

    LOGD("    Interface:\n");
    LOGD("      bInterfaceNumber:      %u\n", interface->bInterfaceNumber);
    LOGD("      bAlternateSetting:     %u\n", interface->bAlternateSetting);
    LOGD("      bNumEndpoints:         %u\n", interface->bNumEndpoints);
    LOGD("      bInterfaceClass:       %u\n", interface->bInterfaceClass);
    LOGD("      bInterfaceSubClass:    %u\n", interface->bInterfaceSubClass);
    LOGD("      bInterfaceProtocol:    %u\n", interface->bInterfaceProtocol);
    LOGD("      iInterface:            %u\n", interface->iInterface);

    for (i = 0; i < interface->bNumEndpoints; i++)
        print_endpoint(&interface->endpoint[i]);
}

static void print_2_0_ext_cap(struct libusb_usb_2_0_extension_descriptor *usb_2_0_ext_cap) {
    LOGD("    USB 2.0 Extension Capabilities:\n");
    LOGD("      bDevCapabilityType:    %u\n", usb_2_0_ext_cap->bDevCapabilityType);
    LOGD("      bmAttributes:          %08xh\n", usb_2_0_ext_cap->bmAttributes);
}

static void print_ss_usb_cap(struct libusb_ss_usb_device_capability_descriptor *ss_usb_cap) {
    LOGD("    USB 3.0 Capabilities:\n");
    LOGD("      bDevCapabilityType:    %u\n", ss_usb_cap->bDevCapabilityType);
    LOGD("      bmAttributes:          %02xh\n", ss_usb_cap->bmAttributes);
    LOGD("      wSpeedSupported:       %u\n", ss_usb_cap->wSpeedSupported);
    LOGD("      bFunctionalitySupport: %u\n", ss_usb_cap->bFunctionalitySupport);
    LOGD("      bU1devExitLat:         %u\n", ss_usb_cap->bU1DevExitLat);
    LOGD("      bU2devExitLat:         %u\n", ss_usb_cap->bU2DevExitLat);
}

static void print_bos(libusb_device_handle *handle) {
    struct libusb_bos_descriptor *bos;
    uint8_t i;
    int ret;

    ret = libusb_get_bos_descriptor(handle, &bos);
    if (ret < 0)
        return;

    LOGD("  Binary Object Store (BOS):\n");
    LOGD("    wTotalLength:            %u\n", bos->wTotalLength);
    LOGD("    bNumDeviceCaps:          %u\n", bos->bNumDeviceCaps);

    for (i = 0; i < bos->bNumDeviceCaps; i++) {
        struct libusb_bos_dev_capability_descriptor *dev_cap = bos->dev_capability[i];

        if (dev_cap->bDevCapabilityType == LIBUSB_BT_USB_2_0_EXTENSION) {
            struct libusb_usb_2_0_extension_descriptor *usb_2_0_extension;

            ret = libusb_get_usb_2_0_extension_descriptor(NULL, dev_cap, &usb_2_0_extension);
            if (ret < 0)
                return;

            print_2_0_ext_cap(usb_2_0_extension);
            libusb_free_usb_2_0_extension_descriptor(usb_2_0_extension);
        } else if (dev_cap->bDevCapabilityType == LIBUSB_BT_SS_USB_DEVICE_CAPABILITY) {
            struct libusb_ss_usb_device_capability_descriptor *ss_dev_cap;

            ret = libusb_get_ss_usb_device_capability_descriptor(NULL, dev_cap, &ss_dev_cap);
            if (ret < 0)
                return;

            print_ss_usb_cap(ss_dev_cap);
            libusb_free_ss_usb_device_capability_descriptor(ss_dev_cap);
        }
    }

    libusb_free_bos_descriptor(bos);
}

static void print_interface(const struct libusb_interface *interface) {
    int i;

    for (i = 0; i < interface->num_altsetting; i++)
        print_altsetting(&interface->altsetting[i]);
}

static void print_configuration(struct libusb_config_descriptor *config) {
    uint8_t i;

    LOGD("  Configuration:\n");
    LOGD("    wTotalLength:            %u\n", config->wTotalLength);
    LOGD("    bNumInterfaces:          %u\n", config->bNumInterfaces);
    LOGD("    bConfigurationValue:     %u\n", config->bConfigurationValue);
    LOGD("    iConfiguration:          %u\n", config->iConfiguration);
    LOGD("    bmAttributes:            %02xh\n", config->bmAttributes);
    LOGD("    MaxPower:                %u\n", config->MaxPower);

    for (i = 0; i < config->bNumInterfaces; i++)
        print_interface(&config->interface[i]);
}

static void print_device(libusb_device *dev, libusb_device_handle *handle) {
    LOGD("print_device");

    struct libusb_device_descriptor desc;
    unsigned char string[256];
    const char *speed;
    int ret;
    int verbose;
    uint8_t i;

    switch (libusb_get_device_speed(dev)) {
        case LIBUSB_SPEED_LOW:
            speed = "1.5M";
            break;
        case LIBUSB_SPEED_FULL:
            speed = "12M";
            break;
        case LIBUSB_SPEED_HIGH:
            speed = "480M";
            break;
        case LIBUSB_SPEED_SUPER:
            speed = "5G";
            break;
        case LIBUSB_SPEED_SUPER_PLUS:
            speed = "10G";
            break;
        default:
            speed = "Unknown";
    }

    ret = libusb_get_device_descriptor(dev, &desc);
    if (ret < 0) {
        LOGD("failed to get device descriptor");
        return;
    }

    LOGD("Dev (bus %u, device %u): %04X - %04X speed: %s\n",
         libusb_get_bus_number(dev), libusb_get_device_address(dev),
         desc.idVendor, desc.idProduct, speed);

    if (!handle)
        libusb_open(dev, &handle);

    if (handle) {
        if (desc.iManufacturer) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, string,
                                                     sizeof(string));
            if (ret > 0)
                LOGD("  Manufacturer:              %s\n", (char *) string);
        }

        if (desc.iProduct) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, string, sizeof(string));
            if (ret > 0)
                LOGD("  Product:                   %s\n", (char *) string);
        }

        if (desc.iSerialNumber && verbose) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, string,
                                                     sizeof(string));
            if (ret > 0)
                LOGD("  Serial Number:             %s\n", (char *) string);
        }
    }

    if (verbose) {
        for (i = 0; i < desc.bNumConfigurations; i++) {
            struct libusb_config_descriptor *config;

            ret = libusb_get_config_descriptor(dev, i, &config);
            if (LIBUSB_SUCCESS != ret) {
                LOGD("  Couldn't retrieve descriptors\n");
                continue;
            }

            print_configuration(config);

            libusb_free_config_descriptor(config);
        }

        if (handle && desc.bcdUSB >= 0x0201)
            print_bos(handle);
    }
}

int FingerApiInit(int fd) {
    LOGD("FingerApiInit\n");
    libusb_context *ctx = NULL;
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
    r = libusb_wrap_sys_device(ctx, (intptr_t) fd, &dev_handle);
    if (r < 0) {
        LOGD("libusb_wrap_sys_device failed: %d\n", r);
        return r;
    } else if (dev_handle == NULL) {
        LOGD("libusb_wrap_sys_device returned invalid handle\n");
        return r;
    }

    print_device(libusb_get_device(dev_handle), dev_handle);

    r = libusb_kernel_driver_active(dev_handle, 0);
    if (r == 0)  //ok
    {
        LOGD("libusb_kernel_driver_active is ok.");
    } else if (r == 1) {
        LOGD("Kernel driver is active, now try detached\n");
        if (libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
            LOGD("Kernel driver is detached!\n");
        else {
            LOGD("libusb_detach_kernel_driver, err:%s\n", libusb_error_name(r));
            return -1;
        }
    } else {
        LOGD("libusb_kernel_driver_active, err:%s\n", libusb_error_name(r));
        return -1;
    }

    r = libusb_claim_interface(dev_handle, 0); //claim interface 0 ,stm32采用接口0
    if (r < 0) {
        LOGD("cannot claim interface, err:%s\n", libusb_error_name(r));
        return -1;
    } else {
        LOGD("libusb_claim_interface success.\n");
    }

    return 0;
}

int FingerApiOpen() {
    uint8_t open_cmd[512];
    memset(open_cmd, 0, 512);

    open_cmd[0] = 0xF0;
    open_cmd[1] = 0x6A;

    open_cmd[2] = 0xA6;
    open_cmd[4] = 0x01;
    open_cmd[6] = 0x04;

    open_cmd[7] = cal_xor_checkcode(open_cmd + 1, 6); //0xC9;
    open_cmd[12] = cal_sum_checkcode(open_cmd + 1, 12);//0x21;
    int ret = cmdSwap(open_cmd, 512);
    if ( ret == 0){
        LOGD("FingerApiOpen success.");
    } else {
        LOGE("FingerApiOpen failed.");
    }
    return ret;
}

int FingerApiClose() {
    //发送关闭模组命令
    uint8_t close_cmd[512];
    memset(close_cmd, 0, 512);

    close_cmd[0] = 0xF0;
    close_cmd[1] = 0x6A;

    close_cmd[2] = 0xA6;
    close_cmd[4] = 0x02;

    close_cmd[7] = cal_xor_checkcode(close_cmd + 1, 6); //0xC9;
    close_cmd[8] = cal_sum_checkcode(close_cmd + 1, 8); //0x21;

    int ret = cmdSwap(close_cmd, 512);
    if ( ret == 0){
        LOGD("FingerApiClose success.");
    } else {
        LOGE("FingerApiClose failed.");
    }
    return ret;
}

int FingerApiTest() {
    // 发送检测状态命令
    uint8_t test_connection_cmd[512];
    memset(test_connection_cmd, 0, 512);

    test_connection_cmd[0] = 0xF0;
    test_connection_cmd[1] = 0x6A;

    test_connection_cmd[2] = 0xA6;
    test_connection_cmd[3] = 0x01;
    test_connection_cmd[4] = 0x01;

    test_connection_cmd[7] = cal_xor_checkcode(test_connection_cmd + 1, 6);
    test_connection_cmd[8] = cal_sum_checkcode(test_connection_cmd + 1, 8);

    int ret = cmdSwap(test_connection_cmd, 512);
    if ( ret == 0){
        LOGD("FingerApiTest success.");
    } else {
        LOGE("FingerApiTest failed.");
    }
    return ret;
}

//从指纹仪读取上一次采集的图像到上位机
static unsigned char *send_obtain_finger_img_cmd() {
    int ret = -1;
    uint8_t version_cmd[512];
    memset(version_cmd, 0, 512);

    version_cmd[0] = 0xF0;
    version_cmd[1] = 0x6A;

    version_cmd[2] = 0xB2;
    version_cmd[3] = 0x01;
    version_cmd[4] = 0x01;

    version_cmd[6] = 0x01;

    version_cmd[7] = cal_xor_checkcode(version_cmd + 1, 6);
    version_cmd[8] = 0x00;//包序号，从0开始，读181次，每次508字节
    version_cmd[9] = cal_sum_checkcode(version_cmd + 1, 9);
   // LOGD("send_obtain_finger_img_cmd");

    ret = cmdSwap(version_cmd, 512);
    if (ret != 0) {
        LOGE("send_obtain_finger_img_cmd failed ret:%d.", ret);
        return NULL;
    }

    int size;
    uint8_t recv_cmd[512];   //打开指纹仪 响应包
    unsigned char *fingerBuffer = (unsigned char *) malloc(92160);
    if (fingerBuffer != NULL) {
        memset(fingerBuffer, 0, 92160);
    }

    for (int i = 0; i < 182; i++) {
        memset(recv_cmd, 0, 512);
        ret = libusb_bulk_transfer(dev_handle, BULK_RECV_EP, recv_cmd, 512, &size, 1000);
        if (ret == 0) {
            // LOGD("recv cmd sucess, length: %d bytes. \n", size);
            printfarray(recv_cmd, 13);
            if (recv_cmd[0] == 0xf0 && recv_cmd[1] == 1 && recv_cmd[2] == 0xfc &&
                recv_cmd[3] == 0x02) {
                memcpy(fingerBuffer + i * 508, recv_cmd + 4, 508);
            }
            if (recv_cmd[0] == 0xf0 && recv_cmd[1] == 0 && recv_cmd[2] == 0xd4 &&
                recv_cmd[3] == 0x01) {
                //最后一包只有212个字节
                memcpy(fingerBuffer + i * 508, recv_cmd + 4, 212);
                //LOGD("gather finger image success:%d:", i);
                return fingerBuffer;
            }
        } else {
            LOGE("recv cmd faild, err: %s, line:%d.\n", libusb_error_name(ret), __LINE__);
            return NULL;
        }
    }
}

unsigned char *FingerApiGatherRawFinger() {
    // 扫描一张原始图像（不做任何图像增强，暂存在指纹仪内）
    int ret = -1;
    uint8_t scan_cmd[512];
    memset(scan_cmd, 0, 512);

    scan_cmd[0] = 0xF0;
    scan_cmd[1] = 0x6A;

    scan_cmd[2] = 0xB2;
    scan_cmd[3] = 0x00;
    scan_cmd[4] = 0x04;

    scan_cmd[7] = cal_xor_checkcode(scan_cmd + 1, 6);
    scan_cmd[8] = cal_sum_checkcode(scan_cmd + 1, 8);

    ret = cmdSwap(scan_cmd, 512);
    if (ret == 0) {
        //LOGD("scan finger image success.");
        unsigned char *fingerBuffer = send_obtain_finger_img_cmd();
        return fingerBuffer;
    } else {
        LOGE("gather finger image failed.");
        return NULL;
    }
}


int FingerApiCalibration() {
    uint8_t calibration_cmd[512];
    memset(calibration_cmd, 0, 512);

    calibration_cmd[0] = 0xF0;
    calibration_cmd[1] = 0x6A;
    calibration_cmd[2] = 0xA6;

    calibration_cmd[4] = 0x03;

    calibration_cmd[7] = cal_xor_checkcode(calibration_cmd + 1, 6);
    calibration_cmd[8] = cal_sum_checkcode(calibration_cmd + 1, 8);

    int ret = cmdSwap(calibration_cmd, 512);
    if ( ret == 0){
        LOGD("FingerApiCalibration success.");
    } else {
        LOGE("FingerApiCalibration failed.");
    }
    return ret;
}

int FingerDownloadImage(unsigned char *imageBuffer) {
    int ret = -1;
    int size;
    uint8_t downloadBuffer[512];
    memset(downloadBuffer, 0, 512);

    downloadBuffer[0] = 0xF0;
    downloadBuffer[1] = 0x6A;

    downloadBuffer[2] = 0xC3;
    downloadBuffer[3] = 0x00;
    downloadBuffer[4] = 0x0C;

    uint16_t dataLen = 503;
    downloadBuffer[5] = dataLen >> 8;
    downloadBuffer[6] = dataLen & 0xff;
    downloadBuffer[7] = cal_xor_checkcode(downloadBuffer + 1, 6);


    for (int i = 0; i < 183; i++) {
        memset(downloadBuffer + 8, 0, 504);

        memcpy(downloadBuffer + 8, imageBuffer + i * 503, 503);
        downloadBuffer[511] = cal_sum_checkcode(downloadBuffer + 1, 511);
        ret = cmdSwap(downloadBuffer, 512);
        if ( ret == 0){
            //LOGD("FingerDownloadImage %d success.", i);
        } else {
            LOGE("FingerDownloadImage %d failed.", i);
            return -1;
        }
    }

    dataLen = 111;
    downloadBuffer[5] = dataLen >> 8;
    downloadBuffer[6] = dataLen & 0xff;
    downloadBuffer[7] = cal_xor_checkcode(downloadBuffer + 1, 6);
    memset(downloadBuffer + 8, 0, 504);


    memcpy(downloadBuffer + 8, imageBuffer + 183 * 503, 111);//最后一个包
    downloadBuffer[119] = cal_sum_checkcode(downloadBuffer + 1, 119);
    ret = cmdSwap(downloadBuffer, 512);
    if ( ret == 0){
        //LOGD("FingerDownloadImage last success.");
        return 0;
    } else {
        LOGE("FingerDownloadImage last failed.");
        return -1;
    }
}


int FingerDownloadFeature0(unsigned char *featureBuffer, int featureLength) {
    int ret = -1;
    int size;
    uint8_t downloadBuffer[512];
    memset(downloadBuffer, 0, 512);

    downloadBuffer[0] = 0xF0;
    downloadBuffer[1] = 0x6A;

    downloadBuffer[2] = 0xC3;
    downloadBuffer[3] = 0x00;
    downloadBuffer[4] = 0x0D;

    uint16_t dataLen = 503;
    downloadBuffer[5] = dataLen >> 8;
    downloadBuffer[6] = dataLen & 0xff;

    downloadBuffer[7] = cal_xor_checkcode(downloadBuffer + 1, 6);

    int packNum = featureLength / 503;
    int tailNum = featureLength - packNum * 503; //最后一个包的长度
    for (int i = 0; i < packNum; i++) {
        memset(downloadBuffer + 8, 0, 504);
        memcpy(downloadBuffer + 8, featureBuffer + i * 503, 503);
        downloadBuffer[511] = cal_sum_checkcode(downloadBuffer + 1, 511);

        ret = cmdSwap(downloadBuffer, 512);
        if ( ret == 0){
            LOGD("FingerDownloadFeature0 feature0 success.");
        } else {
            LOGE("FingerDownloadFeature0 feature0 failed.");
            return -1;
        }
    }

    memset(downloadBuffer, 0, 512);

    downloadBuffer[0] = 0xF0;
    downloadBuffer[1] = 0x6A;

    downloadBuffer[2] = 0xC3;
    downloadBuffer[3] = 0x00;
    downloadBuffer[4] = 0x0D;

    dataLen = tailNum;
    downloadBuffer[5] = dataLen >> 8;
    downloadBuffer[6] = dataLen & 0xff;

    downloadBuffer[7] = cal_xor_checkcode(downloadBuffer + 1, 6);

    memcpy(downloadBuffer + 8, featureBuffer + packNum * 503, tailNum);//最后一个包
    downloadBuffer[8+tailNum] = cal_sum_checkcode(downloadBuffer + 1, 8+tailNum);

    ret = cmdSwap(downloadBuffer, 512);
    if ( ret == 0){
        LOGD("FingerDownloadFeature0 last feature0 success.");
    } else {
        LOGE("FingerDownloadFeature0 last feature0 failed.");
    }
    return ret;
}

int FingerDownloadFeature1(unsigned char *featureBuffer) {
    int ret = -1;
    int size;
    uint8_t downloadBuffer[512];
    memset(downloadBuffer, 0, 512);

    downloadBuffer[0] = 0xF0;
    downloadBuffer[1] = 0x6A;

    downloadBuffer[2] = 0xC3;
    downloadBuffer[3] = 0x00;
    downloadBuffer[4] = 0x0E;
    uint16_t  dataLen = 503;
    downloadBuffer[5] = dataLen >> 8;
    downloadBuffer[6] = dataLen & 0xff;

    downloadBuffer[7] = cal_xor_checkcode(downloadBuffer + 1, 6);


    memset(downloadBuffer + 8, 0, 504);
    memcpy(downloadBuffer + 8, featureBuffer, 503);
    downloadBuffer[511] = cal_sum_checkcode(downloadBuffer + 1, 511);

    ret = cmdSwap(downloadBuffer, 512);
    if ( ret == 0){
        LOGD("FingerDownloadFeature1 feature1 success.");
    } else {
        LOGE("FingerDownloadFeature1 feature1 failed.");
        return -1;
    }

    memset(downloadBuffer + 8, 0, 504);
    memcpy(downloadBuffer + 8, featureBuffer + 503, 9);//最后一个包
    downloadBuffer[17] = cal_sum_checkcode(downloadBuffer + 1, 17);
    ret = cmdSwap(downloadBuffer, 512);
    if ( ret == 0){
        LOGD("FingerDownloadFeature1 last feature1 success.");
    } else {
        LOGE("FingerDownloadFeature1 last feature1 failed.");
    }
    return ret;
}

int FingerFeatureMatch(unsigned char *match_result) {
    int ret = -1;
    int size;
    uint8_t feature_match_cmd[512];
    memset(feature_match_cmd, 0, 512);

    feature_match_cmd[0] = 0xF0;
    feature_match_cmd[1] = 0x6A;

    feature_match_cmd[2] = 0xC3;
    feature_match_cmd[3] = 0x00;
    feature_match_cmd[4] = 0x04;

    feature_match_cmd[7] = cal_xor_checkcode(feature_match_cmd + 1, 6);
    feature_match_cmd[8] = cal_sum_checkcode(feature_match_cmd + 1, 8);

    uint8_t recvBuff[512];
    memset(recvBuff, 0, 512);

    ret = cmdSwap(feature_match_cmd, 512, recvBuff, &size);
    if ( ret == 0){
        LOGD("FingerFeatureMatch success.");
        match_result[0] = recvBuff[8];
    } else {
        LOGE("FingerFeatureMatch failed.");
    }
    return ret;
}

int FingerImageMatch(unsigned char *match_result) {
    int ret = -1;
    int size;
    uint8_t image_match_cmd[512];
    memset(image_match_cmd, 0, 512);

    image_match_cmd[0] = 0xF0;
    image_match_cmd[1] = 0x6A;

    image_match_cmd[2] = 0xC3;
    image_match_cmd[3] = 0x00;
    image_match_cmd[4] = 0x04;

    image_match_cmd[7] = cal_xor_checkcode(image_match_cmd + 1, 6);
    image_match_cmd[8] = cal_sum_checkcode(image_match_cmd + 1, 8);

    uint8_t recvBuff[512];
    memset(recvBuff, 0, 512);
    ret = cmdSwap(image_match_cmd, 512, recvBuff, &size);
    if ( ret == 0){
        LOGD("FingerImageMatch success.");
        match_result[0] = recvBuff[8];
        match_result[1] = recvBuff[9];
    } else {
        LOGE("FingerImageMatch failed.");
    }
    return ret;
}

int FingerApiVersion() {
    return 0;
}