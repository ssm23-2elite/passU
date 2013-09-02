// PassUServerSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "PassUServerSocket.h"
#include "Server.h"

// CPassUServerSocket

CPassUServerSocket::CPassUServerSocket()
{
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
	((CServer *)AfxGetApp())->ReceiveData(this);
	CSocket::OnReceive(nErrorCode);
}
