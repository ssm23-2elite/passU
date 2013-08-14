package org.secmem232.passu.android.network;

import org.secmem232.passu.android.util.Util;

/***
 * This class is parser for PacketHeader ( sendDevice + receiveDevice + deviceType + relativeField + updownFlag ) 
 * @author Jake
 */
public class PacketHeader{
	public static final int LENGTH = 11;
	public static final int SEND_DEVICE_LENGTH = 4;
	public static final int RECEIVE_DEVICE_LENGTH = 4;
	public static final int DEVICE_TYPE_LENGTH = 1;
	public static final int RELATIVE_FIELD_LENGTH = 1;
	public static final int UPDOWN_FLAG_LENGTH = 1;
	
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
		public static final int UP = 1;
		public static final int DOWN = 0;
	}
	
	private int sendDevice = Device_Location.INVALID;
	private int receiveDevice = Device_Location.INVALID;
	private int deviceType = Device_Type.INVALID;
	private int relativeField = Relative_Field.INVALID;
	private int updownFlag = Updown_Flag.INVALID;
	
	private static byte[] sendDeviceBuffer = new byte[SEND_DEVICE_LENGTH];
	private static byte[] receiveDeviceBuffer = new byte[RECEIVE_DEVICE_LENGTH];
	private static byte[] deviceTypeBuffer = new byte[DEVICE_TYPE_LENGTH];
	private static byte[] relativeFieldBuffer = new byte[RELATIVE_FIELD_LENGTH];
	private static byte[] updownFlagBuffer = new byte[UPDOWN_FLAG_LENGTH];
	
	private static byte[] headerBuffer = new byte[SEND_DEVICE_LENGTH+RECEIVE_DEVICE_LENGTH+
	                                              DEVICE_TYPE_LENGTH+RELATIVE_FIELD_LENGTH + UPDOWN_FLAG_LENGTH];
	
	private PacketHeader(){
	}
	
	public PacketHeader(int sendDevice, int receiveDevice, int deviceType, int relativeField, int updownFlag){
		this.sendDevice = sendDevice;
		this.receiveDevice = receiveDevice;
		this.deviceType = deviceType;
		this.relativeField = relativeField;
		this.updownFlag = updownFlag;
	}
	
	@Override
	public String toString(){
		return String.format("%4d%4d%1d%1d%1d", sendDevice, receiveDevice, deviceType, relativeField, updownFlag);
	}
	
	/**
	 * Convert packet header into byte array.
	 * @return
	 */
	public byte[] asByteArray(){
		//return toString().getBytes();
		Util.itoa(sendDevice, headerBuffer, SEND_DEVICE_LENGTH, 0);
		Util.itoa(receiveDevice, headerBuffer, RECEIVE_DEVICE_LENGTH, SEND_DEVICE_LENGTH);
		Util.itoa(deviceType, headerBuffer, DEVICE_TYPE_LENGTH, SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH);
		Util.itoa(relativeField, headerBuffer, RELATIVE_FIELD_LENGTH, SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH
				+ DEVICE_TYPE_LENGTH);
		Util.itoa(updownFlag, headerBuffer, UPDOWN_FLAG_LENGTH, SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH 
				+ DEVICE_TYPE_LENGTH + RECEIVE_DEVICE_LENGTH );
		return headerBuffer;
	}
	
	public static PacketHeader parse(byte[] rawData){
		PacketHeader header = new PacketHeader();
		
		System.arraycopy(rawData, 0, sendDeviceBuffer, 0, SEND_DEVICE_LENGTH);
		header.setSendDevice(Util.ByteToInt(sendDeviceBuffer));
		
		System.arraycopy(rawData, SEND_DEVICE_LENGTH, receiveDeviceBuffer, 0, RECEIVE_DEVICE_LENGTH);				
		header.setReceiveDevice(Util.ByteToInt(receiveDeviceBuffer));
		
		System.arraycopy(rawData, SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH, deviceTypeBuffer, 0, DEVICE_TYPE_LENGTH);				
		header.setDeviceType(Util.ByteToInt(deviceTypeBuffer));
		
		System.arraycopy(rawData, SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH, relativeFieldBuffer, 0,  RELATIVE_FIELD_LENGTH);				
		header.setRelativeField(Util.ByteToInt(relativeFieldBuffer));

		System.arraycopy(rawData, SEND_DEVICE_LENGTH + RECEIVE_DEVICE_LENGTH + DEVICE_TYPE_LENGTH + RELATIVE_FIELD_LENGTH, 
				updownFlagBuffer, 0,  UPDOWN_FLAG_LENGTH);				
		header.setUpdownFlag(Util.ByteToInt(updownFlagBuffer));
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
}
