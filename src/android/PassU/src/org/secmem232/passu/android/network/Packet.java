package org.secmem232.passu.android.network;

import org.secmem232.passu.android.util.Util;

/**
 * This class is parser for Packet ( PacketHeader + (keyCode, wheelFlag + xCoordinate + yCoordinate) ) 
 * @see PacketHeader
 * @author Jake
 */
public class Packet {

	public static final int LENGTH = 24;
	public static final int PAYLOAD_LENGTH = 13;
	public static final int KEY_CODE_LENGTH = 4;
	public static final int WHEEL_FLAG_LENGTH = 1;
	public static final int X_COORDINATE_LENGTH = 4;
	public static final int Y_COORDINATE_LENGTH = 4;
	
	private PacketHeader header;
	private int keyCode;
	private int wheelFlag;
	private int xCoordinate;
	private int yCoordinate;
	
	private static byte[] packetBuffer = new byte[LENGTH];
	private static byte[] keyCodeBuffer = new byte[KEY_CODE_LENGTH];
	private static byte[] wheelFlagBuffer = new byte[WHEEL_FLAG_LENGTH];
	private static byte[] xCoordinateBuffer = new byte[X_COORDINATE_LENGTH];
	private static byte[] yCoordinateBuffer = new byte[Y_COORDINATE_LENGTH];
	
	protected Packet(){
		// Do nothing on here
	}
	
	public Packet(int sendDevice, int receiveDevice, int deviceType,
			int relativeField, int updownFlag, int keyCode){
		header = new PacketHeader(sendDevice, receiveDevice,
				0, relativeField, updownFlag);
		this.keyCode = keyCode;
	}
	
	public Packet(int sendDevice, int receiveDevice, int deviceType,
			int relativeField, int updownFlag, int wheelFlag, int xCoordinate, int yCoordinate){
		header = new PacketHeader(sendDevice, receiveDevice,
				1, relativeField, updownFlag);
		this.wheelFlag = wheelFlag;
		this.xCoordinate = xCoordinate;
		this.yCoordinate = yCoordinate;
	}
	
	public static Packet parse(byte[] rawPacket){
		Packet packet = new Packet();		
		
		// Get header
		packet.setHeader(PacketHeader.parse(rawPacket));		
		
		if(packet.header.getDeviceType() == PacketHeader.Device_Type.KEYBOARD) {
			System.arraycopy(rawPacket, PacketHeader.LENGTH, keyCodeBuffer, 0, KEY_CODE_LENGTH);
			packet.setKeyCode(Util.ByteToInt(keyCodeBuffer));
		} else if (packet.header.getDeviceType() == PacketHeader.Device_Type.MOUSE) {
			System.arraycopy(rawPacket, PacketHeader.LENGTH, wheelFlagBuffer, 0, WHEEL_FLAG_LENGTH);
			packet.setWheelFlag(Util.ByteToInt(wheelFlagBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + WHEEL_FLAG_LENGTH, xCoordinateBuffer, 0, X_COORDINATE_LENGTH);
			packet.setXCoordinate(Util.ByteToInt(xCoordinateBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + WHEEL_FLAG_LENGTH + X_COORDINATE_LENGTH, yCoordinateBuffer, 0, Y_COORDINATE_LENGTH);
			packet.setYCoordinate(Util.ByteToInt(yCoordinateBuffer));
		}
		// Packet parsing has done.
		return packet;
	}
	
	public String toString() {	
		if(getHeader().getDeviceType() == PacketHeader.Device_Type.KEYBOARD) {
			return String.format("%s[%4d]", getHeader(), keyCode);
		} else if (getHeader().getDeviceType() == PacketHeader.Device_Type.MOUSE) {
			return String.format("%s[%1d][%4d][%4d]", getHeader(), wheelFlag, xCoordinate, yCoordinate);
		} else {
			return String.format("UNKNOWN");
		}
	}
	/**
	 * Get packet as byte array.
	 * @return
	 */
	public byte[] asByteArray(){		
		
		if(header == null)
			throw new IllegalStateException("Packet header has not been set.");
				
		// Append header
		byte[] headerData = header.asByteArray();
		System.arraycopy(headerData, 0, packetBuffer, 0, PacketHeader.LENGTH);
		
		if(header.getDeviceType() == PacketHeader.Device_Type.KEYBOARD) {
			if(keyCodeBuffer != null)
				System.arraycopy(keyCodeBuffer, 0, packetBuffer, PacketHeader.LENGTH, keyCodeBuffer.length);
		} else if(header.getDeviceType() == PacketHeader.Device_Type.MOUSE) {
			if(wheelFlagBuffer != null && xCoordinateBuffer != null && yCoordinateBuffer != null) {
				System.arraycopy(wheelFlagBuffer, 0, packetBuffer, PacketHeader.LENGTH, wheelFlagBuffer.length);
				System.arraycopy(xCoordinateBuffer, 0, packetBuffer, PacketHeader.LENGTH + WHEEL_FLAG_LENGTH, xCoordinateBuffer.length);
				System.arraycopy(yCoordinateBuffer, 0, packetBuffer, PacketHeader.LENGTH + WHEEL_FLAG_LENGTH + X_COORDINATE_LENGTH, yCoordinateBuffer.length);
			}
		}
		return packetBuffer;
	}

	public PacketHeader getHeader() {
		return header;
	}

	public void setHeader(PacketHeader mHeader) {
		this.header = mHeader;
	}

	public int getKeyCode() {
		return keyCode;
	}

	public void setKeyCode(int keyCode) {
		this.keyCode = keyCode;
	}
	
	public int getWheelFlag() {
		return wheelFlag;
	}

	public void setWheelFlag(int wheelFlag) {
		this.wheelFlag = wheelFlag;
	}
	
	public int getXCoordinate() {
		return xCoordinate;
	}

	public void setXCoordinate(int xCoordinate) {
		this.xCoordinate = xCoordinate;
	}
	
	public int getYCoordinate() {
		return yCoordinate;
	}

	public void setYCoordinate(int yCoordinate) {
		this.yCoordinate = yCoordinate;
	}
	
	public int getDeviceType(){
		return header.getDeviceType();
	}
	
	public int getUpdownFlag(){
		return header.getUpdownFlag();
	}
}
