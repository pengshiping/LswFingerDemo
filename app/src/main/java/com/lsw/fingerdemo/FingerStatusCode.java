package com.lsw.fingerdemo;



import android.util.Log;


public class FingerStatusCode
{

	public int StatusCode;

	public static final int STATUS_OK	=		0x0000;		// 成功
	public static final int STATUS_ERR_XOR		=		0x8F01;		// XOR校验错误
	public static final int STATUS_ERR_SUM		=		0x8F02;		// SUM校验错误
	public static final int STATUS_ERR_INS		=		0x8F03;		// 指令错误
	public static final int STATUS_ERR_PARA		=		0x8F04;		// 参数错误
	public static final int STATUS_ERR_INSTIMEOUT	=	0x8F05;		// 通信超时
	public static final int STATUS_ERR_EXPARA		=	0x8F06;		// 扩展域参数错误

	public static final int STATUS_ERR_OPEN			=	0x8101;		// 打开模组失败
	public static final int STATUS_ERR_CLOSE		=	0x8102;		// 关闭模组失败
	public static final int STATUS_ERR_GRAP 		=	0x8201;		// 图像采集失败
	public static final int STATUS_ERR_UPLOAD 		=	0x8301;		// 上传图像失败
	public static final int STATUS_ERR_VERIFY 		=	0x8401;		// 指纹对比失败
	public static final int STATUS_ERR_DOWNLOAD		=	0x8501;		// 模板下载失败
	public static final int STATUS_ERR_TRANSMIT		=	0x8601;		// 模板上传失败

	public static final int STATUS_ERR_ADJUST 		=	0x8CCC;		// 传感器校准失败

	public static final int STATUS_ERR_BOOT 		=	0x80FE;		// BOOT TO COS切换失败
	public static final int STATUS_FAILED			=	-1;			// 操作失败
	public static final int STATUS_ERR_HANDLE		=	0x0001;		// 设备错误
	public static final int STATUS_ERR_TIMEOUT		=	0x0002;		// 超时
	public static final int STATUS_ERR_PAK			=	0x0003;		// 错误的协议包
	
	public static final int  STATUS_USB_ERROR_ACCESS  =	0x0004;		//USB 访问被拒绝（没有足够的权限)
	public static final int  STATUS_USB_ERROR_OPENED  =	0x0006;		//USB 连接被拒绝（USB重复建立连接）
	public static final int  STATUS_ERR_OTHER  =	0x0005;			//其他意外错误
	

	public void setCode(int StatusCode)
	{
		this.StatusCode = StatusCode;
	}

	public int getCode()
	{
		return this.StatusCode;
	}

	public static String getMessage(int StatusCode)
	{
		Log.d("getMessagecode", ""+StatusCode);
		
	//	String result = (new StringBuilder("fp process info(")).append(StatusCode).append("): ").toString();
		String result = "";
		switch (StatusCode)
		{
		case STATUS_OK:		// 成功
			result = (new StringBuilder(String.valueOf(result))).append("操作成功").toString();
			break;
		case STATUS_FAILED:		// 失败
			result = (new StringBuilder(String.valueOf(result))).append("操作失败").toString();
			break;
		case STATUS_ERR_XOR:		// XOR校验错误
			result = (new StringBuilder(String.valueOf(result))).append("校验值1错误").toString();
			break;
		case STATUS_ERR_SUM:		// SUM校验错误
			result = (new StringBuilder(String.valueOf(result))).append("校验值2错误").toString();
			break;
		case STATUS_ERR_INS:		// 指令错误
			result = (new StringBuilder(String.valueOf(result))).append("命令错误（不支持的命令）").toString();
			break;
		case STATUS_ERR_PARA:		// 参数错误
			result = (new StringBuilder(String.valueOf(result))).append("命令包参数错误").toString();
			break;
		case STATUS_ERR_INSTIMEOUT:		// 通信超时
			result = (new StringBuilder(String.valueOf(result))).append("传感器通信超时").toString();
			break;
		case STATUS_ERR_EXPARA:		// 扩展域参数错误
			result = (new StringBuilder(String.valueOf(result))).append("扩展域参数错误").toString();
			break;
		case STATUS_ERR_OPEN:		// 打开模组失败
			result = (new StringBuilder(String.valueOf(result))).append("模组初始化失败").toString();
			break;
		case STATUS_ERR_CLOSE:		// 关闭模组失败
			result = (new StringBuilder(String.valueOf(result))).append("模组功能关闭失败").toString();
			break;
		case STATUS_ERR_GRAP:		// 图像采集失败
			result = (new StringBuilder(String.valueOf(result))).append("图像采集失败").toString();
			break;
		case STATUS_ERR_UPLOAD:		// 上传图像失败
			result = (new StringBuilder(String.valueOf(result))).append("上传图像失败").toString();
			break;
		case STATUS_ERR_VERIFY:		// 指纹对比失败
			result = (new StringBuilder(String.valueOf(result))).append("指纹对比失败").toString();
			break;
		case STATUS_ERR_DOWNLOAD:		// 模板下载失败
			result = (new StringBuilder(String.valueOf(result))).append("模板下载失败").toString();
			break;
		case STATUS_ERR_TRANSMIT:		// 模板上传失败
			result = (new StringBuilder(String.valueOf(result))).append("模板上传失败").toString();
			break;
		case STATUS_ERR_ADJUST:		// 传感器校准失败
			result = (new StringBuilder(String.valueOf(result))).append("传感器校准失败").toString();
			break;
		case STATUS_ERR_BOOT:		// BOOT TO COS切换失败
			result = (new StringBuilder(String.valueOf(result))).append("BOOT TO COS切换失败").toString();
			break;
		case STATUS_ERR_HANDLE:		// 设备错误
			result = (new StringBuilder(String.valueOf(result))).append("设备错误（设备可能已被移除）").toString();
			break;
		case STATUS_ERR_TIMEOUT:		// 超时
			result = (new StringBuilder(String.valueOf(result))).append("命令执行超时").toString();
			break;
		case STATUS_ERR_PAK:		// 错误的协议包
			result = (new StringBuilder(String.valueOf(result))).append("错误的数据响应包").toString();
			break;
		case STATUS_USB_ERROR_ACCESS: 
			result = (new StringBuilder(String.valueOf(result))).append("USB 访问被拒绝（没有足够的权限）").toString();
			break;
		case STATUS_USB_ERROR_OPENED: 
			result = (new StringBuilder(String.valueOf(result))).append("USB 连接被拒绝（USB重复建立连接）").toString();
			break;

		case STATUS_ERR_OTHER: 
			result = (new StringBuilder(String.valueOf(result))).append("其他意外错误").toString();
			break;			
		default:
			result = (new StringBuilder(String.valueOf(result))).append("未知错误").toString();
			break;
		}
		return result;
	}
	
}
