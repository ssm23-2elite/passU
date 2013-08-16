#pragma once

// ServerSocket 명령 대상입니다.

class ServerSocket : public CSocket
{
public:
	ServerSocket();
	virtual ~ServerSocket();
	virtual void OnAccept(int nErrorCode);
};


