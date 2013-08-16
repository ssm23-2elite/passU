package org.secmem232.passu.android.network;

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
	public void send(Packet packet) throws IOException{		
		synchronized(java.lang.Object.class){
			sendStream.write(packet.asByteArray(), 0, Packet.LENGTH);
		}
	}
}
