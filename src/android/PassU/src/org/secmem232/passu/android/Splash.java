package org.secmem232.passu.android;

import org.secmem232.passu.android.driver.InstallDriverListener;
import org.secmem232.passu.android.driver.InstallDriverTask;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.Toast;

/**
 * Intro Activity for initialization
 * Role : Checking Rooting => Install Busybox
 * @author Jake Yoon
 */
public class Splash extends Activity implements InstallDriverListener {
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.layout_splash);
	//	onSucceed();
		InstallDriverTask install = new InstallDriverTask(this, this);
		install.execute();
	}
    
    @Override
    public void onBackPressed() {
    	android.os.Process.killProcess(android.os.Process.myPid());
    	super.onBackPressed();
    }
    @Override
    public void onSucceed() {
    	startActivity(new Intent(Splash.this, PassU.class));
    	onDestroy();
		finish();
    }
    
    @Override
    public void onError(int ERR_CODE) {
    	// TODO Auto-generated method stub
    	switch( ERR_CODE ) {
    	case InstallDriverTask.ERR_IO:
    		Toast.makeText(this, "IO ERROR", Toast.LENGTH_LONG).show();
    		break;
    	case InstallDriverTask.ERR_SECURITY:
    		Toast.makeText(this, "SECURITY ERROR", Toast.LENGTH_LONG).show();
    		break;
    	case InstallDriverTask.ERR_UNKNOWN:
    		Toast.makeText(this, "UNKNOWN ERROR", Toast.LENGTH_LONG).show();
    		break;
    	}
    	onDestroy();
		finish();
    }
}