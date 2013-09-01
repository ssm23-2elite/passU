// ClientSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "ClientSocket.h"
#include "ClientConf.h"


// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket 멤버 함수


void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	((ClientConf *)AfxGetApp())->receiveData(this);
	CSocket::OnReceive(nErrorCode);
}


void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	((ClientConf *)AfxGetApp())->closeSock();

	CSocket::OnClose(nErrorCode);
}
