package org.sec.passd.android;
import java.util.List;

interface IPassD{
	String getConnectionStatus();
	boolean isConnected();
	void connect(String ip, int port);
	void disconnect();	
}