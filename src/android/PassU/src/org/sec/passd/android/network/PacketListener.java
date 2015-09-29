package org.sec.passd.android.network;

public interface PacketListener {
	public void onPacketReceived(Packet packet);
	public void onInterrupt();
}
