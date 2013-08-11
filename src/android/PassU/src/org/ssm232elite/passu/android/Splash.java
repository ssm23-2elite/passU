package org.ssm232elite.passu.android;

import org.ssm232elite.passu.android.driver.InstallDriverListener;
import org.ssm232elite.passu.android.driver.InstallDriverTask;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Toast;

import com.googlecode.androidannotations.annotations.EActivity;

/**
 * Intro 무조건 실행됨
 * 역할 : Checking Rooting => Install Busybox
 * @author Jake Yoon
 */
@EActivity
public class Splash extends Activity implements InstallDriverListener {
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.layout_splash);
        
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