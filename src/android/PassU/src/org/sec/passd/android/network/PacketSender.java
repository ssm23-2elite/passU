package org.sec.passd.android.network;

import java.io.IOException;
import java.io.OutputStream;

import android.app.Notification;

public class PacketSender {
	private OutputStream sendStream;	
	public PacketSender(OutputStream stream){
		this.sendStream = stream;		
	}
	public void setOutputStream(OutputStream stream){
		this.sendStream = stream;
	}
	public void send(byte[] b) throws IOException{		
		synchronized(java.lang.Object.class){
			sendStream.write(b, 0, b.length);
		}
	}
}
