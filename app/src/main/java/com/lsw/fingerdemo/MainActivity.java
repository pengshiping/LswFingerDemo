package com.lsw.fingerdemo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.app.NotificationManagerCompat;

import android.Manifest;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.lsw.fingerdemo.databinding.ActivityMainBinding;
import java.io.File;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {


    private ActivityMainBinding binding;
    private UsbApiManager mUsbApiManager = null;
    private Context mContext;
    private boolean mGatherThreadIsRun = false;
    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private final String TAG = "MainActivity";
    private static String[] PERMISSIONS_STORAGE = {"android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE"};


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        initClickListener();
        verifyPermission();
        initUsbManager();
    }

    private void initClickListener() {
        binding.btnOpen.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (UsbApiManager.bOpen) {
                    if (LswFingerApi.lswFingerApiOpen() == 0) {
                        setTextInfo("指纹模组打开成功.");
                    } else {
                        setTextInfo("指纹模组打开失败.");
                    }
                } else {
                    setTextInfo("未识别到指纹模组.");
                }
            }
        });
        binding.btnStopGather.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mGatherThreadIsRun = false;
            }
        });
        binding.btnGather2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (UsbApiManager.bOpen) {
                    mGatherThreadIsRun = false;
                    gatherFinger();
                }
            }
        });

        binding.btnCal.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (UsbApiManager.bOpen) {
                    if (LswFingerApi.lswFingerApiCalibration() == 0) {
                        setTextInfo("指纹模组通讯校验成功.");
                    } else {
                        setTextInfo("指纹模组通讯校验失败.");
                    }
                }
//                if (UsbApiManager.bOpen) {
//                    //LswFingerApi.lswFingerApiVersion();
//                    //featureMatchTest();
//                    //imageMatchTest();
//
//                }
            }
        });
        binding.btnTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (UsbApiManager.bOpen) {
                    if (LswFingerApi.lswFingerApiTest() == 0) {
                        setTextInfo("指纹模组通讯测试成功，可以采集指纹.");
                    } else {
                        setTextInfo("指纹模组通讯测试失败.");
                    }
                }
            }
        });
        binding.btnClose.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (UsbApiManager.bOpen) {
                    if (LswFingerApi.lswFingerApiClose() == 0) {
                        setTextInfo("指纹模组已经关闭");
                    }
                }
            }
        });
        binding.btnGather.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (UsbApiManager.bOpen) {
                    startGatherRawThread();
                }
            }
        });
    }

    private void setTextInfo(String text) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                String info = binding.textView1.getText() + "\n" + text;
                binding.textView1.setText(info);
            }
        });
    }

    private void gatherFinger() {
        byte[] rawFinger = LswFingerApi.lswFingerApiGatherRawFinger();
        if (rawFinger == null) {
            Log.i(TAG, "lswFingerApiGatherRawFinger failed");
            setTextInfo("采集失败");
            return;
        }
        if (rawFinger.length > 0) {
            androidbmp mandroidbmp = new androidbmp();
            Bitmap bm = mandroidbmp.decodeBitmap(rawFinger);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    binding.fpImage.setImageBitmap(bm);
                }
            });
        } else {
            Log.e(TAG, "can not get pic!!!");
        }
    }


    private void startGatherRawThread() {
        mGatherThreadIsRun = true;
        setTextInfo("开始采集指纹.");
        new Thread(new Runnable() {
            @Override
            public void run() {
                while (mGatherThreadIsRun) {
                    gatherFinger();
                }
                setTextInfo("停止连续采集指纹.");
                Log.i(TAG, "gather thread is stop.");
                try {
                    Thread.sleep(1000);
                }catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    private void initUsbManager() {
        mContext = getApplicationContext();
        mUsbApiManager = mUsbApiManager.getInstance(mContext);
        int ret = mUsbApiManager.UsbOpenDevice();
    }

    private void verifyPermission() {
        try {
            int permission = ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);
            if (permission != PackageManager.PERMISSION_GRANTED) {
                // We don't have permission so prompt the user
                ActivityCompat.requestPermissions(
                        MainActivity.this,
                        PERMISSIONS_STORAGE,
                        REQUEST_EXTERNAL_STORAGE);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void featureMatchTest() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                byte[] feature0 = FingerUtils.getFeature0();
                byte[] feature1 = FingerUtils.getFeature1();
                int ret = LswFingerApi.lswFingerDownloadFeature0(feature0, 512);
                Log.i(TAG, "lswFingerDownloadFeature0 ret:" + ret);
                ret = LswFingerApi.lswFingerDownloadFeature1(feature1);
                Log.i(TAG, "lswFingerDownloadFeature1 ret:" + ret);

                byte[] score = LswFingerApi.lswFingerFeatureMatch();
                if (score != null) {
                    Log.i(TAG, "featureMatchTest score:" + score[0]);
                } else {
                    Log.e(TAG, "featureMatchTest failed.");
                }
            }
        }).start();


//        byte[] image = FingerUtils.getFingerImage();
//        LswFingerApi.lswFingerDownloadImage(image);
//        byte[] score = LswFingerApi.lswFingerImageMatch();
//        if (score != null) {
//            Log.i(TAG, "imageMatchTest score:" + score[0] + "," + score[1]);
//        } else {
//            Log.e(TAG, "imageMatchTest failed.");
//        }

    }

    private void imageMatchTest() {
        byte[] feature0 = FingerUtils.getFeature0();
        byte[] image = FingerUtils.getFingerImage();

        byte[] mirror = new byte[256 * 360];
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 360; j++) {
                mirror[j * 256 + 255 - i] = image[j * 256 + i];
            }
        }

        LswFingerApi.lswFingerDownloadFeature0(feature0, 1024);
        LswFingerApi.lswFingerDownloadImage(mirror);
        byte[] score = LswFingerApi.lswFingerImageMatch();
        if (score != null) {
            Log.i(TAG, "imageMatchTest score:" + score[0] + "," + score[1]);
        } else {
            Log.e(TAG, "featureMatchTest failed.");
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mGatherThreadIsRun = false;
        LswFingerApi.lswFingerApiClose();
        System.exit(0);
    }

}