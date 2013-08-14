package org.secmem232.passu.android.util;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

import org.secmem232.passu.android.AR;

import android.content.Context;
import android.content.pm.PackageManager;
import android.util.Log;


public class CommonUtil {

	private final static String LOG = "CommonUtil";

	public static String getLocalIpAddress()
	{
		try {
			for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();) {
				NetworkInterface intf = en.nextElement();
				for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();) {
					InetAddress inetAddress = enumIpAddr.nextElement();
					if (!inetAddress.isLoopbackAddress()) {
						return inetAddress.getHostAddress().toString();
					}
				}
			}
		} catch (SocketException e) {
			Log.e(LOG, "get LocalIpAddress Exception:" + e.toString());
		}
		return null;
	} //[출처] 안드로이드 디바이스의 IP 구하기|작성자 지피

	//get Version Code from packageManager
	public static int getVersion(Context context) {
		try {
			PackageManager pm = context.getPackageManager();
			return pm.getPackageInfo(AR.class.getPackage().getName(), 0).versionCode;
		} catch(Exception e) {
			return 1;
		}
	}
	
	//get Version name from packageManager
	public static String getVersionName(Context context) {
		try {
			PackageManager pm = context.getPackageManager();
			return pm.getPackageInfo(AR.class.getPackage().getName(), 0).versionName;
		} catch(Exception e) {
			return "1.0";
		}
	}
	
	public static int itoa(int iSrc, byte[] buffer){
		int positionalNumber = getPositionalNumber(iSrc);
		int length = positionalNumber;
				
		int i=0;
		while(positionalNumber > 0){
			int jesu = (int)Math.pow(10, positionalNumber-1);
			int quotiont = iSrc / jesu;
			
			buffer[i] = (byte) (quotiont+'0');
			
			int remainder = iSrc % jesu;		
			
			positionalNumber--;
			i++;
			iSrc = remainder;
		}
		return length;
	}
	
	public static int itoa(int iSrc, byte[] buffer, int offset){
		int positionalNumber = getPositionalNumber(iSrc);
		int length = positionalNumber;
				
		int i=offset;
		while(positionalNumber > 0){
			int jesu = (int)Math.pow(10, positionalNumber-1);
			int quotiont = iSrc / jesu;
			
			buffer[i] = (byte) (quotiont+'0');
			
			int remainder = iSrc % jesu;		
			
			positionalNumber--;
			i++;
			iSrc = remainder;
		}
		return length;
	}
	
	public static int itoa(int iSrc, byte[] buffer, int positionalNumber, int offset){
						
		int i=offset;
		while(positionalNumber > 0){
			int jesu = (int)Math.pow(10, positionalNumber-1);
			int quotiont = iSrc / jesu;
			
			buffer[i] = (byte) (quotiont+'0');
			
			int remainder = iSrc % jesu;		
			
			positionalNumber--;
			i++;
			iSrc = remainder;
		}
		return positionalNumber;
	}
	
	public static int getPositionalNumber(int iSrc){
		int positionnalNumber=1;
		while(true){
			if(iSrc/(int)Math.pow(10, positionnalNumber) == 0)
				break;
			positionnalNumber++;
		}
		return positionnalNumber;
	}
	
}
