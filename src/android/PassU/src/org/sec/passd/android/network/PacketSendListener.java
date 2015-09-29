package org.sec.passd.android.network;

public interface PacketSendListener {
	public void onPacketSent();
	public void onSendFailed();
}
