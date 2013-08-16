// ServerSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "ServerSocket.h"


// ServerSocket

ServerSocket::ServerSocket()
{
	int a = 1;

}

ServerSocket::~ServerSocket()
{

}

void ServerSocket::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	((CtestApp *)AfxGetApp())->accept();
	CSocket::OnAccept(nErrorCode);
}
