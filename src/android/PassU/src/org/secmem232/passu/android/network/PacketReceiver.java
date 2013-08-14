package org.secmem232.passu.android.network;

import java.io.IOException;
import java.io.InputStream;

/**
 * Get packet from host and notify to other component via listener to response to each packet properly.
 * @author Taeho Kim
 *
 */
public class PacketReceiver extends Thread{		

	private byte[] buffer = new byte[Packet.MAX_LENGTH*2];
	private int bufferOffset = 0;
	private Packet packet;
	
	private InputStream recvStream;
	private PacketListener mListener;
	
	public PacketReceiver(InputStream recvStream){
		this.recvStream = recvStream;
	}
	
	public PacketReceiver(InputStream recvStream, PacketListener listener){
		this.recvStream = recvStream;
		this.mListener = listener;		
	}		
	
	public void setPacketListener(PacketListener listener){
		this.mListener = listener;
	}
	
	private int readPacketDataFromStream() throws IOException{
		
		int readLen = recvStream.read(buffer, bufferOffset, Packet.MAX_LENGTH);			
			
		if(readLen>0)
			bufferOffset+=readLen;
		
		return readLen;
	}
	
	private boolean tryReadPacketData(){
		if(bufferOffset < PacketHeader.LENGTH)
			return false; // try fetching more data from stream
		
		// Try getting header data
		PacketHeader header = PacketHeader.parse(buffer);
		
		// If read data's length is smaller than whole packet's length
		
		if(bufferOffset < header.getPacketLength())
			return false; //  try fetching more data from stream
		
		// If you reached here, all required packed data has received.
		// Now we can parse received data as Packet object.
		packet = Packet.parse(buffer);
		
		// Decrease current offset by last packet's length
		bufferOffset-=header.getPacketLength();
		
		//The remaining packets moves forward
		System.arraycopy(buffer, header.getPacketLength(), buffer, 0, bufferOffset);
		
		// Return packet object
		return true;
	}	
	

	@Override
	public void run() {	
		if(mListener==null)
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