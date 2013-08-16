package org.secmem232.passu.android.network;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

import org.secmem232.passu.android.AR;
import org.secmem232.passu.android.D;

import android.util.Log;

public class PassUSocket implements PacketListener {
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
		if(D.D) Log.w(LOG, "PassUSocket");
		mServerConnectionListener = listener;
	}

	/**
	 * Check whether connected to server or not.
	 * @return true if connected to server, false otherwise
	 */
	public boolean isConnected(){
		if(D.D) Log.w(LOG, "isConnected");
		return socket != null ? socket.isConnected() : false;		
	}

	public void setVirtualEventListener(VirtualEventListener listener){
		if(D.D) Log.w(LOG, "setVirtualEventListener");
		mVirtEventListener = listener;
	}

	public void setAddOptionListener(AddOptionListener listner) {
		if(D.D) Log.w(LOG, "setAddOptionListener");
		mAddOptionListener = listner;
	}


	/**
	 * Connect to specified host.
	 * @param ipAddr ip address
	 * @throws IOException
	 */
	public synchronized void connect(String ip, int port){
		if(D.D) Log.w(LOG, "connect");
		try{
			socket = new Socket();
			socket.connect(new InetSocketAddress(ip, port), 5000); // Set timeout to 5 seconds

			// Open outputStream
			sendStream = socket.getOutputStream();
			packetSender = new PacketSender(sendStream);
			
			// Open inputStream
			recvStream = socket.getInputStream();		
						
			// Create and start packet receiver
			packetReceiver = new PacketReceiver(recvStream);
			packetReceiver.setPacketListener(this);
			packetReceiver.start();	

			mServerConnectionListener.onServerConnected(ip, port);
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
		if(D.D) Log.w(LOG, "disconnect");
		synchronized(this){
			if(socket != null){
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
		if(D.D) Log.w(LOG, "cleanup");
		synchronized(this){
			if(socket != null){
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
		if(D.D) Log.w(LOG, "sendScreenOnOffState");
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
		if(D.D) Log.w(LOG, "setClipboardText");
		//String str = new String(packet.getPayload(), 0, packet.getHeader().getPayloadLength()).trim();		
		//mAddOptionListener.setClipBoard(str);
	}

	//Send notification to Host
	public void sendEcho(Packet packet){
		if(D.D) Log.w(LOG, "sendEcho");
		try{
			packetSender.send(packet);
		}catch(IOException e){
			e.printStackTrace();
		}
	}

	@Override
	public void onPacketReceived(Packet packet) {
		if(D.D) Log.w(LOG, "onPacketReceived");
		Log.w(LOG, packet.toString());
		if( packet.getDeviceType() == PacketHeader.Device_Type.KEYBOARD ) {
			if( packet.getUpdownFlag() == PacketHeader.Updown_Flag.UP ) {
				mVirtEventListener.onKeyUp(packet.getKeyCode());
			} else if( packet.getUpdownFlag() == PacketHeader.Updown_Flag.DOWN ) {
				mVirtEventListener.onKeyDown(packet.getKeyCode());
			}
		} else if( packet.getDeviceType() == PacketHeader.Device_Type.MOUSE ) {
			if( packet.getUpdownFlag() == PacketHeader.Updown_Flag.UP ) {
				AR.getInstance().m_Service.Update(packet.getXCoordinate(), packet.getYCoordinate(), true);
				mVirtEventListener.onSetCoordinates(packet.getXCoordinate(), packet.getYCoordinate());
				mVirtEventListener.onTouchUp();
			} else if( packet.getUpdownFlag() == PacketHeader.Updown_Flag.DOWN ) {
				AR.getInstance().m_Service.Update(packet.getXCoordinate(), packet.getYCoordinate(), true);
				mVirtEventListener.onSetCoordinates(packet.getXCoordinate(), packet.getYCoordinate());
				mVirtEventListener.onTouchDown();
			} else {
				AR.getInstance().m_Service.Update(packet.getXCoordinate(), packet.getYCoordinate(), true);
				mVirtEventListener.onSetCoordinates(packet.getXCoordinate(), packet.getYCoordinate());
			}
		}
		sendEcho(packet);

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
		if(D.D) Log.w(LOG, "onInterrupt");
		mServerConnectionListener.onServerConnectionInterrupted();	
		synchronized(this){
			if(socket != null){
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
}

