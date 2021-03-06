package org.sec.passd.android.util;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.List;

import org.sec.passd.android.AR;
import org.sec.passd.android.R;
import org.sec.passd.android.mouse.PassDService;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageManager;
import android.preference.PreferenceManager;
import android.util.Log;


public class Util {
	private static final boolean D = false;
	private final static String LOG = "Util";

	public static String getIP(Context context) {
		// TODO Auto-generated method stub
		SharedPreferences pref = context.getSharedPreferences("prof", 0);
		return pref.getString( "ip", "" );
	}

	public static void setIP(Context context, String password) {
		Editor edit = context.getSharedPreferences("prof", 0).edit();
		edit.putString( "ip", password );
		edit.commit();
	}
	
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
	} //[��ó] �ȵ���̵� ����̽��� IP ���ϱ�|�ۼ��� ����

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

	public static int ByteToInt(byte [] data){
		int result = 0;
		for(int i=0; i<data.length; i++){
			if(data[i] == ' ') {
				continue;
			}
			result = result * 10 + (data[i]-'0');
		}
		return result;
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

	public static class Services{
		/**
		 * Check PassDService is running or not.
		 * @param context Application/Activity's context
		 * @return <code>true</code> if PassDService is running, <code>false</code> otherwise.
		 */
		public static boolean isServiceAliveU(Context context){
			String serviceCls = PassDService.class.getName();
			ActivityManager manager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);
			List<RunningServiceInfo> serviceList = manager.getRunningServices(Integer.MAX_VALUE);
			for(RunningServiceInfo info : serviceList){
				if(info.service.getClassName().equals(serviceCls)){
					return true;
				}
			}
			if(D) Log.d(LOG, "PassDService not available.");
			return false;
		}

		/**
		 * Starts PassDService.
		 * @param context Application/Activity's context
		 * @see org.sec.passd.android.mouse.PassDService PassDService
		 */
		public static ComponentName startPassDService(Context context, String ip, int port){
			if(!isServiceAliveU(context)){
				if(D) Log.d(LOG, "Starting PassDService..");
				Intent intent = new Intent(context, PassDService.class);
				intent.putExtra("ip", ip);
				intent.putExtra("port", port);
				return context.startService(intent);
			} else {
				return null;
			}
		}
	}
	
	public static String getSelectedCursorName(Context context) {
		SharedPreferences defaultSharedPref = PreferenceManager.getDefaultSharedPreferences(context);
		return defaultSharedPref.getString("mouseicon", "null");
	}
	
	public static int getSelectedCursorResource(Context context) {
		SharedPreferences defaultSharedPref = PreferenceManager.getDefaultSharedPreferences(context);
		return getCursorResource(defaultSharedPref.getString("mouseicon", "null"));
	}
	
	public static int getCursorResource(String name) {
		if("Basic Cursor".equals(name)) {
			return R.drawable.cursor_basic;
		} else if("Black Cursor".equals(name)) {
			return R.drawable.cursor_black;
		} else if("Blue Cursor".equals(name)) {
			return R.drawable.cursor_blue;
		} else if("Leaf".equals(name)) {
			return R.drawable.cursor_leaf;
		} else if("Sword".equals(name)) {
			return R.drawable.cursor_sword;
		} else if("Finger".equals(name)) {
			return R.drawable.cursor_finger;
		} else {
			return R.drawable.cursor_basic;
		}
	}
}
