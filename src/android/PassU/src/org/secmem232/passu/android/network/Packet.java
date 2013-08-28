package org.secmem232.passu.android.network;

import org.secmem232.passu.android.AR;
import org.secmem232.passu.android.util.Util;

import android.util.Log;

/**
 * This class is parser for Packet ( PacketHeader + (keyCode, wheelFlag + xCoordinate + yCoordinate) ) 
 * @see PacketHeader
 * @author Jake
 */
public class Packet {

	public static final int LENGTH = 28;
	public static final int PAYLOAD_LENGTH = 24;
	public static final int SEND_DEVICE_LENGTH = 4;
	public static final int RECEIVE_DEVICE_LENGTH = 4;
	public static final int DEVICE_TYPE_LENGTH = 1;
	public static final int RELATIVE_FIELD_LENGTH = 1;
	public static final int UPDOWN_FLAG_LENGTH = 1;
	public static final int PAD_LENGTH = 1;
	public static final int KEY_CODE_LENGTH = 4;
	
	public static final int LEFTRIGHT_LENGTH = 1;
	public static final int WHEEL_FLAG_LENGTH = 4;
	public static final int X_COORDINATE_LENGTH = 4;
	public static final int Y_COORDINATE_LENGTH = 4;
	
	public static final int C_ID_LENGTH = 4;
	public static final int PAD3_LENGTH = 4;
	public static final int HELLO_LENGTH = 1;
	public static final int BYE_LENGTH = 1;
	
	public static final int LEN_LENGTH = 4;
		
	/*
	 * location of send & receive devices 
	 */
	public class Device_Location{
		public static final int INVALID = -1;
		public static final int TOP_LEFT = 1;
		public static final int TOP_CENTER = 2;
		public static final int TOP_RIGHT = 3;
		public static final int LEFT = 4;
		public static final int CENTER = 5;
		public static final int RIGHT = 6;
		public static final int BOTTOM_LEFT = 7;
		public static final int BOTTOM_CENTER = 8;
		public static final int BOTTOM_RIGHT = 9;		
	}
	
	/*
	 * type of device
	 */
	public class Device_Type{
		public static final int INVALID = -1;
		public static final int KEYBOARD = 0;
		public static final int MOUSE = 1;
	}
	
	/*
	 * 다른 화면으로 전환될 당시의 패킷이라면 RELATIVE = 1
	 * 평상시에는 RELATIVE = 0 
	 */
	public class Relative_Field{
		public static final int INVALID = -1;
		public static final int IS_NOT_RELATIVE = 0;
		public static final int IS_RELATIVE = 1;
	}
	
	public class Updown_Flag{
		public static final int INVALID = -1;
		public static final int UP = 0;
		public static final int DOWN = 1;
	}
	
	public class LeftRight{
		public static final int INVALID = -1;
		public static final int LEFT = 0;
		public static final int RIGHT = 1;
	}
	
	public class Wheel_Flag{
		public static final int INVALID = -1;
		public static final int WHEEL_OFF = 0;
		public static final int WHEEL_ON = 1;
		public static final int WHEEL_MOVE = 2;
	}
	
	public class Hello{
		public static final int HELLO_OFF = 0;
		public static final int HELLO_ON = 1;
	}
	
	public class Bye{
		public static final int BYE_OFF = 0;
		public static final int BYE_ON = 1;
	}
	
	private PacketHeader header;
	private int sendDevice = Device_Location.INVALID;
	private int receiveDevice = Device_Location.INVALID;
	private int deviceType = Device_Type.INVALID;
	private int relativeField = Relative_Field.INVALID;
	private int updownFlag = Updown_Flag.INVALID;
	private int pad;
	private int keyCode;
	private int leftright = LeftRight.INVALID;
	private int wheelFlag = Wheel_Flag.INVALID;
	private int xCoordinate;
	private int yCoordinate;
	private int c_id;
	private int pad3;
	private int hello;
	private int bye;
	private int len;
	
	private static byte[] packetBuffer = new byte[LENGTH];
	
