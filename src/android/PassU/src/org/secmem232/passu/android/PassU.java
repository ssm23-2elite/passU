package org.secmem232.passu.android;

import org.secmem232.passu.android.mouse.PassUService;
import org.secmem232.passu.android.setting.PassUSetting;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.googlecode.androidannotations.annotations.AfterViews;
import com.googlecode.androidannotations.annotations.Background;
import com.googlecode.androidannotations.annotations.Click;
import com.googlecode.androidannotations.annotations.EActivity;
import com.googlecode.androidannotations.annotations.UiThread;
import com.googlecode.androidannotations.annotations.ViewById;

@EActivity(R.layout.layout_main)
public class PassU extends Activity {
	private final String LOG = "PassU";
	private boolean isConnected;

	@ViewById(R.id.btn_connect)
	Button btn_connect;

	@ViewById(R.id.edit_ip)
	EditText edit_ip;
	
	private IPassU mPassUSvc;
	private ServiceConnection conn = new ServiceConnection() {
		
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			Log.w(LOG, "onServiceConnected");
			mPassUSvc = IPassU.Stub.asInterface(service);
			try {
				// Remote connected requested?
				String ip = edit_ip.getText().toString();
				// Connect to server when client is not connected to server
				if(!mPassUSvc.isConnected()){
					Log.i(LOG, "Remote-connect requested to " + ip);
					onConnectRequested(ip);
				}else{
					Log.e(LOG, "Client already connected to server!");
				}
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}

		@Override
		public void onServiceDisconnected(ComponentName name) {

		}
	};

	@AfterViews
	protected void onInitialize() {
		edit_ip.setText("211.189.20.139");
		isConnected = false;
		
	}

	@Click
	void btn_connect() {
		if( isConnected ) {
			tryDisconnect();
		} else {
			String ip = edit_ip.getText().toString();
			tryConnect(ip);
		}
		
	}

	@Click
	void btn_setting() {
		startActivity(new Intent(PassU.this, PassUSetting.class));
	}

	public void onConnectRequested(String ipAddress) {
		try {
			mPassUSvc.connect(ipAddress);
		} catch (RemoteException e) {
			e.printStackTrace();
		}

	}

	// tryConnect to Server
	@Background
	void tryConnect(final String ip) {
		Log.w(LOG, "bindService");
		bindService(new Intent(this, PassUService.class), conn, Context.BIND_AUTO_CREATE);
		
		/*isConnected = false;
		Socket socket = new Socket();

		try {
			InetAddress ia = InetAddress.getByName(ip);
			InetSocketAddress remoteAddr = new InetSocketAddress(ia, port);
			final int connectionTimeout = 2000;	// 2 seconds
			socket.connect(remoteAddr, connectionTimeout);

			if(socket.isConnected())
			{
				printOutput("Connection succeed");
				isConnected = true;
			} else {
				throw new SocketException();
			}
		} catch (IOException e) {
			printOutput("Connection failed");
		} catch (Exception e) {
			printOutput("Unknown error");
		} finally {
			try {
				if(!socket.isClosed()) 
					socket.close();
			} catch (Exception e) {};
		}

		if(isConnected) {
			PassUServiceOn(ip, port);
			updateBtnText();
		}*/
	}

	// tryDisconnect to Server
	@Background
	void tryDisconnect() {
		isConnected = false;
		PassUServiceOff();
		updateBtnText();
	}

	@UiThread
	void updateBtnText() {
		if(isConnected)
			btn_connect.setText("Disconnect");
		else
			btn_connect.setText("Connect");
	}

	@UiThread
	void printOutput(String update) {
		Toast.makeText(PassU.this, update, Toast.LENGTH_SHORT).show();
	}

	private void PassUServiceOn(String ip) {	
		Log.w(LOG, "PassUServiceOn");
		Intent i = new Intent();
		i.putExtra("SERVER_IP", ip);
		i.setAction("org.ssm232elite.passu.android.mouse.PassUService");
		startService(i);
	}

	private void PassUServiceOff() {
		Log.w(LOG, "PassUServiceOff");
		Intent i = new Intent();
		i.setAction("org.ssm232elite.passu.android.mouse.PassUService");
		stopService(i);
	}

	public void HideBackground() {
		Intent home = new Intent(Intent.ACTION_MAIN); 
		home.addCategory(Intent.CATEGORY_HOME);
		home.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		startActivity(home);
	}
}
