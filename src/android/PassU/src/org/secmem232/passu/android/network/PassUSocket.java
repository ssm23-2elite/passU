package org.secmem232.passu.android.network;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

import android.util.Log;

public class PassUSocket implements PacketListener {

	private static final int PORT = 3737;
	private final String LOG = "PassUSocket";
	
	private Socket socket;
	private OutputStream sendStream;
	private InputStream recvStream;

	private PacketReceiver packetReceiver;	

	// Event listeners
	private VirtualEventListener mVirtEventListener;
	private ServerConnectionListener mServerConnectionListener;
	private AddOptionListener mAddOptionListener;

	private PacketSender packetSender;

	public PassUSocket(ServerConnectionListener listener){
		mServerConnectionListener = listener;
	}

	/**
	 * Check whether connected to server or not.
	 * @return true if connected to server, false otherwise
	 */
	public boolean isConnected(){
		return socket != null ? socket.isConnected() : false;		
	}

	public void setVirtualEventListener(VirtualEventListener listener){
		mVirtEventListener = listener;
	}

	public void setAddOptionListener(AddOptionListener listner) {
		mAddOptionListener = listner;
	}


	/**
	 * Connect to specified host.
	 * @param ipAddr ip address
	 * @throws IOException
	 */
	public synchronized void connect(String ipAddr){
		Log.w(LOG, "connect");
		try{
			socket = new Socket();

			socket.connect(new InetSocketAddress(ipAddr, PORT), 5000); // Set timeout to 5 seconds

			// Open outputStream
			sendStream = socket.getOutputStream();

			// Open inputStream
			recvStream = socket.getInputStream();		
			packetSender = new PacketSender(sendStream);

			// Create and start packet receiver
			packetReceiver = new PacketReceiver(recvStream);
			packetReceiver.setPacketListener(this);
			packetReceiver.start();	

			mServerConnectionListener.onServerConnected(ipAddr);
		} catch(IOException e) {
			e.printStackTrace();
			mServerConnectionListener.onServerConnectionFailed();
		}
	}

	/**
	 * Disconnect from host.
	 * @throws IOException
	 */
	public void disconnect(){
		synchronized(this){
			if(socket!=null){
				try{				
					recvStream.close();
					sendStream.close();
					packetReceiver = null;				
					socket.close();		
					socket = null;
				}catch(IOException e){
					e.printStackTrace();
				} finally{
					mServerConnectionListener.onServerDisconnected();
				}
			}
		}
	}

	private void cleanup(){
		synchronized(this){
			if(socket!=null){
				try{
					recvStream.close();
					sendStream.close();
					packetReceiver = null;
					socket.close();
					socket = null;

				}catch(IOException e){
					e.printStackTrace();
				}
			}
		}
	}

	//Send screen state(On or Off)
	public void sendScreenOnOffState(boolean state){
		/*try{
			
			if(state){				
				packetSender.send(new Packet(OpCode.SCREEN_ON_STATE_INFO, null, 0));
			}else{
				packetSender.send(new Packet(OpCode.SCREEN_OFF_STATE_INFO, null, 0));
			}
		}catch(IOException e){
			e.printStackTrace();
		}*/
	}

	public void setClipboardText(Packet packet){
		//String str = new String(packet.getPayload(), 0, packet.getHeader().getPayloadLength()).trim();		
		//mAddOptionListener.setClipBoard(str);
	}

	@Override
	public void onPacketReceived(Packet packet) {
		if(packet.getDeviceType() == PacketHeader.Device_Type.KEYBOARD) {
			if( packet.getUpdownFlag() == PacketHeader.Updown_Flag.UP ) {
				mVirtEventListener.onKeyUp(packet.getKeyCode());
			} else if( packet.getUpdownFlag() == PacketHeader.Updown_Flag.DOWN ) {
				mVirtEventListener.onKeyDown(packet.getKeyCode());
			}
		} else if(packet.getDeviceType() == PacketHeader.Device_Type.MOUSE) {
			if( packet.getUpdownFlag() == PacketHeader.Updown_Flag.UP ) {
				mVirtEventListener.onSetCoordinates(packet.getXCoordinate(), packet.getYCoordinate());
				mVirtEventListener.onTouchUp();
			} else if( packet.getUpdownFlag() == PacketHeader.Updown_Flag.DOWN ) {
				mVirtEventListener.onSetCoordinates(packet.getXCoordinate(), packet.getYCoordinate());
				mVirtEventListener.onTouchDown();
			} else {
				mVirtEventListener.onSetCoordinates(packet.getXCoordinate(), packet.getYCoordinate());
			}
		}
		/*
		switch(packet.get.GetEventCode()){
		case EventPacket.SETCOORDINATES:
			mVirtEventListener.onSetCoordinates(eventPacket.GetXPosition(), eventPacket.GetYPosition());
			break;
		case EventPacket.TOUCHDOWN:
			mVirtEventListener.onSetCoordinates(eventPacket.GetXPosition(), eventPacket.GetYPosition());
			mVirtEventListener.onTouchDown();
			break;
		case EventPacket.TOUCHUP:
			mVirtEventListener.onTouchUp();
			break;
		case EventPacket.BACK:
			mVirtEventListener.onKeyStroke(NativeKeyCode.KEY_BACK);
			break;
		case EventPacket.MENU:
			mVirtEventListener.onKeyStroke(NativeKeyCode.KEY_MENU);
			break;
		case EventPacket.VOLUMEDOWN:
			mVirtEventListener.onKeyStroke(NativeKeyCode.KEY_VOLUMEDOWN);			
			break;
		case EventPacket.VOLUMEUP:
			mVirtEventListener.onKeyStroke(NativeKeyCode.KEY_VOLUMEUP);			
			break;
		case EventPacket.POWER:
			mVirtEventListener.onKeyStroke(NativeKeyCode.KEY_POWER);			
			break;
		case EventPacket.HOME:
			if(Build.VERSION.SDK_INT <= Build.VERSION_CODES.GINGERBREAD_MR1)
				mVirtEventListener.onKeyStroke(NativeKeyCode.KEY_MOVE_HOME);
			else
				mVirtEventListener.onKeyStroke(NativeKeyCode.KEY_HOME);
		}*/
	}

	@Override
	public void onInterrupt() {

		//If server was closed, throw an IOException	
		//If file is open, Shoud be closed

		synchronized(this){
			if(socket!=null){
				try{					
					recvStream.close();
					sendStream.close();
					packetReceiver = null;
					socket.close();
					socket=null;

				}catch(IOException e){
					e.printStackTrace();
				}finally{
					mServerConnectionListener.onServerConnectionInterrupted();				
				}
			}
		}
	}
}

