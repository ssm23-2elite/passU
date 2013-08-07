package org.ssm232elite.passu.android.setting;

import org.ssm232elite.passu.android.R;

import android.os.Bundle;
import android.preference.PreferenceActivity;

public class PassUSetting extends PreferenceActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.xml.menu_setting);
		setContentView(R.layout.layout_setting);
	}
}
