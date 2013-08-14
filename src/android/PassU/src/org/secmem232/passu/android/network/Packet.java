package org.secmem232.passu.android.network;

import java.text.*;

/**
 * Represents remoteroid's packet system.</br>
 * A packet consisted of header and payload. 
 * @see PacketHeader
 * @author Taeho Kim
 *
 */
public class Packet {

	public static final int MAX_LENGTH = 4096;
	public static final int MAX_PAYLOAD_LENGTH = MAX_LENGTH - PacketHeader.LENGTH;
	
	
	/**
	 * Packet's header data
	 */
	private PacketHeader header;
	/**
	 * Packet's actual data
	 */
	private static byte[] packetBuffer = new byte[MAX_LENGTH];
	private static byte[] payloadBuffer = new byte[MAX_PAYLOAD_LENGTH];
	private byte[] payload;
	
	protected Packet(){
		// Do nothing on here
	}
	
	/**
	 * Generate packet.
	 * @param opCode an operation code
	 * @param data packet data
	 * @param dataLength length of data
	 */
	public Packet(int opCode, byte[] data, int dataLength){
		header = new PacketHeader(opCode, dataLength);
		payload = data;
	}
	
	/**
	 * Parse packet with given raw packet stream.
	 * @param rawPacket packet data in byte array
	 * @return Packet object that has received
	 * @throws ParseException failed to parse packet.
	 */
	public static Packet parse(byte[] rawPacket){
		Packet packet = new Packet();		
		
		// Get header
		packet.setHeader(PacketHeader.parse(rawPacket));		
		
		int payloadLength = packet.getHeader().getPayloadLength();

		// Get data (payload)
		System.arraycopy(rawPacket, PacketHeader.LENGTH, payloadBuffer, 0, payloadLength);
		packet.setPayload(payloadBuffer);		
				
		// Packet parsing has done.
		return packet;
	}
	
	/**
	 * Get packet as byte array.
	 * @return
	 */
	public byte[] asByteArray(){		
		
		if(header==null)
			throw new IllegalStateException("Packet header has not been set.");
				
		// Append header
		byte[] headerData = header.asByteArray();
		System.arraycopy(headerData, 0, packetBuffer, 0, PacketHeader.LENGTH);
		
		// Append payload
		if(payload!=null)			
			System.arraycopy(payload, 0, packetBuffer, PacketHeader.LENGTH, payload.length);
		return packetBuffer;
	}


	/**
	 * Get packet's header.
	 * @return Packet's header
	 */
	public PacketHeader getHeader() {
		return header;
	}

	/**
	 * Set packet's header
	 * @param mHeader Packet's header
	 */
	public void setHeader(PacketHeader mHeader) {
		this.header = mHeader;
	}

	/**
	 * Get packet's actual data(payload).
	 * @return packet's payload
	 */
	public byte[] getPayload() {
		return payload;
	}

	/**
	 * Set packet's payload data
	 * @param payload a data to put into packet with
	 */
	public void setPayload(byte[] payload) {
		this.payload = payload;
	}
	
	/**
	 * Get packet's opcode
	 * @return packet's opCode
	 */
	public int getOpcode(){
		return header.getOpCode();
	}

}
