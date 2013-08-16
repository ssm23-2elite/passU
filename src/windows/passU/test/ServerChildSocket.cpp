// ServerChildSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "ServerChildSocket.h"


// ServerChildSocket

ServerChildSocket::ServerChildSocket()
{
}

ServerChildSocket::~ServerChildSocket()
{
}


// ServerChildSocket 멤버 함수


void ServerChildSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	((CtestApp *)AfxGetApp)->closeChild();
	CSocket::OnClose(nErrorCode);
}


void ServerChildSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	((CtestApp *)AfxGetApp)->receiveData();
	CSocket::OnReceive(nErrorCode);
}
