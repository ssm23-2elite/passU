// ServerSock.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "ServerSock.h"
#include "ServerConf.h"


// CServerSock

CServerSock::CServerSock()
{
}

CServerSock::~CServerSock()
{
}


// CServerSock 멤버 함수


void CServerSock::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	((ServerConf *) AfxGetApp )->accept();
	CAsyncSocket::OnAccept(nErrorCode);
}
