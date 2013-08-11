package org.ssm232elite.passu.android.network;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import android.util.Log;

public class UDPClient implements Runnable {

	public String SERVERIP;
	public int SERVERPORT;    
	private UDPReceiver receiver;
	
	public UDPClient(String ip, int port, UDPReceiver receiver) {
		this.SERVERIP = ip;
		this.SERVERPORT = port;
		this.receiver = receiver;
	}
	 
	public void run() {   
		// TODO Auto-generated method stub
		try {   
			InetAddress serverAddr = InetAddress.getByName(SERVERIP);   
            DatagramSocket socket = new DatagramSocket();   
                
            byte[] buf = ("Hello from Client").getBytes();   
                
            DatagramPacket packet = new DatagramPacket(buf, buf.length, serverAddr, SERVERPORT);   
            socket.send(packet);   
            
            socket.receive(packet);
            Log.d("UDP", "C: Received: '" + new String(packet.getData()) + "'");   
		} catch (Exception e) {   
			receiver.onConnectionFailed();
		}    
	}   
}
