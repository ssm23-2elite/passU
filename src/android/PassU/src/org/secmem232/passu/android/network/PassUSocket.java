package org.secmem232.passu.android.network;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

import org.secmem232.passu.android.natives.NativeKeyCode;
import org.secmem232.passu.android.network.PacketHeader.OpCode;

import android.os.Build;
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
					socket=null;

				}catch(IOException e){
					e.printStackTrace();
				}
			}
		}
	}

	//Send screen state(On or Off)
	public void sendScreenOnOffState(boolean state){
		try{
			if(state){				
				packetSender.send(new Packet(OpCode.SCREEN_ON_STATE_INFO, null, 0));
			}else{
				packetSender.send(new Packet(OpCode.SCREEN_OFF_STATE_INFO, null, 0));
			}
		}catch(IOException e){
			e.printStackTrace();
		}
	}

	public void setClipboardText(Packet packet){
		String str = new String(packet.getPayload(), 0, packet.getHeader().getPayloadLength()).trim();		
		mAddOptionListener.setClipBoard(str);
	}

	@Override
	public void onPacketReceived(Packet packet) {
		switch(packet.getOpcode()){		
		case OpCode.EVENT_RECEIVED:			
			parseVirtualEventPacket(packet);
			break;	
		case OpCode.SET_TO_CLIPBOARD:
			setClipboardText(packet);
			break;
		}
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

	private void parseVirtualEventPacket(Packet packet){
		EventPacket eventPacket = EventPacket.parse(packet);

		switch(eventPacket.GetEventCode()){
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
		case EventPacket.KEYDOWN:
			mVirtEventListener.onKeyDown(eventPacket.GetKeyCode());
			break;
		case EventPacket.KEYUP:
			mVirtEventListener.onKeyUp(eventPacket.GetKeyCode());
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
		}
	}
}

