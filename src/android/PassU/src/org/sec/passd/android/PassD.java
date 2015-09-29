package org.sec.passd.android;

import org.sec.passd.android.intent.PassDIntent;
import org.sec.passd.android.mouse.PassDService;
import org.sec.passd.android.setting.PassDSetting;
import org.sec.passd.android.util.Util;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class PassD extends Activity {
	private final String LOG = "PassD";
	private Button btn_connect;
	private EditText edit_ip;
	private Button btn_setting;
	private String ip;
	private IPassD mPassDSvc;
	private ServiceConnection conn = new ServiceConnection() {
		
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			if(D.D) Log.w(LOG, "onServiceConnected");
			ip = edit_ip.getText().toString();
			
			mPassDSvc = IPassD.Stub.asInterface(service);
			try {
				// Remote connected requested?
				
				// Connect to server when client is not connected to server
				if(!mPassDSvc.isConnected()){
					if(D.D) Log.i(LOG, "Remote-connect requested to " + ip);
					onConnectRequested(ip, 30000);
				} else {
					if(D.D) Log.e(LOG, "Client already connected to server!");
				}
			} catch (ArrayIndexOutOfBoundsException e) {
				printOutput("Insert IP Address");
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}

		@Override
		public void onServiceDisconnected(ComponentName name) {
			mPassDSvc = null;
		}
	};
	
	/***
	 * ���񽺷� ���� ���� ��ε�ĳ��Ʈ ������ �޾Ƽ� ó���ϴ� ���ù�
	 */
	private BroadcastReceiver serviceConnReceiver = new BroadcastReceiver(){
		@Override
		public void onReceive(Context context, Intent intent) {
			if(D.D) Log.w("BroadcastReceiver", "onReceive");
			String action = intent.getAction();
			if(PassDIntent.ACTION_CONNECTED.equals(action)){
				printOutput("Welcome to PassD");
				Util.setIP(PassD.this, ip);
				HideBackground();
				AR.getInstance().m_Service.onViewInit();
			} else if(PassDIntent.ACTION_DEVICE_OPEN_FAILED.equals(action)) {
				printOutput("Device open failed\nis Rooted device?");
			} else if(PassDIntent.ACTION_DISCONNECTED.equals(action)) {
				printOutput("Disconnected");
			} else if(PassDIntent.ACTION_INTERRUPTED.equals(action)) {
				printOutput("Interrupted Server");
				finish();
				onDestroy();
			} else if(PassDIntent.ACTION_CONNECTION_FAILED.equals(action)){
				printOutput("Connection Failed");
			}
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		if(D.D) Log.w(LOG, "onCreate");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.layout_main);
		
		btn_connect = (Button)findViewById(R.id.btn_connect);
		edit_ip = (EditText)findViewById(R.id.edit_ip);
		btn_setting = (Button)findViewById(R.id.btn_setting);
		
		edit_ip.setText(Util.getIP(PassD.this));
		btn_connect.setOnClickListener( new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				ip = edit_ip.getText().toString();
				tryConnect(ip, 30000);
			}
		});
		
		btn_setting.setOnClickListener( new OnClickListener() {
			@Override
			public void onClick(View v) {
				startActivity(new Intent(PassD.this, PassDSetting.class));
			}
		});
	}

	@Override
	protected void onStop() {
		super.onStop();
	}
	
	@Override
	protected void onResume() {
		if(D.D) Log.w(LOG, "onResume");
		super.onResume();
		IntentFilter filter = new IntentFilter();
		filter.addAction(PassDIntent.ACTION_CONNECTED);
		filter.addAction(PassDIntent.ACTION_DEVICE_OPEN_FAILED);
		filter.addAction(PassDIntent.ACTION_CONNECTION_FAILED);
		filter.addAction(PassDIntent.ACTION_DISCONNECTED);
		filter.addAction(PassDIntent.ACTION_INTERRUPTED);

		registerReceiver(serviceConnReceiver, filter);
	}

	@Override
	protected void onPause(){
		if(D.D) Log.w(LOG, "onPause");
		super.onPause();
		unregisterReceiver(serviceConnReceiver);
	}

	public void onConnectRequested(String ip, int port) {
		if(D.D) Log.w(LOG, "onConnectRequested");
		try {
			mPassDSvc.connect(ip, port);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void onBackPressed() {
		if(D.D) Log.w(LOG, "onBackPressed");
		android.os.Process.killProcess(android.os.Process.myPid());
		super.onBackPressed();
	}
	
	private void tryConnect(final String ip, final int port) {
		if(D.D) Log.w(LOG, "tryConnect");
		
		if(!Util.Services.isServiceAliveU(getApplicationContext())){
			Util.Services.startPassDService(PassD.this, ip, port);
		}
		bindService(new Intent(this, PassDService.class), conn, Context.BIND_AUTO_CREATE);
	}

	private void printOutput(final String update) {
		if(D.D) Log.w(LOG, "printOutput");
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				Toast.makeText(PassD.this, update, Toast.LENGTH_SHORT).show();
			}
		});
	}

	public void HideBackground() {
		if(D.D) Log.w(LOG, "HideBackground");
		Intent home = new Intent(Intent.ACTION_MAIN); 
		home.addCategory(Intent.CATEGORY_HOME);
		home.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		startActivity(home);
	}
}
