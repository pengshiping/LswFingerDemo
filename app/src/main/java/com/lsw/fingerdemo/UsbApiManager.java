package com.lsw.fingerdemo;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbRequest;
import android.os.Build;
import android.util.Log;
import android.widget.ArrayAdapter;

import androidx.annotation.RequiresApi;

import java.util.ArrayList;
import java.util.List;

public class UsbApiManager {
    private static PendingIntent intent;

    private enum UsbPermission {Unknown,Requested,Granted,Denied};
    private static UsbManager usbManager = null;
    private UsbDevice mUsbDevice = null;
    private UsbDeviceConnection mDeviceConnection  = null;
    private Context mContext;
    public static boolean bOpen = false;
    private static int mVendorId = 11551;
    private static int mProductId = 5;
    private UsbPermission usbPermissionA = UsbPermission.Unknown;
    private BroadcastReceiver broadcastReceiver;
    private List<String> chipList = new ArrayList<>();
    private List<String> baudList = new ArrayList<>();
    private ArrayAdapter<String> adapterChip;
    private ArrayAdapter<String> adapterBaud;
    private UsbInterface interfaceA = null;
    private UsbEndpoint epOut = null;
    private UsbEndpoint epIn = null;
    private final int MAXPacketSize = 512;
    private byte[] writeBuff;
    private byte[] readBuff;
    private boolean bRefreshRead = false;
    private boolean bRefreshExit = false;
    private boolean bUsbReadExit = false;
    private boolean bUsbReadRunning = false;
    private final String strUsbReadRunning = "LOCKREAD";
    private UsbRequest okUsbReq = null;
    private boolean bUsbWriteOK = false;
    private static final String TAG = "UsbApi";
    public static int finger_width = 256;
    public static int finger_height = 360;
    private Activity mActivity;
    public String devName = null;
    public String interfaceName = null;
    public int epCount = 0;
    public int permit = 0;
    public static int fd = -1;
    public static final String ACTION_USB_PERMISSION = "com.android.usb.USB_PERMISSION";


    public static UsbApiManager getInstance(Context context){
        usbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);

        UsbApiManager result = new UsbApiManager();
        return result;
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public int UsbOpenDevice(){
        if(usbManager == null) return FingerStatusCode.STATUS_ERR_OPEN;
        if(mUsbDevice!=null)
            mUsbDevice = null;
        for(UsbDevice device:usbManager.getDeviceList().values()) {
            if (device.getVendorId() == mVendorId && device.getProductId() == mProductId) {
                //return FingerStatusCode.STATUS_OK;

                mUsbDevice = device;
                interfaceA = device.getInterface(0);
                int endpointCount = interfaceA.getEndpointCount();
                for (int j = 0; j < endpointCount; j++) {
                    UsbEndpoint endpoint = interfaceA.getEndpoint(j);
                    if (endpoint.getDirection() == UsbConstants.USB_DIR_OUT) {
                        epOut = endpoint;
                    } else {
                        epIn = endpoint;
                    }
                    if ((epOut != null) && (epIn != null)) {

                        devName = mUsbDevice.getDeviceName();
                        epCount = endpointCount;
                        interfaceName = interfaceA.getName();
                        //return FingerStatusCode.STATUS_OK;
                    }
                }


            }
        }
        if(mUsbDevice == null)
            return FingerStatusCode.STATUS_USB_ERROR_OPENED;

        if(usbManager.hasPermission(mUsbDevice)){
           // permit = 1;
           // usbPermissionA = UsbPermission.Granted;
            Log.i(TAG,"has permission.");
            mDeviceConnection = usbManager.openDevice(mUsbDevice);
           if(mDeviceConnection == null) {
               Log.e(TAG,"openDevice failed.");
               permit = 0;
                return FingerStatusCode.STATUS_ERR_HANDLE;
            }else{
               permit = 1;
               fd = mDeviceConnection.getFileDescriptor();
               Log.i(TAG,"openDevice success fd:" + fd);
               int ret = LswFingerApi.lswFingerApiInit(fd);
               if (ret == 0) {
                   bOpen = true;
               }
               return FingerStatusCode.STATUS_OK;
           }
           // mDeviceConnection.claimInterface(interfaceA,true);
        }else {
            Log.e(TAG,"no permission.");
            getUsbPermission(mUsbDevice);
            return FingerStatusCode.STATUS_USB_ERROR_ACCESS;
        }



    }

    public void getUsbPermission(UsbDevice mUSBDevice) {
        mContext = BaseApplication.getContextObject();
        PendingIntent pendingIntent = PendingIntent.getBroadcast(mContext, 1, new Intent(ACTION_USB_PERMISSION), 0);
        IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        mContext.registerReceiver(mUsbReceiver, filter);
        usbManager.requestPermission(mUSBDevice, pendingIntent);
    }

    public final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            bOpen = false;
            if (ACTION_USB_PERMISSION.equals(action)) {
                synchronized (this) {
                    mContext.unregisterReceiver(mUsbReceiver);
                    UsbDevice device = (UsbDevice) intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        Log.e(TAG, "user agree open usb device.");
                        mDeviceConnection = usbManager.openDevice(mUsbDevice);
                        if (mDeviceConnection != null) {
                            permit = 1;
                            fd = mDeviceConnection.getFileDescriptor();
                            Log.i(TAG, "fileDescriptor fd:" + fd);
                            int ret = LswFingerApi.lswFingerApiInit(fd);
                            if (ret == 0) {
                                bOpen = true;
                            }
                        } else {
                            Log.e(TAG, "UsbManager openDevice failed");
                            //return FingerStatusCode.STATUS_USB_ERROR_ACCESS;
                        }
                    }
                }
            }
        }
    };


    public void startStatusListener(Activity activity){
        mActivity = activity;
        Intent intent = activity.getIntent();
        String action =  intent.getAction();
        if (UsbManager.ACTION_USB_ACCESSORY_ATTACHED.equals(action)) {
            mUsbDevice = (UsbDevice) intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
            if((mVendorId == mUsbDevice.getVendorId()) && (mProductId == mUsbDevice.getProductId())){
                interfaceA = mUsbDevice.getInterface(0);
                int endpointCount = interfaceA.getEndpointCount();
                for(int j = 0;j<endpointCount;j++){
                    UsbEndpoint endpoint = interfaceA.getEndpoint(j);
                    if(endpoint.getDirection() == UsbConstants.USB_DIR_OUT){
                        epOut = endpoint;
                    }else{
                        epIn = endpoint;
                    }
                    if((epOut != null)&&(epIn != null)){
                        bOpen = true;
                        break;
                    }
                }
            }
        }
        if(UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)){
            mUsbDevice = (UsbDevice) intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
            if(mVendorId == mUsbDevice.getVendorId() && mProductId == mUsbDevice.getProductId()){
                mUsbDevice = null;
                bOpen = false;
            }
        }

    }


    public void updateChipList(){
        int nDID = 0;
        int nVID = 0;
        int nPID = 0;
        //usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        Log.i(TAG,"updateChipList");

    }
}

