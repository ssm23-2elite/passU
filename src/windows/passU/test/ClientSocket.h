#pragma once

// ClientSocket 명령 대상입니다.

class ClientSocket : public CSocket
{
public:
	ClientSocket();
	virtual ~ClientSocket();
	virtual void OnClose(int nErrorCode);
};


