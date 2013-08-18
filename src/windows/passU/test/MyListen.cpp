// MyListen.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "MyListen.h"
#include "MyThread.h"


// CMyListen

CMyListen::CMyListen()
{
}

CMyListen::~CMyListen()
{
}


// CMyListen 멤버 함수


void CMyListen::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CSocket tmp;

	if(Accept(tmp)){
		CMyThread *pThread = (CMyThread *)AfxBeginThread(RUNTIME_CLASS(CMyThread), 0, 0, CREATE_SUSPENDED);

		if(pThread){
			pThread->m_hSocket = tmp.Detach();
			pThread->ResumeThread();
		}
	}
	CAsyncSocket::OnAccept(nErrorCode);
}
