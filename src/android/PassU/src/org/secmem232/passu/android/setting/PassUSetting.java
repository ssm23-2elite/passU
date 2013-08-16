package org.secmem232.passu.android.setting;

import org.secmem232.passu.android.R;

import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceActivity;

public class PassUSetting extends PreferenceActivity {

	private ListPreference preferenceView;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.xml.menu_setting);
		setContentView(R.layout.layout_setting);
	}
}
