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
	int nClient; // 클라이언트 숫자
	virtual void OnAccept(int nErrorCode);
	HWND m_hWnd;
};


