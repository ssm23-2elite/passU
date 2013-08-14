package org.secmem232.passu.android.mouse;

import org.secmem232.passu.android.AR;
import org.secmem232.passu.android.R;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

/* @project 
 * 
 * License to access, copy or distribute this file.
 * This file or any portions of it, is Copyright (C) 2012, Radu Motisan ,  http://www.pocketmagic.net . All rights reserved.
 * @author Radu Motisan, radu.motisan@gmail.com
 * 
 * This file is protected by copyright law and international treaties. Unauthorized access, reproduction 
 * or distribution of this file or any portions of it may result in severe civil and criminal penalties.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * 
 * @purpose 
 * Cursor Overlay Sample
 * (C) 2012 Radu Motisan , all rights reserved.
 */

public class GUI extends Activity {

	RelativeLayout m_panel;
	
	int m_nScreenW = 0, m_nScreenH = 0;
	boolean m_bRunning = false;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        
        m_panel  = new RelativeLayout(this);
        m_panel.setBackgroundResource(R.drawable.back_default);
        setContentView(m_panel);
        
        // build a minimalistic interface
        LinearLayout panelV = new LinearLayout(this);
        panelV.setOrientation(LinearLayout.VERTICAL);
        RelativeLayout.LayoutParams lp_iv = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
		lp_iv.addRule(RelativeLayout.CENTER_IN_PARENT);
		m_panel.addView(panelV, lp_iv);
		
        
        Button bStart = new Button(this);
        bStart.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				if (m_bRunning) return;
				
				cmdShowCursor();
			}
		});
        bStart.setText("START");
        panelV.addView(bStart);
        
        Button bStop = new Button(this);
        bStop.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				Log.d("GUI", "Stop clicked!");
				cmdHideCursor();
				m_bRunning = false;
			}
		});
        bStop.setText("STOP");
        panelV.addView(bStop);
        
        
        Button bHide = new Button(this);
        bHide.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				Log.d("GUI", "Hide clicked!");
				Intent newActivity = new Intent(Intent.ACTION_MAIN); 
				newActivity.addCategory(Intent.CATEGORY_HOME);
				newActivity.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				startActivity(newActivity);
			}
		});
        bHide.setText("HIDE");
        panelV.addView(bHide);
        
        TextView tv = new TextView(this);
        tv.setTextColor(Color.BLACK);
        tv.setText("Press Start then Hide to see the cursor move");
        panelV.addView(tv);
        
        // get screen size
        DisplayMetrics metrics = new DisplayMetrics();
		try {
			WindowManager winMgr = (WindowManager)getSystemService(Context.WINDOW_SERVICE) ;
	       	winMgr.getDefaultDisplay().getMetrics(metrics);
	       	m_nScreenW = winMgr.getDefaultDisplay().getWidth();
	       	m_nScreenH = winMgr.getDefaultDisplay().getHeight();
		}
		catch (Exception e) { //default to a HVGA 320x480 and let's hope for the best
			e.printStackTrace();
			m_nScreenW = 0;
			m_nScreenH = 0;
		} 

        // start cursor service
		cmdTurnCursorServiceOn();

    }
    
    /* onDestroy - called when the app is closed */
	@Override public void onDestroy() {
		super.onDestroy();
		Log.d("GUI", "onDestroy called");
		m_bRunning = false;
		// close our cursor service
		cmdTurnCursorServiceOff();
	}
	
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_gui, menu);
        return true;
    }
    
    /*
     * 	HELPER CURSOR FUNCTIONS
     */
	private void cmdTurnCursorServiceOn() {	
       	Intent i = new Intent();
        i.setAction("org.ssm232elite.passu.android.mouse.PassUService");
        startService(i);
        
        Log.i("cmdTurnCursorServiceOn", "cmdTurnCursorServiceOn");
	}
	
	private void cmdTurnCursorServiceOff() {
		Intent i = new Intent();
        i.setAction("org.ssm232elite.passu.android.mouse.PassUService");
        stopService(i);
	}
	
	private void cmdShowCursor() {
		if (AR.getInstance().m_CurService != null)
			AR.getInstance().m_CurService.ShowCursor();
	}
	
	private void cmdHideCursor() {
		if (AR.getInstance().m_CurService != null)
			AR.getInstance().m_CurService.HideCursor();
	}
    
}
/*
 * java.lang.RuntimeException: Unable to create service net.pocketmagic.android.mycursoroverlay.CursorService: 
 * android.view.WindowManager$BadTokenException: Unable to add window android.view.ViewRoot$W@464d9bf8 -- 
 * permission denied for this window type
 * needs: 	<uses-permission android:name="android.permission.SYSTEM_ALERT_WINDOW" />
 */
 
