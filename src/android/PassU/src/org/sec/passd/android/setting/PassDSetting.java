package org.sec.passd.android.setting;

import org.sec.passd.android.R;
import org.sec.passd.android.util.Util;

import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceActivity;

public class PassDSetting extends PreferenceActivity {

	private ListPreference preferenceView;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.xml.menu_setting);
		
		ListPreference listPreference = (ListPreference) findPreference("mouseicon");
		String mouseicon = Util.getSelectedCursorName(this);
        if( "null".equals(mouseicon) || mouseicon == null ) {
            listPreference.setValueIndex(0);
        } else {
        	listPreference.setValue(mouseicon);
        }
        listPreference.setSummary(listPreference.getValue().toString());
        
        listPreference.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
            	preference.setSummary(newValue.toString());
                return true;
            }
        });
		setContentView(R.layout.layout_setting);
	}
}
