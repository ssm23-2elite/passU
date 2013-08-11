package org.ssm232elite.passu.android.network;

public interface UDPReceiver {
	public void onConnected();
	public void onConnectionFailed();
	public void onDisConnected();
	public void onReceive(byte[] buf);
	public void onError(int errno);
}
