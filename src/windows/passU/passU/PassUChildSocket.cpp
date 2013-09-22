// PassUChildSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "PassUChildSocket.h"
#include "Server.h"
#include "PassUDlg.h"


// CPassUChildSocket

CPassUChildSocket::CPassUChildSocket()
{
}

CPassUChildSocket::~CPassUChildSocket()
{
}


// CPassUChildSocket 멤버 함수


void CPassUChildSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CPassUDlg* pDlg = (CPassUDlg*)::AfxGetMainWnd();
	pDlg->ReceiveData(this);
	CSocket::OnReceive(nErrorCode);
}


void CPassUChildSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CPassUDlg* pDlg = (CPassUDlg*)::AfxGetMainWnd();
	pDlg->CloseChild(this);
	CSocket::OnClose(nErrorCode);
}
