package org.ssm232elite.passu.android.gcm;

import org.ssm232elite.passu.android.mouse.Singleton;
import org.ssm232elite.passu.android.natives.InputHandler;
import org.ssm232elite.passu.android.network.KeyEventListener;

import android.app.IntentService;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.IBinder;
import android.os.PowerManager;
import android.util.Log;

public class GCMIntentService extends IntentService implements KeyEventListener {

	private static PowerManager.WakeLock sWakeLock;
	private static final Object LOCK = GCMIntentService.class;
	private InputHandler mInputHandler;
	
	static void runIntentInService(Context context, Intent intent) {
		synchronized(LOCK) {
			if (sWakeLock == null) {
				PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
				sWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "my_wakelock");
			}
		}
		sWakeLock.acquire();
		intent.setClassName(context, GCMIntentService.class.getName());
		context.startService(intent);
	}

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
        
        mInputHandler = new InputHandler(this);
        
        new AsyncTask<Void, Void, Void>(){
			@Override 
			protected Void doInBackground(Void... params) {
					mInputHandler.open();
					return null;	
			}
		}.execute();  
	}
	public GCMIntentService() {
		super("GCMIntentService");
		// TODO Auto-generated constructor stub
	}

	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	protected void onHandleIntent(Intent intent) {
		// TODO Auto-generated method stub
		try {
			String action = intent.getAction();
			if (action.equals("com.google.android.c2dm.intent.REGISTRATION")) {
				handleRegistration(intent);
			} else if (action.equals("com.google.android.c2dm.intent.RECEIVE")) {
				handleMessage(intent);
			}
		} finally {
			synchronized(LOCK) {
				sWakeLock.release();
			}
		}
	}

	private void handleMessage(Intent intent) {
		// TODO Auto-generated method stub
		
		String data =  intent.getStringExtra("DATA");
		if( data.equals("LEFT") ) {
			Singleton.getInstance().m_CurService.Update(-10,0, true);
			onSetCoordinates(Singleton.getInstance().m_CurService.getX(), Singleton.getInstance().m_CurService.getY());
		} else if (data.equals("RIGHT")) {
			Singleton.getInstance().m_CurService.Update(10,0, true);
			onSetCoordinates(Singleton.getInstance().m_CurService.getX(), Singleton.getInstance().m_CurService.getY());
		} else if (data.equals("UP")) {
			Singleton.getInstance().m_CurService.Update(0,-10, true);
			onSetCoordinates(Singleton.getInstance().m_CurService.getX(), Singleton.getInstance().m_CurService.getY());
		} else if (data.equals("DOWN")) {
			Singleton.getInstance().m_CurService.Update(0,10, true);
			onSetCoordinates(Singleton.getInstance().m_CurService.getX(), Singleton.getInstance().m_CurService.getY());
		} else if (data.equals("CLICK")) {
			onTouchDown();
			onTouchUp();
		}
	}

	private void handleRegistration(Intent intent) {
		// TODO Auto-generated method stub
		String registrationId = intent.getStringExtra("registration_id");
		String error = intent.getStringExtra("error");
		String unregistered = intent.getStringExtra("unregistered");     

		Log.i("Id", "registrationId = " + registrationId);

		// registration succeeded
		if (registrationId != null) {
			// store registration ID on shared preferences
			// notify 3rd-party server about the registered ID
		}

		// unregistration succeeded
		if (unregistered != null) {
			// get old registration ID from shared preferences
			// notify 3rd-party server about the unregistered ID
		} 

		// last operation (registration or unregistration) returned an error;
		if (error != null) {
			if ("SERVICE_NOT_AVAILABLE".equals(error)) {
				// optionally retry using exponential back-off
				// (see Advanced Topics)
			} else {
				// Unrecoverable error, log it
				Log.i("TAG", "Received error: " + error);
			}
		}
	}

	@Override
	public void onSetCoordinates(int xPosition, int yPosition) {
		if(mInputHandler.isDeviceOpened()) {
			Singleton.getInstance().m_CurService.Update(xPosition, yPosition, true);
			mInputHandler.touchSetPtr(xPosition, yPosition);
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
		if(mInputHandler.isDeviceOpened())
			mInputHandler.keyUp(keyCode);
	}
	@Override
	public void onKeyStroke(int keyCode) {
		if(mInputHandler.isDeviceOpened())
			mInputHandler.keyStroke(keyCode);
	}
	
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		mInputHandler.close();
	}
}


