
// TestServer.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "TestServer.h"
#include "TestServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestServerApp

BEGIN_MESSAGE_MAP(CTestServerApp, CWinApp)
	//ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestServerApp 생성

CTestServerApp::CTestServerApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CTestServerApp 개체입니다.

CTestServerApp theApp;


// CTestServerApp 초기화

BOOL CTestServerApp::InitInstance()
{
	CWinApp::InitInstance();

	bClientConnected = FALSE;

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}



	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	CTestServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 응용 프로그램이 예기치 않게 종료됩니다.\n");
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

void CTestServerApp::initServer(int port)
{
	if(!server.Create(port))  
	{            
		AfxMessageBox(_T("Socket Create Failed\n"));
		return;
	}

	if(!server.Listen())                                          //리슨
	{            
		AfxMessageBox(_T("Socket Listen Failed\n"));
		return;        
	}

	if(!server.Accept(client))      //클라이언트 접속 기다림 (접속될때까지블락!!)
	{          
		AfxMessageBox(_T("Socket Accept Failed\n"));
		return;
	}

	MPACKET packet;
	packet.sendDev = 5;
	packet.recvDev = 6;
	packet.deviceType = 1;
	packet.relativeField = 0;
	packet.updownFlag = -1;
	packet.wheelFlag = 0;
	packet.xCoord = 100;
	packet.yCoord = 200;

	sendMouseData(&packet);
	bClientConnected = TRUE;
	bMouseDown = FALSE;
}

void CTestServerApp::sendMouseMove(int x, int y)
{
	if(bClientConnected) {
		MPACKET packet;
		packet.sendDev = 5;
		packet.recvDev = 6;
		packet.deviceType = 1;
		packet.relativeField = 0;
		packet.updownFlag = (bMouseDown == TRUE)?0:1;
		packet.wheelFlag = 0;
		packet.xCoord = x;
		packet.yCoord = y;

		sendMouseData(&packet);
	}
}

void CTestServerApp::sendMouseDown(int x, int y)
{
	if(bClientConnected) {
		bMouseDown = TRUE;
		MPACKET packet;
		packet.sendDev = 5;
		packet.recvDev = 6;
		packet.deviceType = 1;
		packet.relativeField = 0;
		packet.updownFlag = (bMouseDown == TRUE)?0:1;
		packet.wheelFlag = 0;
		packet.xCoord = x;
		packet.yCoord = y;

		sendMouseData(&packet);
	}
}


void CTestServerApp::sendMouseUp(int x, int y)
{
	if(bClientConnected) {
		bMouseDown = FALSE;
		MPACKET packet;
		packet.sendDev = 5;
		packet.recvDev = 6;
		packet.deviceType = 1;
		packet.relativeField = 0;
		packet.updownFlag = (bMouseDown == TRUE)?0:1;
		packet.wheelFlag = 0;
		packet.xCoord = x;
		packet.yCoord = y;

		sendMouseData(&packet);
	}
}


void CTestServerApp::sendMouseData(MPACKET *pPacket)
{
	char buf2[100];

	sprintf_s(buf2, "%4d%4d%1d%1d%1d%1d%4d%4d%4d", pPacket->sendDev, pPacket->recvDev, pPacket->deviceType, 
		pPacket->relativeField, pPacket->updownFlag,
		pPacket->wheelFlag, pPacket->xCoord, pPacket->yCoord, 0);
	MPACKET buf;
	if(bClientConnected) {
		int bytes = client.Send((void*)buf2, sizeof(MPACKET));    //데이터전송

		CString str;
		str.Format(_T("real[%d] want[%d]"), bytes, sizeof(MPACKET));

		ZeroMemory(&buf, sizeof(MPACKET));
		bytes = client.Receive((void*)&buf, sizeof(MPACKET)); 
	}
}

void CTestServerApp::sendKeyDown(UINT keyCode)
{
	if(bClientConnected) {
		bKeyDown = TRUE;
		KPACKET packet;
		packet.sendDev = 5;
		packet.recvDev = 6;
		packet.deviceType = 0;
		packet.relativeField = 0;
		packet.updownFlag = (bKeyDown == TRUE)?0:1;
		packet.keyCode = keyCode;

		sendKeyData(&packet);
	}
}


void CTestServerApp::sendKeyUp(UINT keyCode)
{
	if(bClientConnected) {
		bKeyDown = FALSE;
		KPACKET packet;
		packet.sendDev = 5;
		packet.recvDev = 6;
		packet.deviceType = 0;
		packet.relativeField = 0;
		packet.updownFlag = (bKeyDown == TRUE)?0:1;
		packet.keyCode = keyCode;

		sendKeyData(&packet);
	}
}


void CTestServerApp::sendKeyData(KPACKET *pPacket)
{
	char buf2[100];

	sprintf_s(buf2, "%4d%4d%1d%1d%1d%4d%9d", pPacket->sendDev, pPacket->recvDev, pPacket->deviceType, 
		pPacket->relativeField, pPacket->updownFlag,
		pPacket->keyCode, 0);
	KPACKET buf;
	if(bClientConnected) {
		int bytes = client.Send((void*)buf2, sizeof(KPACKET));    //데이터전송

		CString str;
		str.Format(_T("real[%d] want[%d]"), bytes, sizeof(KPACKET));

		ZeroMemory(&buf, sizeof(KPACKET));
		bytes = client.Receive((void*)&buf, sizeof(KPACKET));
	}
}