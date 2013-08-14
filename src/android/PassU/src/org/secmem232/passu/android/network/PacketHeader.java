package org.secmem232.passu.android.network;

import java.text.ParseException;

import org.secmem232.passu.android.util.CommonUtil;

/**
 * Represents packet's header data.</br>
 * Header is consisted of <code>{@link OpCode opCode}</code> and {@link #payloadLength PayloadLength}.
 * @author Taeho Kim
 *
 */
public class PacketHeader{
	
	public static final int LENGTH = 6;
	public static final int OPCODE_LENGTH = 2;
	public static final int PAYLOAD_LENGTH = 4;
	
	
	/**
	 * An operation code for {@link PacketHeader}.
	 * @author Taeho Kim
	 *
	 */
	public class OpCode{
		public static final int INVALID = -1;
		public static final int FILEINFO_RECEIVED = 1;
		public static final int FILEDATA_RECEIVED = 2;
		public static final int JPGINFO_SEND = 3;
		public static final int JPGDATA_SEND = 4;
		public static final int FILEDATA_REQUESTED = 5;
		public static final int READY_TO_SEND = 6;
		public static final int FILEINFO_REQUESTED = 7;
		public static final int EVENT_RECEIVED = 8;		
		public static final int DEVICEINFO_SEND = 9;
		public static final int NOTIFICATION_SEND = 10;
		public static final int SCREEN_SEND_REQUESTED = 11;
		public static final int SCREEN_STOP_REQUESTED = 12;
		public static final int OPTION_START_EXPLORER = 13;
		public static final int OPTION_SEND_KAKAOTALK = 14;
		public static final int NOTIFICATION_KAKAOTALK_SEND = 15;
		public static final int FILETRANSFER_COMPLETE = 16;
		public static final int FILETRANSFER_STOP_REQUESTED = 17;
		public static final int FILEDATA_CANCEL = 18;
		public static final int SET_TO_CLIPBOARD = 19;
		public static final int SCREEN_ON_STATE_INFO = 20;
		public static final int SCREEN_OFF_STATE_INFO = 21;
		
		
	}
	
	/**
	 * An operation code.
	 */
	private int opCode = OpCode.INVALID;
	
	/**
	 * Data's length in packet
	 */
	private int payloadLength = 0;
	
	private static byte[] opCodeBuffer = new byte[OPCODE_LENGTH];
	private static byte[] packetSizeBuffer = new byte[PAYLOAD_LENGTH];
	
	private static byte[] heaerBuffer = new byte[OPCODE_LENGTH+PAYLOAD_LENGTH];
	
	
	private PacketHeader(){
	}
	
	public PacketHeader(int opCode, int dataLength){
		this.opCode = opCode;
		this.payloadLength = dataLength;
	}
	
	@Override
	public String toString(){
		return String.format("%2d%4d", opCode, payloadLength+LENGTH);
	}
	
	/**
	 * Convert packet header into byte array.
	 * @return
	 */
	public byte[] asByteArray(){
		//return toString().getBytes();
		CommonUtil.itoa(opCode, heaerBuffer, OPCODE_LENGTH, 0);
		CommonUtil.itoa(payloadLength+LENGTH, heaerBuffer, PAYLOAD_LENGTH, OPCODE_LENGTH);
		return heaerBuffer;
	}
	
	
	private static int ByteToInt(byte [] data){
		int result = 0;
		for(int i=0; i<data.length; i++){
			if(data[i] == ' ')
				continue;
			result = result * 10 + (data[i]-'0');
		}
		return result;
	}
	/**
	 * Parse packet header from given packet header data.
	 * @param headerStr packet header's data in String format
	 * @return PacketHeader object of given packet header
	 * @throws ParseException given packet header is not valid
	 */	
	public static PacketHeader parse(byte[] rawData){
		PacketHeader header = new PacketHeader();
		
		System.arraycopy(rawData, 0, opCodeBuffer, 0, OPCODE_LENGTH);
		
		header.setOpCode(ByteToInt(opCodeBuffer));
		
		System.arraycopy(rawData, OPCODE_LENGTH, packetSizeBuffer, 0, PAYLOAD_LENGTH);				
		
		header.setPayloadLength(ByteToInt(packetSizeBuffer)-PacketHeader.LENGTH);
		
		return header;
	}

	/**
	 * Get an opCode in packet header.
	 * @return
	 */
	public int getOpCode() {
		return opCode;
	}

	/**
	 * Set an opCode.
	 * @param opCode opCode to set
	 * @see OpCode
	 */
	public void setOpCode(int opCode) {
		this.opCode = opCode;
	}

	/**
	 * Get payload's data length.
	 * @return payload's data length
	 */
	public int getPayloadLength() {
		return payloadLength;
	}
	
	/**
	 * Get packet's total length, including header size
	 * @return total packet size
	 */
	public int getPacketLength() {
		return LENGTH+payloadLength;
	}

	/**
	 * Set payload's data length
	 * @param payloadLength payload's data length
	 */
	public void setPayloadLength(int payloadLength) {
		this.payloadLength = payloadLength;
	}

}
