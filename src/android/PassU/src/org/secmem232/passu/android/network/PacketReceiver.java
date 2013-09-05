package org.secmem232.passu.android.network;

import java.io.IOException;
import java.io.InputStream;

import org.secmem232.passu.android.D;

import android.util.Log;

public class PacketReceiver extends Thread {
	
	private final String LOG = "PacketReceiver";
	private byte[] buffer = new byte[Packet.LENGTH];
	private int bufferOffset = 0;
	private Packet packet;
	
	private InputStream recvStream;
	private PacketListener mListener;
	
	public PacketReceiver(InputStream recvStream){
		if(D.D) Log.w(LOG, "PacketReceiver");
		this.recvStream = recvStream;
	}
	
	public PacketReceiver(InputStream recvStream, PacketListener listener){
		if(D.D) Log.w(LOG, "PacketReceiver");
		this.recvStream = recvStream;
		this.mListener = listener;		
	}		
	
	public void setPacketListener(PacketListener listener){
		if(D.D) Log.w(LOG, "setPacketListener");
		this.mListener = listener;
	}
	
	private int readPacketDataFromStream() throws IOException{
		int readLen = recvStream.read(buffer, 0, Packet.LENGTH);
		if(D.D) Log.w(LOG, "readPacketDataFromStream" + readLen);	
		if(readLen > 0)
			bufferOffset += readLen;
		
		return readLen;
	}
	
	private boolean tryReadPacketData(){
		if(bufferOffset < PacketHeader.LENGTH)
			return false; // try fetching more data from stream
		
		packet = Packet.parse(buffer);
		// Decrease current offset by last packet's length
		
		bufferOffset -= Packet.LENGTH;		
		// Return packet object
		return true;
	}	
	

	@Override
	public void run() {	
		if(D.D) Log.w(LOG, "run");
		if(mListener == null)
			throw new IllegalStateException("Packet listener must be set first.");
		
		/**
		 * Run infinitely and get packet from stream before user requested to stop
		 */		
		while(true){
			try{
				int readLen = readPacketDataFromStream();
				if(readLen < 0){
					//when host was closed
					throw new IOException();
				}
				while(tryReadPacketData()){
					mListener.onPacketReceived(packet);
				}				
			} catch(IOException e){
				e.printStackTrace();					
				mListener.onInterrupt();
				break;
			}
		}
	}		
}