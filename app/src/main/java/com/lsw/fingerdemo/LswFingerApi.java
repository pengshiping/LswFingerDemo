package com.lsw.fingerdemo;

public class LswFingerApi {

    static {
        System.loadLibrary("lswfinger");
    }

    public static native int lswFingerApiInit();

    public static native int lswFingerApiOpen();

    public static native int lswFingerApiClose();

    public static native int lswFingerApiTest();

    public static native int lswFingerApiGatherRawFinger();

    public static native int lswFingerApiGatherFingerDelBg();

    public static native int lswFingerApiCalibration();

    public static native int lswFingerApiVersion();



}
