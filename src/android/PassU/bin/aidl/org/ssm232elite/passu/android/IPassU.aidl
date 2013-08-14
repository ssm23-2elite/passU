package org.ssm232elite.passu.android;
import java.util.List;

interface IPassU{
	String getConnectionStatus();
	boolean isConnected();
	void connect(String ipAddress);
	void disconnect();
}