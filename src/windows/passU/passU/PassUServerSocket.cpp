// PassUServerSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "PassUServerSocket.h"
#include "Server.h"

// CPassUServerSocket

CPassUServerSocket::CPassUServerSocket(){}

CPassUServerSocket::CPassUServerSocket(HWND hWnd)
{
	TRACE(_T("Socket에서의 받기 전 m_hwnd : %p, hWnd : %p\n", m_hWnd, hWnd));
	m_hWnd = hWnd;
	TRACE(_T("Socket에서의 받고나서 m_hwnd : %p, hWnd : %p\n", m_hWnd, hWnd));
}

CPassUServerSocket::~CPassUServerSocket()
{
}

// CPassUServerSocket 멤버 함수


void CPassUServerSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CSocket::OnClose(nErrorCode);
}


void CPassUServerSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	
	PACKET p;
	COPYDATASTRUCT CDS;
	Receive(&p, sizeof(PACKET));

	HWND hWnd = ::FindWindow(NULL, TEXT("PassU - Pass Your USB via Network"));
	
	
	TRACE(_T("Socket에서의 m_hwnd : %p\n", m_hWnd));

	::SendMessage(m_hWnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
	
 //	((CServer *)AfxGetApp())->ReceiveData(this);
	CSocket::OnReceive(nErrorCode);
}
