package org.secmem232.passu.android.setting;

import java.util.ArrayList;
import java.util.List;

import org.secmem232.passu.android.D;

import android.R;
import android.R.string;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.preference.ListPreference;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;


public class CursorListPreference extends ListPreference {
	 
	private final String LOG = "CursorListPreference";
	
    public CursorListPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
    }
    /* 
    public void onPrepareDialogBuilder(AlertDialog.Builder builder){
        super.onPrepareDialogBuilder(builder);
        builder.setAdapter(
        		new CursorListAdapter( getContext(), R.layout.layout_listpreference, getEntriesList()), new OnClickListener(){
            public void onClick(DialogInterface dialog, int which) {
                setValue(getEntryValues()[which].toString());
                dialog.dismiss();
            }
        });
    }
     

    private ArrayList<Cursor> getEntriesList(){
        ArrayList<Cursor> list = new ArrayList<Cursor>();
        Cursor[] array = getEntries();
         
        for(Cursor cursor : array){
            list.add(cursor);
        }
         
        return list;
    }
     
    private class CursorListAdapter extends ArrayAdapter<Cursor>{
         
        public CursorListAdapter(Context context, int resource, List<Cursor> objects) {
            super(context, resource, objects);
        }
         
        public View getView(int position, View view, ViewGroup parent){
            LayoutInflater vi = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            String name = (String) Cur.this.getEntries()[position];
            String value = (String) ColorListPreference.this.getEntryValues()[position];
             
            view = vi.inflate(R.layout.color_list_preference_row, null);
             
            View colorView = view.findViewById(R.id.color);
            TextView nameView = (TextView) view.findViewById(R.id.name);
            Log.i("yViewer",value);
            colorView.setBackgroundColor(rgb.toRGB(value));
            nameView.setText(name);
            nameView.setTextColor(rgb.toRGB(value));
             
            return view;
        }
    }*/
}