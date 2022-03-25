package com.lsw.fingerdemo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.lsw.fingerdemo.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {


    private ActivityMainBinding binding;

    private UsbApiManager mUsbApiManager = null;
    private Context mContext;

    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {"android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE" };

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
                } else {
                    Toast.makeText(MainActivity.this, "指纹模组打开失败.", Toast.LENGTH_LONG).show();
                }
            }
        });
        binding.btnGather.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                LswFingerApi.lswFingerApiGatherRawFinger();
            }
        });
        binding.btnGather2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                LswFingerApi.lswFingerApiGatherFingerDelBg();
            }
        });
        binding.btnCal.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (LswFingerApi.lswFingerApiCalibration() == 0) {
                    Toast.makeText(MainActivity.this, "指纹模组通讯校验成功.", Toast.LENGTH_LONG).show();
                } else {
                    Toast.makeText(MainActivity.this, "指纹模组通讯校验失败.", Toast.LENGTH_LONG).show();
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
                    Toast.makeText(MainActivity.this, "指纹模组通讯测试成功，可以采集指纹.", Toast.LENGTH_LONG).show();
                } else {
                    Toast.makeText(MainActivity.this, "指纹模组通讯测试失败.", Toast.LENGTH_LONG).show();
                }
            }
        });
        binding.btnClose.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                finish();
            }
        });
        binding.btnGather.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                byte[] rawFinger = LswFingerApi.lswFingerApiGatherRawFinger();
            }
        });
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
        }
        catch (Exception e){
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