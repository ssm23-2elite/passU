#pragma once
#include "MySocket.h"
#include "MyThread.h"
// CMyListen 명령 대상입니다.

class CMyListen : public CAsyncSocket
{
public:
	CObList m_sockList;

/*
	typedef CTypedPtrArray<CObArray, CMyThread *> CSocketList;

	CSocketList m_clientSock;
*/
	CMyListen();
	
	virtual ~CMyListen();
	virtual void OnAccept(int nErrorCode);

};


