package com.lsw.fingerdemo;

import android.os.Environment;

import java.io.File;
import java.io.FileInputStream;

public class FingerUtils {

    private static String filepath = Environment.getExternalStorageDirectory().getAbsolutePath();

    public static byte[] getFeature0() {
        try {
            FileInputStream fin = new FileInputStream(new File(filepath + "/fingerdata/left.feat"));
            byte[] buffer = new byte[512];
            int length = fin.read(buffer, 0, 512);
            return buffer;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static byte[] getFeature1() {
        try {
            FileInputStream fin = new FileInputStream(new File(filepath + "/fingerdata/right.feat"));
            byte[] buffer = new byte[512];
            int length = fin.read(buffer, 0, 512);
            return buffer;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static byte[] getFingerImage() {
        try {
            FileInputStream fin = new FileInputStream(new File(filepath + "/fingerdata/image.dat"));
            byte[] buffer = new byte[92160];
            int length = fin.read(buffer);
            return buffer;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }


}
