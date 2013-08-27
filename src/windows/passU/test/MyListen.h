#pragma once

#include <list>
#include "MySocket.h"
#include "MyThread.h"

// CMyListen 명령 대상입니다.
using namespace std;

class CMyListen : public CAsyncSocket
{
public:
	//CObList m_sockList;
//	std::list<CMyThread>  
/*
	typedef CTypedPtrArray<CObArray, CMyThread *> CSocketList;

	CSocketList m_clientSock;
*/
	CMyListen();
	CObList m_sockList;
	int m_AcceptCount;
	virtual ~CMyListen();
	virtual void OnAccept(int nErrorCode);

	CObList getSockList(void);
};