	private static byte[] sendDeviceBuffer = new byte[SEND_DEVICE_LENGTH];
	private static byte[] receiveDeviceBuffer = new byte[RECEIVE_DEVICE_LENGTH];
	private static byte[] deviceTypeBuffer = new byte[DEVICE_TYPE_LENGTH];
	private static byte[] relativeFieldBuffer = new byte[RELATIVE_FIELD_LENGTH];
	private static byte[] updownFlagBuffer = new byte[UPDOWN_FLAG_LENGTH];
	private static byte[] padBuffer = new byte[PAD_LENGTH];
	private static byte[] keyCodeBuffer = new byte[KEY_CODE_LENGTH];
	private static byte[] leftrightBuffer = new byte[LEFTRIGHT_LENGTH];
	private static byte[] wheelFlagBuffer = new byte[WHEEL_FLAG_LENGTH];
	private static byte[] xCoordinateBuffer = new byte[X_COORDINATE_LENGTH];
	private static byte[] yCoordinateBuffer = new byte[Y_COORDINATE_LENGTH];
	private static byte[] cidBuffer = new byte[C_ID_LENGTH];
	private static byte[] pad3Buffer = new byte[PAD3_LENGTH];
	private static byte[] helloBuffer = new byte[HELLO_LENGTH];
	private static byte[] byeBuffer = new byte[BYE_LENGTH];
	
	protected Packet(){
		// Do nothing on here
	}
	
	public Packet(int messageType, int sendDevice, int receiveDevice, int deviceType,
			int relativeField, int updownFlag, int keyCode){
		header = new PacketHeader(messageType);
		
		this.sendDevice = sendDevice;
		this.receiveDevice = receiveDevice;
		this.deviceType = deviceType;
		this.relativeField = relativeField;
		this.updownFlag = updownFlag;
		this.pad = 0;
		this.keyCode = keyCode;
	}
	
	public Packet(int messageType, int sendDevice, int receiveDevice, int deviceType,
			int relativeField, int updownFlag, int leftright, int wheelflag, int xCoordinate, int yCoordinate){
		header = new PacketHeader(messageType);
		
		this.sendDevice = sendDevice;
		this.receiveDevice = receiveDevice;
		this.deviceType = deviceType;
		this.relativeField = relativeField;
		this.updownFlag = updownFlag;
		this.leftright = leftright;
		this.wheelFlag = wheelflag;
		this.xCoordinate = xCoordinate;
		this.yCoordinate = yCoordinate;
	}
	
	
	public Packet(int messageType, int c_id, int hello, int bye){
		header = new PacketHeader(messageType);
		
		this.c_id = c_id;
		this.pad3 = 0;
		this.hello = hello;
		this.bye = bye;
	}
	
