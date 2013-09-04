#pragma once

// CPassUChildSocket 명령 대상입니다.

class CPassUChildSocket : public CSocket
{
public:
	CPassUChildSocket();
	virtual ~CPassUChildSocket();
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	int c_id;
};


