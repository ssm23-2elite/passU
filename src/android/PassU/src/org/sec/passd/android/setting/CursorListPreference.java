package org.sec.passd.android.setting;

import java.util.ArrayList;
import java.util.List;

import org.sec.passd.android.R;
import org.sec.passd.android.util.Util;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.preference.ListPreference;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class CursorListPreference extends ListPreference {
	private LayoutInflater inflater;
	
	public CursorListPreference(final Context context) {
		this(context, null);
		this.inflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	}

	public CursorListPreference(final Context context, final AttributeSet attrs) {
		super(context, attrs);
		this.inflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	}

	public void onPrepareDialogBuilder(AlertDialog.Builder builder){
        builder.setAdapter(new CursorListAdapter(getContext(),R.layout.layout_cursorlistitem,
        		getEntriesList()), new OnClickListener(){
            public void onClick(DialogInterface dialog, int which) {
            }
        });
        super.onPrepareDialogBuilder(builder);
    }
	
	private ArrayList<String> getEntriesList(){
        ArrayList<String> list = new ArrayList<String>();
        CharSequence[] array = getEntries();
        for(CharSequence name : array){
            list.add(name.toString());
        } 
        return list;
    }

	private class CursorListAdapter extends ArrayAdapter<String>{
		public CursorListAdapter(Context context, int resource, List<String> objects) {
			super(context, resource, objects);
		}
		public View getView(int position, View convertView, ViewGroup parent){
			ViewHolder holder;
			if( convertView == null ) {
				convertView = inflater.inflate(R.layout.layout_cursorlistitem, parent, false);

				holder = new ViewHolder();
				holder.name = (TextView)convertView.findViewById(R.id.name);
				holder.cursor = (ImageView)convertView.findViewById(R.id.cursor);
				
				convertView.setTag(holder);
			} else {
				holder = (ViewHolder) convertView.getTag();
			}

			holder.name.setText(getItem(position));
			holder.cursor.setImageResource(Util.getCursorResource(getItem(position)));
			return convertView;
		}
	}

	private class ViewHolder {
		public ImageView cursor;
		public TextView name;
	}
}