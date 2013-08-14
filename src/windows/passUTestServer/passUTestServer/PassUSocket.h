#pragma once

// CPassUSocket 명령 대상입니다.

#pragma once

class CPassUSocket : public CSocket
{
public:
	CPassUSocket();
	virtual ~CPassUSocket();

	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};


