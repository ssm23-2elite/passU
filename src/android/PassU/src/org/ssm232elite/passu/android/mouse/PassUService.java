package org.ssm232elite.passu.android.mouse;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import org.ssm232elite.passu.android.AR;

import android.app.Service;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.IBinder;
import android.util.Log;
import android.view.Gravity;
import android.view.WindowManager;

import com.googlecode.androidannotations.annotations.EService;

public class PassUService extends Service {
	
	private static final String LOG = "PassUService";
	
	private PassUMouseT mView;
	private DatagramSocket socket;
	private Boolean shouldRestartSocketListen = true;
	private Thread UDPBroadcastThread;
	static String UDP_BROADCAST = "UDPBroadcast";

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		AR.getInstance().m_CurService = this;
		Log.w(LOG, "onStartCommand");
		
		onViewInit();

		shouldRestartSocketListen = true;
		startListenForUDPBroadcast();
		ShowCursor(true);
		return super.onStartCommand(intent, flags, startId);
	}

	private void onViewInit() {
		Log.w(LOG, "onViewInit");
		
		mView = new PassUMouseT(this);
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

	void startListenForUDPBroadcast() {
		Log.w(LOG, "startListenForUDPBroadcast");
		
		UDPBroadcastThread = new Thread(new Runnable() {
			public void run() {
				try {
					InetAddress broadcastIP = InetAddress.getByName("127.0.0.1");
					Integer port = 3737;
					while (shouldRestartSocketListen) {
						listenAndWaitAndThrowIntent(broadcastIP, port);
					}
					//if (!shouldListenForUDPBroadcast) throw new ThreadDeath();
				} catch (Exception e) {
					Log.i("UDP", "no longer listening for UDP broadcasts cause of error " + e.getMessage());
				}
			}
		});
		UDPBroadcastThread.start();
	}

	
	/**
	 * listenAndWaitAndThrowIntent
	 * @param broadcastIP
	 * @param port
	 * @throws Exception
	 */
	private void listenAndWaitAndThrowIntent(InetAddress broadcastIP, Integer port) throws Exception {
		Log.w(LOG, "listenAndWaitAndThrowIntent");
		
		byte[] recvBuf = new byte[1024];
		if (socket == null || socket.isClosed()) {
			socket = new DatagramSocket(port, broadcastIP);
			socket.setBroadcast(true);
		}
		
		DatagramPacket packet = new DatagramPacket(recvBuf, recvBuf.length);
		Log.e("UDP", "Waiting for UDP broadcast");
		socket.receive(packet);

		String senderIP = packet.getAddress().getHostAddress();
		String message = new String(packet.getData()).trim();

		if(message.equals("1")) {
			Update(0, 10, true);
			Log.w(LOG, "UP");
		} else if (message.equals("2")) {
			Update(0, -10, true);
			Log.w(LOG, "DOWN");
		} else if (message.equals("3")) {
			Update(-10, 0, true);
			Log.w(LOG, "LEFT");
		} else if (message.equals("4")) {
			Update(10, 0, true);
			Log.w(LOG, "RIGHT");
		}
		Log.w(LOG, "broadcastData IP: " + senderIP + ", Message: " + message);
		socket.close();
	}

	private void broadcastIntent(String senderIP, String message) {
		/*Intent intent = new Intent(PassU.this);
		intent.putExtra("sender", senderIP);
		intent.putExtra("message", message);
		sendBroadcast(intent);*/
	}


	@Override
	public void onDestroy() {
		super.onDestroy();
		AR.getInstance().m_CurService = null;
		
		Log.w(LOG, "onDestroy");
		if(mView != null) {
			((WindowManager) getSystemService(WINDOW_SERVICE)).removeView(mView);
			mView = null;
		}
	}

	public void Update(final int x, final int y, final boolean autoenable) {
		Log.w(LOG, "Update x: " + x + ", y: " + y);
		
		mView.Update(x,y);
		if ((x!=0 || y!= 0) && autoenable && !mView.isCursorShown() ) 
			ShowCursor(true); //will also post invalidate
		else
			mView.postInvalidate();
	}

	public void ShowCursor(boolean status) {
		mView.ShowCursor(status);
		mView.postInvalidate();
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	public int getX() {
		return mView.x;
	}

	public int getY() {
		return mView.y;
	}
}

