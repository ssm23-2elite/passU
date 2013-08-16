package org.secmem232.passu.android.network;

public interface ServerConnectionListener {
	public void onServerConnected(String ip, int port);
	public void onServerConnectionFailed();
	public void onServerConnectionInterrupted();
	public void onServerDisconnected();
}
