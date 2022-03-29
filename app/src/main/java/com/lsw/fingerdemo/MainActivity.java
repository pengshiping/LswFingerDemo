package com.lsw.fingerdemo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
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
                if (LswFingerApi.lswFingerApiOpen() == 0) {
                    setTextInfo("指纹模组打开成功.");
                } else {
                    setTextInfo("指纹模组打开失败.");
                }
            }
        });
        binding.btnStopGather.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mGatherThreadIsRun = false;
            }
        });
        binding.btnCal.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (LswFingerApi.lswFingerApiCalibration() == 0) {
                    setTextInfo("指纹模组通讯校验成功.");
                } else {
                    setTextInfo("指纹模组通讯校验失败.");
                }
            }
        });

        binding.btnVersion.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                LswFingerApi.lswFingerApiVersion();
            }
        });
        binding.btnTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (LswFingerApi.lswFingerApiTest() == 0) {
                    setTextInfo("指纹模组通讯测试成功，可以采集指纹.");
                } else {
                    setTextInfo("指纹模组通讯测试失败.");
                }
            }
        });
        binding.btnClose.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (LswFingerApi.lswFingerApiClose() == 0) {
                    setTextInfo("指纹模组已经关闭");
                }
            }
        });
        binding.btnGather.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startGatherRawThread();
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


    private void startGatherRawThread() {
        mGatherThreadIsRun = true;
        setTextInfo("开始采集指纹.");
        new Thread(new Runnable() {
            @Override
            public void run() {
                while (mGatherThreadIsRun) {
                    byte[] rawFinger = LswFingerApi.lswFingerApiGatherRawFinger();
                    if (rawFinger != null) {
                        //Log.i(TAG, "lswFingerApiGatherRawFinger success");
                    } else {
                        //Log.e(TAG, "lswFingerApiGatherRawFinger failed regather.");
                        continue;
                    }
                    String bmpfilepath = Environment.getExternalStorageDirectory().getAbsolutePath();
                    //Log.e(TAG, "" + bmpfilepath);
                    if (rawFinger.length > 0) {
                        String filename = bmpfilepath + "/a.bmp";
                        androidbmp mandroidbmp = new androidbmp();
                        File mFile = new File(filename);
                        if (mFile.exists()) {
                            mFile.delete();
                        }
                        try {
                            mandroidbmp.save_bmp(filename, rawFinger);

                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        Bitmap bm = BitmapFactory.decodeFile(filename);
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
                setTextInfo("停止采集指纹.");
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

    @Override
    protected void onDestroy() {
        super.onDestroy();
        LswFingerApi.lswFingerApiClose();
        System.exit(0);
    }

}