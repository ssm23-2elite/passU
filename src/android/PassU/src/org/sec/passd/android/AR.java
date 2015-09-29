package org.sec.passd.android;

import org.sec.passd.android.mouse.PassDService;

import android.app.Application;
import android.content.Context;
import android.os.Build;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

/***
 * Role : Manage Service as Singleton
 * @author Jake Yoon
 */
public class AR extends Application {
	private final String LOG = "AR";
	
	private static 		AR					m_Instance;
	public				PassDService		m_Service = null;
	
	public static int width;
	public static int height;
	public static String phone_lcd_size;
	public static String phone_model;
	public static String phone_os_vrsn;
	
	public AR() {
		super();
		m_Instance = this;
	}
	
	@Override
	public void onCreate()
	{
		if(D.D) Log.w(LOG, "onCreate");
		super.onCreate();		
		Display dp = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
		width = dp.getWidth();
		height = dp.getHeight();
		phone_lcd_size = AR.width + "*" + AR.height;
		phone_model = Build.MODEL;
		phone_os_vrsn = Build.VERSION.RELEASE;
	}
	
	// Double-checked singleton fetching
	public static AR getInstance() {
		if(m_Instance == null) {
			synchronized(AR.class) {
				if(m_Instance == null) new AR();
			}
		}
		return m_Instance;
	}	
}

