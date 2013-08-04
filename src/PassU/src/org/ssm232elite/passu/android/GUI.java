package org.ssm232elite.passu.android;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class GUI extends Activity {

	RelativeLayout m_panel;
	
	int m_nScreenW = 0, m_nScreenH = 0;
	boolean m_bRunning = false;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        
        m_panel  = new RelativeLayout(this);
        m_panel.setBackgroundResource(R.drawable.back);
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
				Log.d("GUI", "Start clicked!");
				// show cursor
				cmdShowCursor();
				
				// start a thread to move cursor arround
				m_bRunning = true;
				/*
				Thread t = new Thread() {
					int last_x = 0, last_y = 0;
					public void run() 
				        {		
							while (m_bRunning) {
								Random rnd = new Random();
								int new_x = rnd.nextInt(m_nScreenW), new_y = rnd.nextInt(m_nScreenH);
								//new_x = 100; new_y = 10;
								int ix = last_x, iy = last_y;
								while ((ix!=new_x || iy!=new_y) && m_bRunning) {
									//Log.e("XX", ""+ix+":"+iy+"  "+new_x+":"+new_y);
									if (new_x > ix) ix++; else if (new_x < ix) ix --;
									if (new_y > iy) iy++; else if (new_y < iy) iy --;
								
									Singleton.getInstance().m_CurService.Update(ix, iy, true);
									try {
										Thread.sleep(5);
									} catch (InterruptedException e) {
										e.printStackTrace();
									}

								}
								last_x = new_x; last_y = new_y;
							}
							
				        }
				};
				t.start();*/
				
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

		m_panel.setOnTouchListener(new OnTouchListener() {
			
			@Override
			public boolean onTouch(View v, MotionEvent ev) {
				// TODO Auto-generated method stub
				return false;
			}
		});
		
		
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
        i.setAction("org.ssm232elite.passu.android.CursorService");
        startService(i);
	}
	
	private void cmdTurnCursorServiceOff() {
		Intent i = new Intent();
        i.setAction("org.ssm232elite.passu.android.CursorService");
        stopService(i);
	}
	
	private void cmdShowCursor() {
		if (Singleton.getInstance().m_CurService != null)
			Singleton.getInstance().m_CurService.ShowCursor(true);
	}
	
	private void cmdHideCursor() {
		if (Singleton.getInstance().m_CurService != null)
			Singleton.getInstance().m_CurService.ShowCursor(false);
	}
    
}
/*
 * java.lang.RuntimeException: Unable to create service net.pocketmagic.android.mycursoroverlay.CursorService: 
 * android.view.WindowManager$BadTokenException: Unable to add window android.view.ViewRoot$W@464d9bf8 -- 
 * permission denied for this window type
 * needs: 	<uses-permission android:name="android.permission.SYSTEM_ALERT_WINDOW" />
 */
 
