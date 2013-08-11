package org.ssm232elite.passu.android;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

import org.ssm232elite.passu.android.network.UDPReceiver;
import org.ssm232elite.passu.android.setting.PassUSetting;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
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
public class PassU extends Activity implements UDPReceiver {

	@ViewById(R.id.edit_ipport)
	EditText edit_ipport;

	@AfterViews
	protected void onSetting() {
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
			socket = openSocket(port);
			sendPacket(socket, ip, port);
				
		} catch (Exception e) {
			Toast.makeText(PassU.this, "IP:PORT로 입력해주세요.", Toast.LENGTH_SHORT).show();
			return;
		}
	}
	
	@Click
	void btn_setting() {
		startActivity(new Intent(PassU.this, PassUSetting.class));
	}

	DatagramSocket openSocket(int port) {
		try	{
			return new DatagramSocket(port);
		} catch (SocketException e) {
			e.printStackTrace();
		}
		return null;
	}

	// Network interactions in the background on other thread
	@Background
	void sendPacket(DatagramSocket socket, String ip, int port) {
		try	{
			InetAddress address = InetAddress.getByName(ip);

			DatagramPacket packet = new DatagramPacket("Hello Server".getBytes(), 
					"Hello Server".length(), address, port);
			socket.send(packet);
			Log.d("AndroidUDP", "Sent: " + packet);
			printOutput("Packet sent, waiting for response \n");

			//Now get response
			byte[] buf = new byte[1024];
			packet = new DatagramPacket(buf, buf.length);
			socket.setSoTimeout(5000);
			socket.receive(packet);
			if (packet.getData().length < 1) {
				printOutput("Timed out with no response \n");
			} else {
				String s = new String(packet.getData(), 0, packet.getLength());
				printOutput(s);
				Log.d("AndroidUDP", "Received: "+ s);
				printOutput(s);
			}

		}	catch (UnknownHostException e) {
			e.printStackTrace();
		}	catch (SocketException e) {
			e.printStackTrace();
		}	catch (IOException e) {
			e.printStackTrace();
		}	finally {
			socket.close();
		}
	}


	// Show Response and other updates on UI
	@UiThread
	void printOutput(String update) {
		Toast.makeText(PassU.this, update, Toast.LENGTH_SHORT).show();
	}

	@Override
	public void onReceive(byte[] buf) {
		// TODO Auto-generated method stub
		Toast.makeText(PassU.this, "Receive Data", Toast.LENGTH_SHORT).show();
	}

	@Override
	public void onError(int errno) {
		// TODO Auto-generated method stub

	}

	@Override
	public void onConnected() {
		Toast.makeText(PassU.this, "연결되었습니다.", Toast.LENGTH_SHORT).show();

		Intent home = new Intent(Intent.ACTION_MAIN); 
		home.addCategory(Intent.CATEGORY_HOME);
		home.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		startActivity(home);
	}

	@Override
	public void onDisConnected() {
		// TODO Auto-generated method stub

	}

	@Override
	public void onConnectionFailed() {
		// TODO Auto-generated method stub
		Log.e("LOG", "연결실패");
		//Toast.makeText(PassU.this, "연결 실패", Toast.LENGTH_LONG).show();
	}
}
