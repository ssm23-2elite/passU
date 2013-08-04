package org.ssm232elite.passu.android;

import org.ssm232elite.passu.android.driver.InstallDriverListener;
import org.ssm232elite.passu.android.driver.InstallDriverTask;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.Toast;

/**
 * Intro 무조건 실행됨
 * 역할 : Checking Rooting => Install Busybox
 * @author Jake Yoon
 */
public class Intro extends Activity implements InstallDriverListener {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.layout_intro);
        
		InstallDriverTask install = new InstallDriverTask(this, this);
		install.execute();
    }
    
    private void amove() {
    	try {
			// 1초 대기
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    	startActivity(new Intent(Intro.this, PassUMain.class));
		onDestroy();
		finish();
	}
    
    @Override
    public void onSucceed() {
    	// TODO Auto-generated method stub
    	Toast.makeText(this, "SUCCESS!! ", Toast.LENGTH_LONG).show();
    	amove();
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