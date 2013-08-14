// PassUThread.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "passUTestServer.h"
#include "PassUThread.h"


// CPassUThread

IMPLEMENT_DYNCREATE(CPassUThread, CWinThread)

CPassUThread::CPassUThread()
{
}

CPassUThread::~CPassUThread()
{
}

BOOL CPassUThread::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	m_passuSocket = new CPassUSocket();

	TRY
	{
		m_passuSocket->Attach(m_hSocket);
		m_passuSocket->AsyncSelect(FD_READ | FD_CLOSE);
	}
	CATCH_ALL(e)
	{
		DWORD dwErrorCode = GetLastError();
		AfxMessageBox(dwErrorCode, 0, 0);
	}
	END_CATCH_ALL
	return TRUE;
}

int CPassUThread::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPassUThread, CWinThread)
END_MESSAGE_MAP()


// CPassUThread 메시지 처리기입니다.
