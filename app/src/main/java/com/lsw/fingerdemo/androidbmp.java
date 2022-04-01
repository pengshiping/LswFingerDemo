package com.lsw.fingerdemo;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import java.io.FileOutputStream;
import java.io.IOException;

class androidbmp {
    public static int dataNUM=92456;
    public  byte[] bmphead=new byte[]{//tagBITMAPFILEHEADER文件头：14字节
                0x42, 0x4d,//flag：BM
                0x36, 0x6c, 0x01, 0x00,
                0x00, 0x00, 0x00, 0x00,//reserved
                0x36, 0x04, 0x00, 0x00,//offset:1078

                //tagBITMAPFILEHEADER描述位图属性：40字节
                0x28, 0x00, 0x00, 0x00,//size:40
                0x00, 0x01, 0x00, 0x00,//width:256
                0x68, 0x01, 0x00, 0x00,//height:360
                0x01, 0x00,//planes:must be 1
                0x08, 0x00,//bitcout:8,256灰阶即1字节8bit
                0x00, 0x00, 0x00, 0x00,//compression
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,//xpels
                0x00, 0x00, 0x00, 0x00,//yperls
                0x00, 0x00, 0x00, 0x00,//colorUsed
                0x00, 0x00, 0x00, 0x00//colorImportant};
    };
    public byte[] colorinfor=new byte[1024];


    androidbmp(){
        priv_BmpColorsInit();
    }

    public  void priv_BmpColorsInit()
    {
        int i;
        for (i = 0; i < 256; i++)
        {
            colorinfor[i * 4] = colorinfor[i * 4 + 1] = colorinfor[i * 4 + 2] = (byte)i;
            colorinfor[i * 4 + 3] = 0;
        }
    }

    public int save_bmp(String filePath,byte[] bufferdata) throws IOException {
        FileOutputStream fos = new FileOutputStream(filePath);
        fos.write(bmphead);
        fos.write(colorinfor);
        fos.write(bufferdata,0,256*360);
        fos.close();
        return 0;
    }

    public Bitmap decodeBitmap(byte[] bufferdata) {
        byte[] bitmapBuffer = new byte[bmphead.length + colorinfor.length + 256*360];

        System.arraycopy(bmphead, 0, bitmapBuffer, 0, bmphead.length);
        System.arraycopy(colorinfor, 0, bitmapBuffer, bmphead.length, colorinfor.length);
        System.arraycopy(bufferdata, 0, bitmapBuffer, bmphead.length + colorinfor.length, bufferdata.length);

        Bitmap bm = BitmapFactory.decodeByteArray(bitmapBuffer,0, 256*360);
        return bm;
    }

}