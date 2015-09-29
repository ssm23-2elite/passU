package org.sec.passd.android;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;

/**
 * Intro Activity for initialization
 * Role : Checking Rooting => Install Busybox
 * @author Jake Yoon
 */
public class Splash extends Activity {
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.layout_splash);
		
		new Handler().postDelayed(new Runnable() {
			@Override
			public void run() {
				startActivity(new Intent(Splash.this, PassD.class));
		    	onDestroy();
				finish();
			}
		}, 1000);
	}
    
    @Override
    public void onBackPressed() {
    	android.os.Process.killProcess(android.os.Process.myPid());
    	super.onBackPressed();
    }
}