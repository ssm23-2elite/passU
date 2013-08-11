package org.ssm232elite.passu.android;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

import org.ssm232elite.passu.android.setting.PassUSetting;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;
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
	
	@ViewById(R.id.edit_ipport)
	EditText edit_ipport;

	@AfterViews
	protected void onInitialize() {
		edit_ipport.setText("211.189.20.139:3737");
	}

	@Click
	void btn_connect() {
		DatagramSocket socket;
		String ipport = edit_ipport.getText().toString();
		String ip;
		int port;

		try {
			ip = ipport.split(":")[0];
			port = Integer.parseInt(ipport.split(":")[1]);
			socket = new DatagramSocket(port);
			sendPacket(socket, ip, port);
		} catch (SocketException e) {
			printOutput("Socket Open failed");
			return;
		} catch (Exception e) {
			printOutput("Insert as format 'IP:PORT'");
			return;
		}
	}

	@Click
	void btn_setting() {
		startActivity(new Intent(PassU.this, PassUSetting.class));
	}

	// sendPacket to Server
	@Background
	void sendPacket(DatagramSocket socket, String ip, int port) {
		try	{
			InetAddress address = InetAddress.getByName(ip);
			DatagramPacket packet = new DatagramPacket("Hello Server".getBytes(), 
					"Hello Server".length(), address, port);
			socket.send(packet);

			printOutput("Packet sent, waiting for response \n");

			byte[] buf = new byte[1024];

			packet = new DatagramPacket(buf, buf.length);
			socket.receive(packet);
			
			if (packet.getData().length < 1) {
				printOutput("Timed out with no response \n");
			} else {
				String s = new String(packet.getData(), 0, packet.getLength());
				printOutput("Be connected with Server");
				Log.d(LOG, "Data Received: "+ s);
				
				PassUServiceOn();
			}
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (SocketException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			socket.close();
		}
	}

	// Show Response and other updates on UI
	@UiThread
	void printOutput(String update) {
		Toast.makeText(PassU.this, update, Toast.LENGTH_SHORT).show();
	}
	
	private void PassUServiceOn() {	
		Intent i = new Intent();
		i.setAction("org.ssm232elite.passu.android.mouse.PassUService");
		startService(i);
	}
	
	private void PassUServiceOff() {
		HideCursor();	
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
	private void ShowCursor() {
		if (AR.getInstance().m_CurService != null)
			AR.getInstance().m_CurService.ShowCursor(true);
	}

	private void HideCursor() {
		if (AR.getInstance().m_CurService != null)
			AR.getInstance().m_CurService.ShowCursor(false);
	}
}
