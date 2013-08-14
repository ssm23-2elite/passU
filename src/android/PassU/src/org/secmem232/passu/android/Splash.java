package org.secmem232.passu.android;

import org.secmem232.passu.android.driver.InstallDriverListener;
import org.secmem232.passu.android.driver.InstallDriverTask;

import android.app.Activity;
import android.widget.Toast;

import com.googlecode.androidannotations.annotations.AfterViews;
import com.googlecode.androidannotations.annotations.EActivity;

/**
 * Intro Activity for initialization
 * Role : Checking Rooting => Install Busybox
 * @author Jake Yoon
 */

@EActivity(R.layout.layout_splash)
public class Splash extends Activity implements InstallDriverListener {
	
    @AfterViews
	void onInitialize() {
		//onSucceed();
        
		InstallDriverTask install = new InstallDriverTask(this, this);
		install.execute();
    }

    @Override
    public void onSucceed() {
    	PassU_.intent(this).start();
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