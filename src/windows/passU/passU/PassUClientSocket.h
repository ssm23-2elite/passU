#pragma once

// CPassUClientSocket 명령 대상입니다.

class CPassUClientSocket : public CSocket
{
public:
	CPassUClientSocket();
	virtual ~CPassUClientSocket();
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};


