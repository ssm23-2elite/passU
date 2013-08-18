#pragma once

#include "MySocket.h"

// CMyThread

class CMyThread : public CWinThread
{
	DECLARE_DYNCREATE(CMyThread)

protected:
	CMyThread();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMyThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	SOCKET m_hSocket;
	CMySocket *m_mySocket;
protected:
	DECLARE_MESSAGE_MAP()
};


