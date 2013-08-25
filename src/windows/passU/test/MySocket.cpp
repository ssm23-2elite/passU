// MySocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "MySocket.h"
#include "ServerConf.h"

// CMySocket

CMySocket::CMySocket()
{
}

CMySocket::~CMySocket()
{
}


// CMySocket 멤버 함수


void CMySocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	((ServerConf *)AfxGetApp())->closeClient(this);
	CSocket::OnClose(nErrorCode);
}


void CMySocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	((ServerConf *)AfxGetApp())->receiveData(this);

	CSocket::OnReceive(nErrorCode);
}
