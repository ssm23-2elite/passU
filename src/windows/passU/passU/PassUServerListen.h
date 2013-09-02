#pragma once

#include "PassUServerThread.h"
// CPassUServerListen 명령 대상입니다.

class CPassUServerListen : public CAsyncSocket
{
public:
	CPassUServerListen();
	virtual ~CPassUServerListen();
	CObList * getSockList(void);
	CObList m_sockList;
	CPassUServerThread *pThread;

	virtual void OnAccept(int nErrorCode);
};


