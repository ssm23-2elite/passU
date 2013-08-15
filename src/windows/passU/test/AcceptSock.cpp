// AcceptSock.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "AcceptSock.h"
#include "ServerConf.h"

// CAcceptSock

CAcceptSock::CAcceptSock()
{
}

CAcceptSock::~CAcceptSock()
{
}


// CAcceptSock 멤버 함수


void CAcceptSock::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	((ServerConf *) AfxGetApp)->closeAcceptSock();
	CAsyncSocket::OnClose(nErrorCode);
}
