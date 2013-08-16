#pragma once

// ServerChildSocket 명령 대상입니다.

class ServerChildSocket : public CSocket
{
public:
	ServerChildSocket();
	virtual ~ServerChildSocket();
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};


