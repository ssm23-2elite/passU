package org.secmem232.passu.android;
import java.util.List;

interface IPassU{
	String getConnectionStatus();
	boolean isConnected();
	void connect(String ip, int port);
	void disconnect();	
}