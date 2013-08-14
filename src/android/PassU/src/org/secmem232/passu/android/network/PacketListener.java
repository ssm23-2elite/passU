package org.secmem232.passu.android.network;

public interface PacketListener {
	public void onPacketReceived(Packet packet);
	public void onInterrupt();
}
