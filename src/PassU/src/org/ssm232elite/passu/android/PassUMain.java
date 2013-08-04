package org.ssm232elite.passu.android;

import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;

public class PassUMain extends PreferenceActivity{
	Preference mFirst;
	CheckBoxPreference mSecond;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.xml.layout_main);
	}
}
