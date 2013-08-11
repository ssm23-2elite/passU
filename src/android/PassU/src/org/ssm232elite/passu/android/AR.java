package org.ssm232elite.passu.android;

import android.app.Application;
import android.content.Context;
import android.os.Build;
import android.view.Display;
import android.view.WindowManager;

/**
 * 어플 시동시 제일먼저 시동되는 클래스
 * 디바이스관련된 정보를 가지고 있음
 * 
 * @author Jake Yoon
 */

public class AR extends Application {

	public static int width;
	public static int height;
	public static String phon_lcd_size;
	public static String phon_model;
	public static String phon_os_vrsn;
	@Override
	public void onCreate() {
		super.onCreate();

		Display dp = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
		width = dp.getWidth();
		height = dp.getHeight();
		phon_lcd_size = AR.width + "*" + AR.height;
		phon_model = Build.MODEL;
		phon_os_vrsn = Build.VERSION.RELEASE;
	}
}
