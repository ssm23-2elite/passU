// MyThread.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "MyThread.h"
#include "ServerConf.h"

#include "packet.h"
// CMyThread

IMPLEMENT_DYNCREATE(CMyThread, CWinThread)

CMyThread::CMyThread()
{
}

CMyThread::~CMyThread()
{
}

BOOL CMyThread::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	m_mySocket = new CMySocket();

	TRY{
		m_mySocket -> Attach(m_hSocket); 
		m_mySocket -> AsyncSelect(FD_READ | FD_CLOSE);
//		m_mySocket -> send(

	} CATCH_ALL(e){
		DWORD dwErrorCode = GetLastError();
		AfxMessageBox(dwErrorCode, 0, 0);
	}
	END_CATCH_ALL

	return TRUE;
}

int CMyThread::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CMyThread, CWinThread)
END_MESSAGE_MAP()


// CMyThread 메시지 처리기입니다.
