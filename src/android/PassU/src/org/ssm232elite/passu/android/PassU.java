package org.ssm232elite.passu.android;

import org.ssm232elite.passu.android.mouse.GUI;

import android.content.Intent;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.util.Log;
import android.widget.Toast;

import com.google.android.gcm.GCMRegistrar;

public class PassU extends PreferenceActivity{
	private final String SENDER_ID = "96418752374";
	private Preference mFirst;
	private CheckBoxPreference mSecond;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//>> GCM 등록
		try {
			GCMRegistrar.checkDevice(this);
			GCMRegistrar.checkManifest(this);
			final String regId = GCMRegistrar.getRegistrationId(this);

			Log.w("LOG", regId);

			if (regId.equals("")) {

				GCMRegistrar.register(this, SENDER_ID);
				Toast.makeText( this, "등록성공", Toast.LENGTH_SHORT).show();

			} else {

				Log.v("TAG", "Already registered");

			}
		} catch ( Exception e ) {
			Toast.makeText( this, "GCM Error", Toast.LENGTH_SHORT).show();
		}

		addPreferencesFromResource(R.xml.layout_main);
		
		startActivity(new Intent(PassU.this, GUI.class));
	}
}
