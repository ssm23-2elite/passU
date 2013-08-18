#pragma once

// CMyListen 명령 대상입니다.

class CMyListen : public CAsyncSocket
{
public:
	CMyListen();
	virtual ~CMyListen();
	virtual void OnAccept(int nErrorCode);
};


