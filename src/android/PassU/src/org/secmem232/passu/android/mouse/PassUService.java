package org.secmem232.passu.android.mouse;

import org.secmem232.passu.android.AR;
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
import android.os.IBinder;
import android.os.Looper;
import android.os.RemoteException;
import android.util.Log;
import android.view.Gravity;
import android.view.WindowManager;
import android.widget.Toast;

public class PassUService extends Service 
implements ServerConnectionListener, VirtualEventListener, AddOptionListener {

	public enum ServiceState{IDLE, CONNECTING, CONNECTED};

	private PassUSocket mSocket;
	private InputHandler mInputHandler;
	private static ServiceState mState = ServiceState.IDLE;

	private static final String LOG = "PassUService";

	private PassUMouse mView;

	private Handler handler;

	private IBinder mBinder = new IPassU.Stub() {
		@Override
		public boolean isConnected() throws RemoteException {
			// TODO Auto-generated method stub
			return (mSocket != null && mSocket.isConnected()) ? true : false;
		}

		@Override
		public String getConnectionStatus() throws RemoteException {
			// TODO Auto-generated method stub
			return mState.name();
		}

		@Override
		public void disconnect() throws RemoteException {
			// Do time-consuming (blocks UI thread, causes activity death) task on here
			new AsyncTask<Void, Void, Void>(){
				@Override
				protected Void doInBackground(Void... params) {					
					// Close input device
					mInputHandler.close();

					mSocket.disconnect();
					Log.i(LOG,"disconnect()");
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
					// Start connection and receive events from server
					mSocket.connect(ip, port);
					
					//Send devices resolution to host for coordinate transformation;
					if(mSocket != null && mSocket.isConnected()){
						mState = ServiceState.CONNECTING;
						//mSocket.sendDeviceInfo(getApplicationContext().getResources().getDisplayMetrics());
					}
					return null;
				}
			}.execute();
		}
	};

	@Override
	public IBinder onBind(Intent intent) {
		return mBinder;
	}

	public ServiceState getConnectionState() {
		return mState;
	}

	@Override
	public void onCreate() {
		super.onCreate();
		AR.getInstance().m_CurService = this;
		
		mSocket = new PassUSocket(this);
		mSocket.setVirtualEventListener(this);
		mSocket.setAddOptionListener(this);
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.w(LOG, "onStartCommand");
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		AR.getInstance().m_CurService = null;
		HideCursor();
		Log.w(LOG, "onDestroy");
		if(mView != null) {
			((WindowManager) getSystemService(WINDOW_SERVICE)).removeView(mView);
			mView = null;
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

	private void onViewInit() {
		Log.w(LOG, "onViewInit");

		mView = new PassUMouse(this);
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
		wm.addView(mView, params);
	}

	@Override
	public void onSetCoordinates(int xPosition, int yPosition) {
		if(mInputHandler.isDeviceOpened()) {
			mInputHandler.touchSetPtr(xPosition, yPosition);
			Update(xPosition, yPosition, true);
		}
	}

	@Override
	public void onTouchDown() {
		if(mInputHandler.isDeviceOpened())
			mInputHandler.touchDown();		
	}

	@Override
	public void onTouchUp() {
		if(mInputHandler.isDeviceOpened())
			mInputHandler.touchUp();		
	}


	@Override
	public void onKeyDown(int keyCode) {
		if(mInputHandler.isDeviceOpened())
			mInputHandler.keyDown(keyCode);
	}

	@Override
	public void onKeyUp(int keyCode) {
		if(mInputHandler.isDeviceOpened()){
			mInputHandler.keyUp(keyCode);
		}
	}

	@Override
	public void onKeyStroke(int keyCode) {
		if(mInputHandler.isDeviceOpened())
			mInputHandler.keyStroke(keyCode);
	}

	public void Update(final int x, final int y, final boolean autoenable) {
		mView.Update(x,y);
		if ((x != 0 || y != 0) && autoenable && !mView.isCursorShown() ) 
			ShowCursor(); //will also post invalidate
		else
			mView.postInvalidate();
	}

	public void ShowCursor() {
		mView.ShowCursor();
		mView.postInvalidate();
	}

	public void HideCursor() {
		mView.HideCursor();
		mView.postInvalidate();
	}


	public int getX() {
		return mView.x;
	}

	public int getY() {
		return mView.y;
	}

	@Override
	public void onServerConnected(String ipAddress) {
		Log.w(LOG, "onServerConnected");
		mState = ServiceState.CONNECTED;
		
		Looper.prepare();
		
		handler = new Handler();
		onViewInit();
		ShowCursor();
		sendBroadcast(new Intent(PassUIntent.ACTION_CONNECTED));
		mInputHandler = new InputHandler(this);
		
		Looper.loop();
	}

	@Override
	public void onServerConnectionFailed() {
		Log.i(LOG, "onServerConnectionFailed");
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
		Log.i(LOG, "onServerConnectionInterrupted");
		mState = ServiceState.IDLE;
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
		Log.i(LOG, "onServerDisconnected");
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
		Runnable updater = new Runnable() {
			public void run() {
				ClipboardManager clip = 
						(ClipboardManager)getApplicationContext().getSystemService(Context.CLIPBOARD_SERVICE);
				ClipData data = ClipData.newPlainText("Remoteroid_Clip", msg);
				clip.setPrimaryClip(data);					
			}             
		};
		handler.post(updater);
	}
}

