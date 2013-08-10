package org.ssm232elite.passu.android;

import org.ssm232elite.passu.android.setting.PassUSetting;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

import com.google.android.gcm.GCMRegistrar;

public class PassU extends Activity{
	private final String SENDER_ID = "96418752374";
	private Button btn_connect;
	private Button btn_setting;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//>> GCM 등록
		try {
			GCMRegistrar.checkDevice(this);
			GCMRegistrar.checkManifest(this);
			final String regId = GCMRegistrar.getRegistrationId(this);

			if (regId.equals("")) {
				GCMRegistrar.register(this, SENDER_ID);
			} 
		} catch ( Exception e ) {
			Toast.makeText( this, "GCM Error", Toast.LENGTH_SHORT).show();
		}

		setContentView(R.layout.layout_main);
		
		btn_connect = (Button)findViewById(R.id.btn_connect);
		btn_setting = (Button)findViewById(R.id.btn_setting);
		
		btn_connect.setOnClickListener( new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Toast.makeText(PassU.this, "접속을 시도합니다.",  Toast.LENGTH_SHORT).show();
			}
		});
		
		btn_setting.setOnClickListener( new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				startActivity(new Intent(PassU.this, PassUSetting.class));
			}
		});
	}
}
