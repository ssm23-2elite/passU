// MyListen.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "MyListen.h"
#include "MyThread.h"
#include "packet.h"
#include "afxdialogex.h"

// CMyListen

CMyListen::CMyListen()
{
}

CMyListen::~CMyListen()
{
	if(pThread != NULL){
		pThread->Delete();
	}
}


// CMyListen 멤버 함수



void CMyListen::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CSocket tmp;

	if(Accept(tmp)){
		pThread = (CMyThread *)AfxBeginThread(RUNTIME_CLASS(CMyThread), 0, 0, CREATE_SUSPENDED);

		if(pThread){
			pThread->m_hSocket = tmp.Detach();
			m_sockList.AddTail(pThread);
			m_AcceptCount = m_sockList.GetCount();
			pThread->ResumeThread(); 
			TRACE("m_sockList Add success, m_Acceptcount : %d\n", m_AcceptCount);
			
		}

		AfxMessageBox(_T("쓰레드 생성, 연결 완료!"));
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

CObList * CMyListen::getList(void)
{
	return &m_sockList;
}
