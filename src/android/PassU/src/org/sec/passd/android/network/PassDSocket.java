package org.sec.passd.android.network;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.Locale;

import org.sec.passd.android.AR;
import org.sec.passd.android.D;
import org.sec.passd.android.natives.KeyCodeMap;
import org.sec.passd.android.natives.WindowsKeyCode;

import android.app.Activity;
import android.content.res.Configuration;
import android.util.Log;
import android.view.KeyEvent;

public class PassDSocket implements PacketListener {
	private final String LOG = "PassDSocket";

	private Socket socket;
	private OutputStream sendStream;
	private InputStream recvStream;
	private PacketReceiver packetReceiver;	

	private boolean isLeftShiftDown = false;
	private boolean isRightShiftDown = false;
	private boolean isLeftCtrlDown = false;
	private boolean isRightCtrlDown = false;
	private int client_id;
	// Event listeners
	private VirtualEventListener mVirtEventListener;
	private ServerConnectionListener mServerConnectionListener;

	private PacketSender packetSender;

	public PassDSocket(ServerConnectionListener listener){
		if(D.D) Log.w(LOG, "PassDSocket");
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

	/**
	 * Connect to specified host.
	 * @param ipAddr ip address
	 * @throws IOException
	 */
	public synchronized boolean connect(String ip, int port){
		if(D.D) Log.w(LOG, "connect");
		try{
			socket = new Socket();
			socket.connect(new InetSocketAddress(ip, port), 2000); // Set timeout to 2 seconds

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
			
			Packet packet = new Packet(
					PacketHeader.Message_Type.CLIENT, 0, 2, 1, 0,
					127, 0, 0, 1, AR.width, AR.height);
			sendEcho(packet.toString().getBytes());
			return true;
		} catch(IOException e) {
			e.printStackTrace();
			mServerConnectionListener.onServerConnectionFailed();
			
			return false;
		}
	}

	/**
	 * Disconnect from host.
	 * @throws IOException
	 */
	public void disconnect(){
		if(D.D) Log.w(LOG, "disconnect");
		Packet packet = new Packet(PacketHeader.Message_Type.CLIENT, client_id, 0, 0,  1,
				127, 0, 0, 1, AR.width, AR.height);
		sendEcho(packet.toString().getBytes());
		
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

	//Send screen state(On or Off)
	public void sendScreenOnOffState(boolean state){
		if(D.D) Log.w(LOG, "sendScreenOnOffState");

	}

	public void setClipboardText(Packet packet){
		if(D.D) Log.w(LOG, "setClipboardText");
		//String str = new String(packet.getPayload(), 0, packet.getHeader().getPayloadLength()).trim();		
		//mAddOptionListener.setClipBoard(str);
	}

	//Send notification to Host
	public void sendEcho(byte[] b){
		if(D.D) Log.w(LOG, "sendEcho");
		try{
			packetSender.send(b);
		}catch(IOException e){
			e.printStackTrace();
		}
	}

	@Override
	public void onPacketReceived(Packet packet) {
		if(D.D) Log.w(LOG, "onPacketReceived");
		Log.w(LOG, packet.toString());
		if( packet.getHeader().getMessageType() == PacketHeader.Message_Type.KEYBOARD ) {
			if( packet.getUpdownFlag() == Packet.Updown_Flag.KEYUP ) {
				Integer keyCode = KeyCodeMap.M.get(packet.getKeyCode());
				
				if (keyCode == KeyEvent.KEYCODE_SHIFT_LEFT) {
					isLeftShiftDown = false;
				}
				
				if (keyCode == KeyEvent.KEYCODE_SHIFT_RIGHT) {
					isRightShiftDown = false;
				}
				
				if (keyCode == KeyEvent.KEYCODE_CTRL_LEFT) {
					isLeftCtrlDown = false;
				}
				
				if (keyCode == KeyEvent.KEYCODE_CTRL_RIGHT) {
					isRightCtrlDown = false;
				}
				
				if(keyCode != null)
					mVirtEventListener.onKeyUp(keyCode);
			} else if( packet.getUpdownFlag() == Packet.Updown_Flag.KEYDOWN ) {
				Integer keyCode = KeyCodeMap.M.get(packet.getKeyCode());
				
				if (keyCode == KeyEvent.KEYCODE_SHIFT_LEFT) {
					if (isLeftShiftDown) {
						return;
					} else {
						isLeftShiftDown = true;
					}
				}
				
				if (keyCode == KeyEvent.KEYCODE_SHIFT_RIGHT) {
					if (isRightShiftDown) {
						return;
					} else {
						isRightShiftDown = true;
					}
				}
				
				if (keyCode == KeyEvent.KEYCODE_CTRL_LEFT) {
					if (isLeftCtrlDown) {
						return;
					} else {
						isLeftCtrlDown = true;
					}
				}
				
				if (keyCode == KeyEvent.KEYCODE_CTRL_RIGHT) {
					if (isRightCtrlDown) {
						return;
					} else {
						isRightCtrlDown = true;
					}
				}
				
				if(keyCode != null)
					mVirtEventListener.onKeyDown(keyCode);
			}
		} else if( packet.getHeader().getMessageType() == PacketHeader.Message_Type.MOUSE ) {
			mVirtEventListener.onSetCoordinates(packet.getXCoordinate(), packet.getYCoordinate());
			AR.getInstance().m_Service.Update(packet.getXCoordinate(), packet.getYCoordinate(), true);
			
			if(packet.getXCoordinate() <= 3 ||
					packet.getYCoordinate() <= 3 ||
					packet.getXCoordinate() >= AR.width - 3 ||
					packet.getYCoordinate() >= AR.height - 3) {
				AR.getInstance().m_Service.HideCursor();
			} else {
				AR.getInstance().m_Service.ShowCursor();
			}
			
			if( packet.getLeftRight() == Packet.LeftRight.LEFT ) {
				if( packet.getUpdownFlag() == Packet.Updown_Flag.UP ) {
					mVirtEventListener.onTouchUp();
				} else if( packet.getUpdownFlag() == Packet.Updown_Flag.DOWN ) {
					mVirtEventListener.onTouchDown();
				} else {
					
				}
			} else if( packet.getLeftRight() == Packet.LeftRight.RIGHT ) {
				if( packet.getUpdownFlag() == Packet.Updown_Flag.DOWN ) {
					mVirtEventListener.onKeyDown(KeyEvent.KEYCODE_BACK);
					mVirtEventListener.onKeyUp(KeyEvent.KEYCODE_BACK);
				} 
			}
		} else if( packet.getHeader().getMessageType() == PacketHeader.Message_Type.CLIENT ) {
		}
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

