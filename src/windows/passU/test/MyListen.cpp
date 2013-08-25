// MyListen.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "MyListen.h"
#include "MyThread.h"

#include "afxdialogex.h"

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
			m_sockList.AddTail(pThread);
			pThread->ResumeThread(); 
		}

		AfxMessageBox(_T("쓰레드 생성, 연결 완료!"));
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