	public static Packet parse(byte[] rawPacket){
		Log.w("parse", rawPacket.toString());
		Packet packet = new Packet();		
		
		// Get header
		packet.setHeader(PacketHeader.parse(rawPacket));		
		
		if(packet.header.getMessageType() == PacketHeader.Message_Type.KEYBOARD) {		
			System.arraycopy(rawPacket, PacketHeader.LENGTH, sendDeviceBuffer, 0, SEND_DEVICE_LENGTH);
			packet.setSendDevice(Util.ByteToInt(sendDeviceBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH, receiveDeviceBuffer, 0, RECEIVE_DEVICE_LENGTH);
			packet.setReceiveDevice(Util.ByteToInt(receiveDeviceBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH, 
					deviceTypeBuffer, 0, DEVICE_TYPE_LENGTH);
			packet.setDeviceType(Util.ByteToInt(deviceTypeBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH, 
					relativeFieldBuffer, 0, RELATIVE_FIELD_LENGTH);
			packet.setRelativeField(Util.ByteToInt(relativeFieldBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH, 
					updownFlagBuffer, 0, UPDOWN_FLAG_LENGTH);
			packet.setUpdownFlag(Util.ByteToInt(updownFlagBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH
					+ UPDOWN_FLAG_LENGTH, 
					padBuffer, 0, PAD_LENGTH);
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH
					+ UPDOWN_FLAG_LENGTH + PAD_LENGTH, 
					keyCodeBuffer, 0, KEY_CODE_LENGTH);
			packet.setKeyCode(Util.ByteToInt(keyCodeBuffer));

		} else if (packet.header.getMessageType() == PacketHeader.Message_Type.MOUSE) {
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH, sendDeviceBuffer, 0, SEND_DEVICE_LENGTH);
			packet.setSendDevice(Util.ByteToInt(sendDeviceBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH, receiveDeviceBuffer, 0, RECEIVE_DEVICE_LENGTH);
			packet.setReceiveDevice(Util.ByteToInt(receiveDeviceBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH, 
					deviceTypeBuffer, 0, DEVICE_TYPE_LENGTH);
			packet.setDeviceType(Util.ByteToInt(deviceTypeBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH, 
					relativeFieldBuffer, 0, RELATIVE_FIELD_LENGTH);
			packet.setRelativeField(Util.ByteToInt(relativeFieldBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH, 
					updownFlagBuffer, 0, UPDOWN_FLAG_LENGTH);
			packet.setUpdownFlag(Util.ByteToInt(updownFlagBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH
					+ UPDOWN_FLAG_LENGTH, 
					leftrightBuffer, 0, LEFTRIGHT_LENGTH);
			packet.setLeftRight(Util.ByteToInt(leftrightBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH
					+ UPDOWN_FLAG_LENGTH + LEFTRIGHT_LENGTH, 
					wheelFlagBuffer, 0, WHEEL_FLAG_LENGTH);
			packet.setWheelFlag(Util.ByteToInt(wheelFlagBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH
					+ UPDOWN_FLAG_LENGTH + LEFTRIGHT_LENGTH, 
					wheelFlagBuffer, 0, WHEEL_FLAG_LENGTH);
			packet.setWheelFlag(Util.ByteToInt(wheelFlagBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH
					+ UPDOWN_FLAG_LENGTH + LEFTRIGHT_LENGTH + WHEEL_FLAG_LENGTH, 
					xCoordinateBuffer, 0, X_COORDINATE_LENGTH);
			packet.setXCoordinate(Util.ByteToInt(xCoordinateBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH
					+ UPDOWN_FLAG_LENGTH + LEFTRIGHT_LENGTH + WHEEL_FLAG_LENGTH + X_COORDINATE_LENGTH, 
					yCoordinateBuffer, 0, Y_COORDINATE_LENGTH);
			packet.setYCoordinate(Util.ByteToInt(yCoordinateBuffer));
			
		} else if (packet.header.getMessageType() == PacketHeader.Message_Type.CLIENT) {
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH, cidBuffer, 0, C_ID_LENGTH);
			packet.setCId(Util.ByteToInt(cidBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + C_ID_LENGTH, pad3Buffer, 0, PAD3_LENGTH);
			packet.setHello(Util.ByteToInt(pad3Buffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + C_ID_LENGTH + PAD3_LENGTH, helloBuffer, 0, HELLO_LENGTH);
			packet.setHello(Util.ByteToInt(helloBuffer));
			
			System.arraycopy(rawPacket, PacketHeader.LENGTH + C_ID_LENGTH + PAD3_LENGTH + HELLO_LENGTH, 
					byeBuffer, 0, BYE_LENGTH);
			packet.setBye(Util.ByteToInt(byeBuffer));
			
		}
		// Packet parsing has done.
		return packet;
	}
	
	
	public String toString() {	
		if(getHeader().getMessageType() == PacketHeader.Message_Type.KEYBOARD) {
			return String.format("%s%4d%4d%1d%1d%1d%1d%4d", getHeader(), 
					sendDevice, receiveDevice, deviceType, relativeField, updownFlag, pad, keyCode );
		} else if (getHeader().getMessageType() == PacketHeader.Message_Type.MOUSE) {
			return String.format("%s%4d%4d%1d%1d%1d%1d%4d%4d%4d", getHeader(), 
					sendDevice, receiveDevice, deviceType, relativeField, updownFlag, leftright, wheelFlag, xCoordinate, yCoordinate );
		} else if (getHeader().getMessageType() == PacketHeader.Message_Type.CLIENT){
			return String.format("%s%4d%4d%1d%1d", getHeader(), c_id, pad3, hello, bye);
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
		
		if(getHeader().getMessageType() == PacketHeader.Message_Type.KEYBOARD) {
			System.arraycopy(sendDeviceBuffer, 0, packetBuffer, PacketHeader.LENGTH, sendDeviceBuffer.length);
			System.arraycopy(receiveDeviceBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH, receiveDeviceBuffer.length);
			System.arraycopy(deviceTypeBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH, deviceTypeBuffer.length);
			System.arraycopy(relativeFieldBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH +
					DEVICE_TYPE_LENGTH, relativeFieldBuffer.length);
			System.arraycopy(updownFlagBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH +
					DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH, updownFlagBuffer.length);
			System.arraycopy(padBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH +
					DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH + UPDOWN_FLAG_LENGTH, padBuffer.length);
			System.arraycopy(keyCodeBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH +
					DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH + UPDOWN_FLAG_LENGTH + PAD_LENGTH, keyCodeBuffer.length);
		} else if(getHeader().getMessageType() == PacketHeader.Message_Type.MOUSE) {
			System.arraycopy(sendDeviceBuffer, 0, packetBuffer, PacketHeader.LENGTH, sendDeviceBuffer.length);
			System.arraycopy(receiveDeviceBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH, receiveDeviceBuffer.length);
			System.arraycopy(deviceTypeBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH, deviceTypeBuffer.length);
			System.arraycopy(relativeFieldBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH +
					DEVICE_TYPE_LENGTH, relativeFieldBuffer.length);
			System.arraycopy(updownFlagBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH +
					DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH, updownFlagBuffer.length);
			System.arraycopy(leftrightBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH +
					DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH + UPDOWN_FLAG_LENGTH, leftrightBuffer.length);
			System.arraycopy(wheelFlagBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH +
					DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH + UPDOWN_FLAG_LENGTH + LEFTRIGHT_LENGTH, wheelFlagBuffer.length);
			System.arraycopy(xCoordinateBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH +
					DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH + UPDOWN_FLAG_LENGTH + LEFTRIGHT_LENGTH + WHEEL_FLAG_LENGTH, xCoordinateBuffer.length);
			System.arraycopy(yCoordinateBuffer, 0, packetBuffer, PacketHeader.LENGTH + SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH +
					DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH + UPDOWN_FLAG_LENGTH + LEFTRIGHT_LENGTH + WHEEL_FLAG_LENGTH + X_COORDINATE_LENGTH, yCoordinateBuffer.length);
		} else if(getHeader().getMessageType() == PacketHeader.Message_Type.CLIENT) {
			System.arraycopy(cidBuffer, 0, packetBuffer, PacketHeader.LENGTH, cidBuffer.length);
			System.arraycopy(pad3Buffer, 0, packetBuffer, PacketHeader.LENGTH + C_ID_LENGTH, pad3Buffer.length);
			System.arraycopy(helloBuffer, 0, packetBuffer, PacketHeader.LENGTH + C_ID_LENGTH + PAD3_LENGTH, helloBuffer.length);
			System.arraycopy(byeBuffer, 0, packetBuffer, PacketHeader.LENGTH + C_ID_LENGTH + PAD3_LENGTH + HELLO_LENGTH, byeBuffer.length);
			
		}
		return packetBuffer;
	}

	public PacketHeader getHeader() {
		return header;
	}


	public int getSendDevice() {
		return sendDevice;
	}

	public void setSendDevice(int sendDevice) {
		this.sendDevice = sendDevice;
	}

	public int getReceiveDevice() {
		return receiveDevice;
	}

	public void setReceiveDevice(int receiveDevice) {
		this.receiveDevice = receiveDevice;
	}

	public int getDeviceType() {
		return deviceType;
	}
	
	public void setDeviceType(int deviceType) {
		this.deviceType = deviceType;
	}
	
	public int getRelativeField() {
		return relativeField;
	}
	
	public void setRelativeField(int relativeField) {
		this.relativeField = relativeField;
	}
	
	public int getUpdownFlag() {
		return updownFlag;
	}
	
	public void setUpdownFlag(int updownFlag) {
		this.updownFlag = updownFlag;
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
	
	public int getLeftRight() {
		return leftright;
	}

	public void setLeftRight(int leftright) {
		this.leftright = leftright;
	}
	
	public int getWheelFlag() {
		return wheelFlag;
	}

	public void setWheelFlag(int wheelFlag) {
		this.wheelFlag = wheelFlag;
	}
	
	public int getXCoordinate() {
		return (xCoordinate > AR.width) ? AR.width : xCoordinate;
	}

	public void setXCoordinate(int xCoordinate) {
		this.xCoordinate = xCoordinate;
	}
	
	public int getYCoordinate() {
		return (yCoordinate > AR.height) ? AR.height : yCoordinate;
	}

	public void setYCoordinate(int yCoordinate) {
		this.yCoordinate = yCoordinate;
	}
	public int getCId() {
		return c_id;
	}

	public void setCId(int c_id) {
		this.c_id = c_id;
	}
	
	public int getHello() {
		return hello;
	}

	public void setHello(int hello) {
		this.hello = hello;
	}
	
	public int getBye() {
		return bye;
	}

	public void setBye(int bye) {
		this.bye = bye;
	}
}
