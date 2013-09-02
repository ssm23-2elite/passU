// PassUServerThread.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "PassUServerThread.h"


// CPassUServerThread

IMPLEMENT_DYNCREATE(CPassUServerThread, CWinThread)

CPassUServerThread::CPassUServerThread()
{
	
}

CPassUServerThread::~CPassUServerThread()
{
}

BOOL CPassUServerThread::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	m_hWnd = NULL;
	TRACE("Before passUsocket\n");
	TRACE(_T("Thread에서의 m_hwnd : %p\n", m_hWnd));
	m_passUSocket = new CPassUServerSocket(m_hWnd);

	TRY{
		m_passUSocket -> Attach(m_hSocket); 
		m_passUSocket -> AsyncSelect(FD_READ | FD_CLOSE);
		
		/* Nagle 알고리즘을 해제하는 코드, 우리 프로그램에서는 Nagle 알고리즘 필요없엉 */
	
		const char opt_val = true;
		setsockopt(*m_passUSocket, IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(opt_val));


	} CATCH_ALL(e){
		DWORD dwErrorCode = GetLastError();
		AfxMessageBox(dwErrorCode, 0, 0);
	}
	END_CATCH_ALL

	return TRUE;
}

int CPassUServerThread::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPassUServerThread, CWinThread)
END_MESSAGE_MAP()


// CPassUServerThread 메시지 처리기입니다.


void CPassUServerThread::setClientID(int clientID)
{
	c_id = clientID;
}


int CPassUServerThread::getClientID(void)
{
	return c_id;
}
