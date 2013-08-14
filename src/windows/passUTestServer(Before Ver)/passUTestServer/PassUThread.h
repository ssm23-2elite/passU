#pragma once



// CPassUThread
#pragma once

#include "PassUSocket.h"

class CPassUThread : public CWinThread
{
	DECLARE_DYNCREATE(CPassUThread)

protected:
	CPassUThread();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CPassUThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	SOCKET m_hSocket;
	CPassUSocket* m_passuSocket;

protected:
	DECLARE_MESSAGE_MAP()
};


