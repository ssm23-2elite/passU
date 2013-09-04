// PassUClientSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "PassUClientSocket.h"
#include "PassUDlg.h"

// CPassUClientSocket

CPassUClientSocket::CPassUClientSocket()
{
}

CPassUClientSocket::~CPassUClientSocket()
{
}


// CPassUClientSocket 멤버 함수


void CPassUClientSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
		CPassUDlg* pDlg = (CPassUDlg*)::AfxGetMainWnd();
	pDlg->ReceiveClientData(this);
	CSocket::OnReceive(nErrorCode);
}


void CPassUClientSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
		CPassUDlg* pDlg = (CPassUDlg*)::AfxGetMainWnd();
		pDlg->CloseClient(this);
	CSocket::OnClose(nErrorCode);
}
