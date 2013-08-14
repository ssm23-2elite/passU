// PassUSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "passUTestServer.h"
#include "PassUSocket.h"


// CPassUSocket

CPassUSocket::CPassUSocket()
{
}

CPassUSocket::~CPassUSocket()
{
}


// CPassUSocket 멤버 함수
void CPassUSocket::OnReceive(int nErrorCode)
{
	printf("Hello");
}

void CPassUSocket::OnClose(int nErrorCode)
{
}