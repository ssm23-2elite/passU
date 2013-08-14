// PassUListen.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "passUTestServer.h"
#include "PassUListen.h"



// CPassUListen

CPassUListen::CPassUListen()
{
}

CPassUListen::~CPassUListen()
{
}


// CPassUListen 멤버 함수
void CPassUListen::OnAccept(int nErrorCode)
{
	CSocket tmp;

	if(Accept(tmp))
	{
		CPassUThread* pThread = (CPassUThread *)AfxBeginThread(RUNTIME_CLASS(CPassUThread), 0, 0, CREATE_SUSPENDED);

		if (pThread)
		{
			pThread->m_hSocket = tmp.Detach();
			pThread->ResumeThread();
		}
	}
	CAsyncSocket::OnAccept(nErrorCode);
}
