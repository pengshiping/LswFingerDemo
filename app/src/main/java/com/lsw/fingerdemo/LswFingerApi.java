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

    public static native byte[] lswFingerApiGatherDelBgFinger();

    public static native int lswFingerApiCalibration();

    public static native int lswFingerApiVersion();



}
