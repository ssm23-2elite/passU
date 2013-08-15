
// test.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "ServerChildSocket.h"


// CtestApp:
// 이 클래스의 구현에 대해서는 test.cpp을 참조하십시오.
//

class CtestApp : public CWinApp
{
public:
	CtestApp();
	int flag;
	CString m_ipAddress;
	CString m_portNum;

	CSocket *m_serverSock;
	CSocket *m_serverRecvSock;

	ServerSocket *m_pServer;
	ServerChildSocket *m_pChild;
	ClientSocket *m_pClient;
	
	void initServer(UINT nPort);
	void cleanUp(void);
	void sendData(CString strData);
	void receiveData(void);
	void accept(void);
	void closeChild(void);
	
	void clientConnect(void);
	void clientCleanUp(void);
	void clientSendData(CString strData);
	void clientReceiveData(void);
	void clientcloseChild(void);

	// 재정의입니다.
public:
	virtual BOOL InitInstance();

	// 구현입니다.

	DECLARE_MESSAGE_MAP()
};

extern CtestApp theApp;