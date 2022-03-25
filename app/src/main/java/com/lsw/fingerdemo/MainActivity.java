package com.lsw.fingerdemo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

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
                LswFingerApi.lswFingerApiOpen();
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
                LswFingerApi.lswFingerApiCalibration();
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
                LswFingerApi.lswFingerApiTest();
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
    }

}