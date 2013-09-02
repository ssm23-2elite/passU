// PassUServerListen.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "PassUServerListen.h"


// CPassUServerListen

CPassUServerListen::CPassUServerListen()
{
}

CPassUServerListen::~CPassUServerListen()
{
}


// CPassUServerListen 멤버 함수


CObList * CPassUServerListen::getSockList(void)
{
	return &m_sockList;
}


void CPassUServerListen::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CSocket tmp;

	if(Accept(tmp)){
		pThread = (CPassUServerThread *)AfxBeginThread(RUNTIME_CLASS(CPassUServerThread), 0, 0, CREATE_SUSPENDED);

		if(pThread){
			pThread->m_hSocket = tmp.Detach();
			m_sockList.AddTail(pThread);
			pThread->c_id = m_sockList.GetCount();
			pThread->ResumeThread();
			TRACE("m_sockList Add Tail Success, pThread Sucess !\n");

		}

		AfxMessageBox(_T("쓰레드 생성, Accept 완료!"));
	}
	CAsyncSocket::OnAccept(nErrorCode);
}
