package com.lsw.fingerdemo;

public class LswFingerApi {

    static {
        System.loadLibrary("lswfinger");
    }

    public static native int lswFingerApiInit(int fd);

    public static native int lswFingerApiOpen();

    public static native int lswFingerApiClose();

    public static native int lswFingerApiTest();

    public static native byte[] lswFingerApiGatherRawFinger();

    public static native int lswFingerApiCalibration();

    public static native int lswFingerApiVersion();

    //以下是指纹比对
    public static native int lswFingerDownloadImage(byte[] imageBuffer);

    public static native int lswFingerDownloadFeature0(byte[] featureBuffer, int featureLength);

    public static native int lswFingerDownloadFeature1(byte[] featureBuffer);

    public static native byte[] lswFingerFeatureMatch();

    public static native byte[] lswFingerImageMatch();



}
