package org.secmem232.passu.android.mouse;

import org.secmem232.passu.android.AR;
import org.secmem232.passu.android.D;
import org.secmem232.passu.android.IPassU;
import org.secmem232.passu.android.intent.PassUIntent;
import org.secmem232.passu.android.natives.InputHandler;
import org.secmem232.passu.android.network.AddOptionListener;
import org.secmem232.passu.android.network.PassUSocket;
import org.secmem232.passu.android.network.ServerConnectionListener;
import org.secmem232.passu.android.network.VirtualEventListener;

import android.app.Service;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.RemoteException;
import android.util.Log;
import android.view.Gravity;
import android.view.WindowManager;
import android.widget.Toast;

public class PassUService extends Service implements 
				ServerConnectionListener, VirtualEventListener, AddOptionListener {
	private final String LOG = "PassUService";
	public enum ServiceState{IDLE, CONNECTING, CONNECTED};

	private PassUSocket mSocket;
	private InputHandler mInputHandler;
	private static ServiceState mState = ServiceState.IDLE;

	private PassUMouse mMouseView;

	private IBinder mBinder = new IPassU.Stub() {
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
					// Close input device
					mInputHandler.close();
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
					
					mInputHandler.open();					
					// Start connection and receive events from server
					mSocket.connect(ip, port);
					//Send devices resolution to host for coordinate transformation;
					if(mSocket != null && mSocket.isConnected()){
						mState = ServiceState.CONNECTING;
						sendBroadcast(new Intent(PassUIntent.ACTION_CONNECTED));
						
						//mSocket.sendDeviceInfo(getApplicationContext().getResources().getDisplayMetrics());
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

		mInputHandler = new InputHandler(PassUService.this);
		mSocket = new PassUSocket(this);
		mSocket.setVirtualEventListener(this);
		mSocket.setAddOptionListener(this);
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
		onViewInit();
		HideCursor();
		
		if(mMouseView != null) {
			((WindowManager) getSystemService(WINDOW_SERVICE)).removeView(mMouseView);
			mMouseView = null;
		}
		super.onDestroy();
	}

	/*	
	private void showConnectionNotification(String ipAddress){
		Notification notification = new Notification();
		PendingIntent intent = PendingIntent.getActivity(getApplicationContext(), 0, new Intent(this, Main.class), 0);
		notification.icon = R.drawable.ic_launcher;
		notification.tickerText = String.format(getString(R.string.connected_to_s), ipAddress);
		notification.when = System.currentTimeMillis();
		notification.setLatestEventInfo(getApplicationContext(), getString(R.string.app_name), String.format(getString(R.string.connected_to_s), ipAddress), intent);

		notification.flags |= Notification.FLAG_ONGOING_EVENT;
		NotificationManager notifManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
		notifManager.notify(NOTIFICATION_ID, notification);
	}

		private void dismissNotification(){
		NotificationManager notifManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
		notifManager.cancelAll();
	}*/

	public void onViewInit() {
		if(D.D) Log.w(LOG, "onViewInit");

		mMouseView = new PassUMouse(this);
		WindowManager.LayoutParams params = new WindowManager.LayoutParams(
				WindowManager.LayoutParams.WRAP_CONTENT,
				WindowManager.LayoutParams.WRAP_CONTENT,
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
		if(mInputHandler.isDeviceOpened()) {
			mInputHandler.touchSetPtr(xPosition, yPosition);
			Update(xPosition, yPosition, true);
		}
	}

	@Override
	public void onTouchDown() {
		if(D.D) Log.w(LOG, "onTouchDown");
		if(mInputHandler.isDeviceOpened())
			mInputHandler.touchDown();		
	}

	@Override
	public void onTouchUp() {
		if(D.D) Log.w(LOG, "onTouchUp");
		if(mInputHandler.isDeviceOpened())
			mInputHandler.touchUp();		
	}


	@Override
	public void onKeyDown(int keyCode) {
		if(D.D) Log.w(LOG, "onKeyDown");
		if(mInputHandler.isDeviceOpened())
			mInputHandler.keyDown(keyCode);
	}

	@Override
	public void onKeyUp(int keyCode) {
		if(D.D) Log.w(LOG, "onKeyUp");
		if(mInputHandler.isDeviceOpened()){
			mInputHandler.keyUp(keyCode);
		}
	}

	@Override
	public void onKeyStroke(int keyCode) {
		if(D.D) Log.w(LOG, "onKeyStroke");
		if(mInputHandler.isDeviceOpened())
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
		sendBroadcast(new Intent(PassUIntent.ACTION_CONNECTION_FAILED));

		new AsyncTask<Void, Void, Void>(){
			@Override
			protected Void doInBackground(Void... params) {				
				mInputHandler.close();				
				return null;
			}
		}.execute();
	}

	@Override
	public void onServerConnectionInterrupted() {
		if(D.D) Log.w(LOG, "onServerConnectionInterrupted");
		mState = ServiceState.IDLE;
		HideCursor();
		sendBroadcast(new Intent(PassUIntent.ACTION_INTERRUPTED));

		Looper.prepare();
		new AsyncTask<Void, Void, Void>(){
			@Override
			protected Void doInBackground(Void... params) {				
				mInputHandler.close();				
				return null;
			}
		}.execute();
		Toast.makeText(getApplicationContext(), "Server Connection Interrupted", Toast.LENGTH_SHORT).show();
		onDestroy();
		Looper.loop();
	}

	@Override
	public void onServerDisconnected() {
		if(D.D) Log.w(LOG, "onServerDisconnected");
		mState = ServiceState.IDLE;		
		// Sending broadcast for disconnection..
		sendBroadcast(new Intent(PassUIntent.ACTION_DISCONNECTED));
		Looper.prepare();
		Toast.makeText(getApplicationContext(), "Server Disconnected", Toast.LENGTH_SHORT).show();
		onDestroy();
		Looper.loop();
	}

	@Override
	public void setClipBoard(final String msg) {
		if(D.D) Log.w(LOG, "setClipBoard");
		/*Runnable updater = new Runnable() {
			public void run() {
				ClipboardManager clip = 
						(ClipboardManager)getApplicationContext().getSystemService(Context.CLIPBOARD_SERVICE);
				ClipData data = ClipData.newPlainText("Remoteroid_Clip", msg);
				clip.setPrimaryClip(data);					
			}             
		};
		handler.post(updater);*/
	}
}

