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

#define BULK_RECV_EP    0x81
#define BULK_SEND_EP    0x01

static int image_number = 0;

// 指纹图像
#define  IMAGElENGTH 92160
unsigned char finger_img_buffer[IMAGElENGTH];//256 * 360 = 92160字节

libusb_device_handle *dev_handle;

unsigned char cal_xor_checkcode(unsigned char *pBuf, int nLen)
{
    int i;
    unsigned char ret = 0;
    for (i = 0; i < nLen; i++)
    {
        ret ^= pBuf[i];
    }
    return ret;
}

unsigned char cal_sum_checkcode(unsigned char *pBuf, int nLen)
{
    int i;
    unsigned char ret = 0;
    for (i = 0; i < nLen; i++)
    {
        ret += pBuf[i];
    }
    ret = ~ret;
    return ret;
}

void printfarray(unsigned char* array, int length)
{
    char printstr[512];
    memset(printstr, 0, 512);
    sprintf(printstr, "[%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x]", array[0],array[1], array[2],array[3],
            array[4],array[5], array[6],array[7], array[8],array[9], array[10],array[11], array[12]);\
    LOGD("%s", printstr);
}

//采用bulk端点发送数据
static int bulk_send(unsigned char *buf, int num)
{
    int size;
    int rec;
    // LOGD("bulk send length: %d\n", num);
    rec = libusb_bulk_transfer(dev_handle, BULK_SEND_EP, buf, num, &size, 10);
    // LOGD("libusb_bulk_transfer return: %d\n", rec);
    if(rec == 0) {
        // LOGD("libusb_bulk_transfer send sucess,length: %d bytes\n", size);
    }
    else{
        LOGD("libusb_bulk_transfer send faild, err: %s\n", libusb_error_name(rec));
    }
    return rec;
}

static void print_endpoint_comp(const struct libusb_ss_endpoint_companion_descriptor *ep_comp)
{
    LOGD("      USB 3.0 Endpoint Companion:\n");
    LOGD("        bMaxBurst:           %u\n", ep_comp->bMaxBurst);
    LOGD("        bmAttributes:        %02xh\n", ep_comp->bmAttributes);
    LOGD("        wBytesPerInterval:   %u\n", ep_comp->wBytesPerInterval);
}

