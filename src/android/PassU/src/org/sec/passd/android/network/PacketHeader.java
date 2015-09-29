package org.sec.passd.android.network;

import org.sec.passd.android.util.Util;

/***
 * This class is parser for PacketHeader ( sendDevice + receiveDevice + deviceType + relativeField + updownFlag ) 
 * @author Jake
 */
public class PacketHeader{
	public static final int LENGTH = 4;
	public static final int MESSAGE_TYPE_LENGTH = 4;
	
	/*
	 * Message Type
	 */
	public class Message_Type{
		public static final int INVALID = -1;
		public static final int KEYBOARD = 1;
		public static final int MOUSE = 2;
		public static final int CLIENT = 3;
		public static final int DATA = 4;		
	}
	
	private int messageType = Message_Type.INVALID;
	
	private static byte[] messageTypeBuffer = new byte[MESSAGE_TYPE_LENGTH];
	private static byte[] headerBuffer = new byte[MESSAGE_TYPE_LENGTH];
	
	public PacketHeader(){
	}
	
	public PacketHeader(int messageType){
		this.messageType = messageType;
	}
	
	@Override
	public String toString(){
		return String.format("%4d", messageType);
	}
	
	/**
	 * Convert packet header into byte array.
	 * @return
	 */
	public byte[] asByteArray(){
		//return toString().getBytes();
		Util.itoa(messageType, messageTypeBuffer, MESSAGE_TYPE_LENGTH, 0);
		return headerBuffer;
	}
	
	public static PacketHeader parse(byte[] rawData){
		PacketHeader header = new PacketHeader();
		
		System.arraycopy(rawData, 0, messageTypeBuffer, 0, MESSAGE_TYPE_LENGTH);
		header.setMessageType(Util.ByteToInt(messageTypeBuffer));
		
		return header;
	}
	

	public int getMessageType() {
		return messageType;
	}

	public void setMessageType(int messageType) {
		this.messageType = messageType;
	}
}
