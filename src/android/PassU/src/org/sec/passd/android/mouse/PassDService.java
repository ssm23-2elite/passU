package org.sec.passd.android.mouse;

import org.sec.passd.android.AR;
import org.sec.passd.android.D;
import org.sec.passd.android.IPassD;
import org.sec.passd.android.intent.PassDIntent;
import org.sec.passd.android.natives.InputHandler;
import org.sec.passd.android.natives.KeyCodeMap;
import org.sec.passd.android.network.PassDSocket;
import org.sec.passd.android.network.ServerConnectionListener;
import org.sec.passd.android.network.VirtualEventListener;

import android.app.Service;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.AsyncTask;
import android.os.IBinder;
import android.os.Looper;
import android.os.RemoteException;
import android.util.Log;
import android.view.Gravity;
import android.view.WindowManager;
import android.widget.Toast;

public class PassDService extends Service implements 
				ServerConnectionListener, VirtualEventListener {
	private final String LOG = "PassDService";
	public enum ServiceState{IDLE, CONNECTING, CONNECTED};

	private PassDSocket mSocket;
	private InputHandler mInputHandler;
	private static ServiceState mState = ServiceState.IDLE;

	private PassDMouse mMouseView;

	private IBinder mBinder = new IPassD.Stub() {
		@Override
		public boolean isConnected() throws RemoteException {
			if(D.D) Log.w(LOG, "isConnected");
			return (mSocket != null && mSocket.isConnected()) ? true : false;
		}

		@Override
		public String getConnectionStatus() throws RemoteException {
			if(D.D) Log.w(LOG, "getConnectionStatus");
			return mState.name();
		}

		@Override
		public void disconnect() throws RemoteException {
			new AsyncTask<Void, Void, Void>(){
				@Override
				protected Void doInBackground(Void... params) {
					if(D.D) Log.w(LOG, "disconnect");
					mSocket.disconnect();
					mState = ServiceState.IDLE;
					return null;
				}
			}.execute();
		}

		@Override
		public void connect(final String ip, final int port) throws RemoteException {
			new AsyncTask<Void, Void, Void>() {
				@Override
				protected Void doInBackground(Void... params) {
					if(D.D) Log.w(LOG, "connect");
					
					// Start connection and receive events from server
					if( mSocket.connect(ip, port) == true ) {
						//Send devices resolution to host for coordinate transformation;
						if(mSocket != null && mSocket.isConnected()){
							mState = ServiceState.CONNECTING;
							sendBroadcast(new Intent(PassDIntent.ACTION_CONNECTED));
							
							//mSocket.sendDeviceInfo(getApplicationContext().getResources().getDisplayMetrics());
						}
					}
					return null;
				}
			}.execute();
		}
	};

	@Override
	public IBinder onBind(Intent intent) {
		if(D.D) Log.w(LOG, "onBind");
		return mBinder;
	}

	public ServiceState getConnectionState() {
		if(D.D) Log.w(LOG, "getConnectionState");
		return mState;
	}

	@Override
	public void onCreate() {
		super.onCreate();
		if(D.D) Log.w(LOG, "onCreate");
		AR.getInstance().m_Service = this;

		mInputHandler = new InputHandler(PassDService.this);
		mSocket = new PassDSocket(this);
		mSocket.setVirtualEventListener(this);
		KeyCodeMap.init();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		if(D.D) Log.w(LOG, "onStartCommand");
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		if(D.D) Log.w(LOG, "onDestroy");
		AR.getInstance().m_Service = null;
		HideCursor();
		if(mMouseView != null) {
			((WindowManager) getSystemService(WINDOW_SERVICE)).removeView(mMouseView);
			mMouseView = null;
		}
		super.onDestroy();
	}

	public void onViewInit() {
		if(D.D) Log.w(LOG, "onViewInit");

		mMouseView = new PassDMouse(this);
		WindowManager.LayoutParams params = new WindowManager.LayoutParams(
				WindowManager.LayoutParams.MATCH_PARENT,
				WindowManager.LayoutParams.MATCH_PARENT,
				WindowManager.LayoutParams.TYPE_SYSTEM_OVERLAY,//TYPE_SYSTEM_ALERT,//TYPE_SYSTEM_OVERLAY,
				WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE |
				WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN, //will cover status bar as well!!!
				PixelFormat.TRANSLUCENT);
		params.gravity = Gravity.LEFT | Gravity.TOP;
		params.setTitle("Cursor");
		WindowManager wm = (WindowManager) getSystemService(WINDOW_SERVICE);
		wm.addView(mMouseView, params);
	}

	@Override
	public void onSetCoordinates(int xPosition, int yPosition) {
		if(D.D) Log.w(LOG, "onSetCoordinates");
		mInputHandler.touchSetPtr(xPosition, yPosition);
		Update(xPosition, yPosition, true);
	}

	@Override
	public void onTouchDown() {
		if(D.D) Log.w(LOG, "onTouchDown");
		mInputHandler.touchDown();		
	}

	@Override
	public void onTouchUp() {
		if(D.D) Log.w(LOG, "onTouchUp");
		mInputHandler.touchUp();		
	}


	@Override
	public void onKeyDown(int keyCode) {
		if(D.D) Log.w(LOG, "onKeyDown");
		mInputHandler.keyDown(keyCode);
	}

	@Override
	public void onKeyUp(int keyCode) {
		if(D.D) Log.w(LOG, "onKeyUp");
		mInputHandler.keyUp(keyCode);
	}

	@Override
	public void onKeyStroke(int keyCode) {
		if(D.D) Log.w(LOG, "onKeyStroke");
		mInputHandler.keyStroke(keyCode);
	}

	public void Update(final int x, final int y, final boolean autoenable) {
		if(D.D) Log.w(LOG, "Update");
		if(mMouseView != null) {
			mMouseView.Update(x,y);
			if ((x != 0 || y != 0) && autoenable && !mMouseView.isCursorShown() ) 
				ShowCursor(); //will also post invalidate
			else
				mMouseView.postInvalidate();
		}
	}

	public void ShowCursor() {
		if(D.D) Log.w(LOG, "ShowCursor");
		mMouseView.ShowCursor();
		mMouseView.postInvalidate();
	}

	public void HideCursor() {
		if(D.D) Log.w(LOG, "HideCursor");
		mMouseView.HideCursor();
		mMouseView.postInvalidate();
	}


	public int getX() {
		if(D.D) Log.w(LOG, "getX");
		return mMouseView.x;
	}

	public int getY() {
		if(D.D) Log.w(LOG, "getY");
		return mMouseView.y;
	}

	@Override
	public void onServerConnected(String ip, int port) {
		if(D.D) Log.w(LOG, "onServerConnected");
		mState = ServiceState.CONNECTED;
	}

	@Override
	public void onServerConnectionFailed() {
		if(D.D) Log.w(LOG, "onServerConnectionFailed");
		mState = ServiceState.IDLE;
		sendBroadcast(new Intent(PassDIntent.ACTION_CONNECTION_FAILED));
	}

	@Override
	public void onServerConnectionInterrupted() {
		if(D.D) Log.w(LOG, "onServerConnectionInterrupted");
		mState = ServiceState.IDLE;
		HideCursor();
		sendBroadcast(new Intent(PassDIntent.ACTION_INTERRUPTED));
		onDestroy();
	}

	@Override
	public void onServerDisconnected() {
		if(D.D) Log.w(LOG, "onServerDisconnected");
		mState = ServiceState.IDLE;		
		// Sending broadcast for disconnection..
		sendBroadcast(new Intent(PassDIntent.ACTION_DISCONNECTED));
		Looper.prepare();
		onDestroy();
		Looper.loop();
	}
}