static void print_endpoint(const struct libusb_endpoint_descriptor *endpoint)
{
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

static void print_altsetting(const struct libusb_interface_descriptor *interface)
{
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

static void print_2_0_ext_cap(struct libusb_usb_2_0_extension_descriptor *usb_2_0_ext_cap)
{
    LOGD("    USB 2.0 Extension Capabilities:\n");
    LOGD("      bDevCapabilityType:    %u\n", usb_2_0_ext_cap->bDevCapabilityType);
    LOGD("      bmAttributes:          %08xh\n", usb_2_0_ext_cap->bmAttributes);
}

static void print_ss_usb_cap(struct libusb_ss_usb_device_capability_descriptor *ss_usb_cap)
{
    LOGD("    USB 3.0 Capabilities:\n");
    LOGD("      bDevCapabilityType:    %u\n", ss_usb_cap->bDevCapabilityType);
    LOGD("      bmAttributes:          %02xh\n", ss_usb_cap->bmAttributes);
    LOGD("      wSpeedSupported:       %u\n", ss_usb_cap->wSpeedSupported);
    LOGD("      bFunctionalitySupport: %u\n", ss_usb_cap->bFunctionalitySupport);
    LOGD("      bU1devExitLat:         %u\n", ss_usb_cap->bU1DevExitLat);
    LOGD("      bU2devExitLat:         %u\n", ss_usb_cap->bU2DevExitLat);
}

static void print_bos(libusb_device_handle *handle)
{
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

static void print_interface(const struct libusb_interface *interface)
{
    int i;

    for (i = 0; i < interface->num_altsetting; i++)
        print_altsetting(&interface->altsetting[i]);
}

static void print_configuration(struct libusb_config_descriptor *config)
{
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

static void print_device(libusb_device *dev, libusb_device_handle *handle)
{
    LOGD("print_device");

    struct libusb_device_descriptor desc;
    unsigned char string[256];
    const char *speed;
    int ret;
    int verbose;
    uint8_t i;

    switch (libusb_get_device_speed(dev)) {
        case LIBUSB_SPEED_LOW:		speed = "1.5M"; break;
        case LIBUSB_SPEED_FULL:		speed = "12M"; break;
        case LIBUSB_SPEED_HIGH:		speed = "480M"; break;
        case LIBUSB_SPEED_SUPER:	speed = "5G"; break;
        case LIBUSB_SPEED_SUPER_PLUS:	speed = "10G"; break;
        default:			speed = "Unknown";
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
            ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, string, sizeof(string));
            if (ret > 0)
                LOGD("  Manufacturer:              %s\n", (char *)string);
        }

        if (desc.iProduct) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, string, sizeof(string));
            if (ret > 0)
                LOGD("  Product:                   %s\n", (char *)string);
        }

        if (desc.iSerialNumber && verbose) {
            ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, string, sizeof(string));
            if (ret > 0)
                LOGD("  Serial Number:             %s\n", (char *)string);
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

int FingerApiInit(int fd)
{
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
    r = libusb_wrap_sys_device(ctx, (intptr_t)fd, &dev_handle);
    if (r < 0) {
        LOGD("libusb_wrap_sys_device failed: %d\n", r);
        return r;
    } else if (dev_handle == NULL) {
        LOGD("libusb_wrap_sys_device returned invalid handle\n");
        return r;
    }

    print_device(libusb_get_device(dev_handle), dev_handle);

    r = libusb_kernel_driver_active(dev_handle, 0);
    if(r == 0)  //ok
    {
        LOGD("libusb_kernel_driver_active is ok.");
    }
    else if(r == 1)
    {
        LOGD("Kernel driver is active, now try detached\n");
        if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
            LOGD("Kernel driver is detached!\n");
        else{
            LOGD("libusb_detach_kernel_driver, err:%s\n", libusb_error_name(r));
            return -1;
        }
    }
    else{
        LOGD("libusb_kernel_driver_active, err:%s\n", libusb_error_name(r));
        return -1;
    }

    r = libusb_claim_interface(dev_handle, 0); //claim interface 0 ,stm32采用接口0
    if(r < 0) {
        LOGD("cannot claim interface, err:%s\n", libusb_error_name(r));
        return -1;
    } else {
        LOGD("libusb_claim_interface success.\n");
    }

    return 0;
}

int FingerApiOpen()
{
    int ret = -1;
    int size;
    uint8_t open_cmd[512];
    memset(open_cmd, 0, 512);

    open_cmd[0] = 0xF0;
    open_cmd[1] = 0x6A;

    open_cmd[2] = 0xA6;
    open_cmd[4] = 0x01;
    open_cmd[6] = 0x04;

    open_cmd[7] = cal_xor_checkcode(open_cmd+1, 6); //0xC9;
    open_cmd[12] = cal_sum_checkcode(open_cmd+1, 12);//0x21;


    LOGD("send_open_cmd");
    printfarray(open_cmd, 13);

    ret = bulk_send(open_cmd, 512);
    if (ret == 0)
    {
        LOGD("send_open_cmd success.");
    }
    else
    {
        LOGE("send_open_cmd failed.");
        return -1;
    }
    uint8_t recv_cmd[512];
    memset(recv_cmd, 0, 512);
    ret = libusb_bulk_transfer(dev_handle, BULK_RECV_EP, recv_cmd, 512, &size, 2);
    if (ret == 0)
    {
        LOGD("recv cmd sucess, length: %d bytes. \n", size);
        printfarray(recv_cmd, 13);
        return 0;
    }
    else
    {
        LOGD("recv cmd faild, err: %s\n", libusb_error_name(ret));
    }
    return -1;
}

int FingerApiClose()
{
    LOGD("FingerApiClose");
    if (dev_handle)
    {
        libusb_close(dev_handle);
    }
    return 0;
}

int FingerApiTest() {
    // 发送检测状态命令
    int ret = -1;
    int size;
    uint8_t test_connection_cmd[512];
    memset(test_connection_cmd, 0, 512);

    test_connection_cmd[0] = 0xF0;
    test_connection_cmd[1] = 0x6A;

    test_connection_cmd[2] = 0xA6;
    test_connection_cmd[3] = 0x01;
    test_connection_cmd[4] = 0x01;

    test_connection_cmd[7] = cal_xor_checkcode(test_connection_cmd + 1, 6);
    test_connection_cmd[8] = cal_sum_checkcode(test_connection_cmd + 1, 8);

    LOGD("send_test_connection_cmd");
    printfarray(test_connection_cmd, 13);

    ret = bulk_send(test_connection_cmd, 512);
    if (ret == 0) {
        LOGD("send_test_connection_cmd success.");
    } else {
        LOGE("send_test_connection_cmd failed.");
        return -1;
    }


    uint8_t recv_cmd[512];
    memset(recv_cmd, 0, 512);
    ret = libusb_bulk_transfer(dev_handle, BULK_RECV_EP, recv_cmd, 512, &size, 2);
    if (ret == 0)
    {
        LOGD("recv cmd sucess, length: %d bytes. \n", size);
        printfarray(recv_cmd, 13);
        return 0;
    }
    else
    {
        LOGD("recv cmd faild, err: %s\n", libusb_error_name(ret));
    }
    return -1;
}

//从指纹仪读取上一次采集的图像到上位机
static int send_obtain_finger_img_cmd()
{
    int ret = -1;
    uint8_t version_cmd[512];
    memset(version_cmd, 0, 512);

    version_cmd[0] = 0xF0;
    version_cmd[1] = 0x6A;

    version_cmd[2] = 0xB2;
    version_cmd[3] = 0x01;
    version_cmd[4] = 0x01;

    version_cmd[6] = 0x01;

    version_cmd[7] = cal_xor_checkcode(version_cmd+1, 6);
    version_cmd[8] = 0x00;//包序号，从0开始，读181次，每次508字节
    version_cmd[9] = cal_sum_checkcode(version_cmd+1, 9);

    LOGD("send_obtain_finger_img_cmd");
    printfarray(version_cmd, 13);

    ret = bulk_send(version_cmd, 512);
    if (ret == 0)
    {
        LOGD("send_obtain_finger_img_cmd success.");
    }
    else
    {
        LOGE("send_obtain_finger_img_cmd failed.");
        return -1;
    }

    int size;
    int rec;
    uint8_t recv_cmd[512];   //打开指纹仪 响应包

    while(1)
    {
        memset(recv_cmd, 0, 512);
        rec = libusb_bulk_transfer(dev_handle, BULK_RECV_EP, recv_cmd, 512, &size, 2);
        if(rec == 0)
        {
            LOGD("recv cmd sucess, length: %d bytes. \n", size);
            printfarray(recv_cmd, 13);
            if (recv_cmd[0]== 0xf0 && recv_cmd[1]== 1 &&recv_cmd[2]== 0xfc &&recv_cmd[3]== 0x02)
            {
                memcpy(finger_img_buffer + image_number*508, recv_cmd+4, 508);
                image_number++;
                LOGD("image number:%d", image_number);
            }
            if (recv_cmd[0]== 0xf0 && recv_cmd[1]== 0 &&recv_cmd[2]== 0xd4 &&recv_cmd[3]== 0x01)
            {
                //最后一包只有212个字节
                memcpy(finger_img_buffer + image_number*508, recv_cmd+4, 212);
                image_number++;
                LOGE("the last image number:%d", image_number);
                image_number = 0;
                return 0;
            }
        }
        else
        {
            LOGD("recv cmd faild, err: %s\n", libusb_error_name(rec));
        }
        LOGD("**********************************************************************");

    }

}

unsigned char *FingerApiGatherRawFinger() {
    // 扫描一张原始图像（不做任何图像增强，暂存在指纹仪内）
    int ret = -1;
    int size = 0;
    uint8_t scan_cmd[512];
    memset(scan_cmd, 0, 512);

    scan_cmd[0] = 0xF0;
    scan_cmd[1] = 0x6A;

    scan_cmd[2] = 0xB2;
    scan_cmd[3] = 0x00;
    scan_cmd[4] = 0x04;

    scan_cmd[7] = cal_xor_checkcode(scan_cmd + 1, 6);
    scan_cmd[8] = cal_sum_checkcode(scan_cmd + 1, 8);

    LOGD("send_scan_finger_rawimg_cmd");
    printfarray(scan_cmd, 13);

    ret = bulk_send(scan_cmd, 512);
    if (ret == 0) {
        LOGD("send_scan_finger_rawimg_cmd success.");
    } else {
        LOGE("send_scan_finger_rawimg_cmd failed.");
        return NULL;
    }

    uint8_t recv_cmd[512];
    memset(recv_cmd, 0, 512);
    ret = libusb_bulk_transfer(dev_handle, BULK_RECV_EP, recv_cmd, 512, &size, 2);
    if (ret == 0)
    {
        LOGD("recv cmd sucess, length: %d bytes. \n", size);
        printfarray(recv_cmd, 13);
        ret = send_obtain_finger_img_cmd();
        if (ret == 0)
        {
            LOGD("send_obtain_finger_img_cmd success.");
        }
        else
        {
            LOGE("send_obtain_finger_img_cmd failed.");
        }
        return NULL;
    }
    else
    {
        LOGD("recv cmd faild, err: %s\n", libusb_error_name(ret));
    }
    return NULL;

}



int FingerApiGatherFingerDelBg()
{
    return 0;
}

int FingerApiCalibration()
{
    int ret = -1;
    int size;
    uint8_t calibration_cmd[512];
    memset(calibration_cmd, 0, 512);

    calibration_cmd[0] = 0xF0;
    calibration_cmd[1] = 0x6A;
    calibration_cmd[2] = 0xA6;

    calibration_cmd[4] = 0x03;

    calibration_cmd[7] = cal_xor_checkcode(calibration_cmd+1, 6);
    calibration_cmd[8] = cal_sum_checkcode(calibration_cmd+1, 8);

    LOGD("send_calibration_cmd");
    printfarray(calibration_cmd, 13);

    ret = bulk_send(calibration_cmd, 512);
    if (ret == 0)
    {
        LOGD("send_calibration_cmd success.");
    }
    else
    {
        LOGE("send_calibration_cmd failed.");
        return -1;
    }

    uint8_t recv_cmd[512];
    memset(recv_cmd, 0, 512);
    ret = libusb_bulk_transfer(dev_handle, BULK_RECV_EP, recv_cmd, 512, &size, 2);
    if (ret == 0)
    {
        LOGD("recv cmd sucess, length: %d bytes. \n", size);
        printfarray(recv_cmd, 13);
        return 0;
    }
    else
    {
        LOGD("recv cmd faild, err: %s\n", libusb_error_name(ret));
    }
    return -1;
}

int FingerApiVersion()
{
    return 0;
}