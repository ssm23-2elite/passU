#pragma once

// CAcceptSock 명령 대상입니다.

class CAcceptSock : public CAsyncSocket
{
public:
	CAcceptSock();
	virtual ~CAcceptSock();
	virtual void OnClose(int nErrorCode);
};


