package com.lsw.fingerdemo;

import android.os.Environment;

import java.io.File;
import java.io.FileInputStream;

public class FingerUtils {

    private static String filepath = Environment.getExternalStorageDirectory().getAbsolutePath();

    public static byte[] getFeature0() {
        try {
            FileInputStream fin = new FileInputStream(new File(filepath + "/imagedata/sfz.data"));
            byte[] buffer = new byte[1024];
            int length = fin.read(buffer, 0, 1024);
            return buffer;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static byte[] getFeature1() {
        try {
            FileInputStream fin = new FileInputStream(new File(filepath + "/imagedata/img2.data"));
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
            FileInputStream fin = new FileInputStream(new File(filepath + "/imagedata/img1.data"));
            byte[] buffer = new byte[92160];
            int length = fin.read(buffer);
            return buffer;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }


}
