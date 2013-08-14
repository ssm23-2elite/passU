
// test.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"
#include "ClientConf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtestApp

BEGIN_MESSAGE_MAP(CtestApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CtestApp 생성

CtestApp::CtestApp()
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	m_pServer = NULL;
	m_pClient = NULL;
	m_pChild = NULL;
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CtestApp 개체입니다.

CtestApp theApp;


// CtestApp 초기화

BOOL CtestApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	
	

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	CtestDlg dlg;
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
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}



void CtestApp::initServer(int nPort)
{
	m_pServer = new ServerSocket();
	m_pServer->Create(nPort);
	m_pServer->Listen();

}


void CtestApp::cleanUp(void)
{
	if(m_pServer)	delete m_pServer;
	if(m_pChild)	delete m_pChild;
}


void CtestApp::sendData(CString strData)
{
	if(m_pChild){
		strData.Insert(strData.GetLength(), _T("\0"));
		m_pChild->Send(strData, 100);
		CString strIP, strText;
		UINT nPort;
		m_pChild->GetSockName(strIP, nPort);
		strText.Format(_T("[ %s ] %s "), strIP, strData);

	}
}


void CtestApp::receiveData(void)
{
	char temp[1024];

	m_pChild->Receive(temp, sizeof(temp));
	CString strIP, strText;

	UINT nPort;

	m_pChild->GetPeerName(strIP, nPort);
	
	strText.Format(_T("[ %s ] %s "), strIP, temp);
	
}


void CtestApp::accept(void)
{
	m_pChild = new ServerChildSocket();

	BOOL check = m_pServer->Accept(*m_pChild);

	if(check == FALSE){
		AfxMessageBox(_T("접속 허용 실패"));
		return ;
	}

}


void CtestApp::closeChild(void)
{
	m_pChild->Close();
	delete m_pChild;

}


void CtestApp::clientConnect(void)
{
	ClientConf dlg;
	UINT nPort;

	nPort = _ttoi(m_portNum);

	if(dlg.DoModal() == IDOK)
	{
		m_pClient = new ClientSocket();
		m_pClient->Create();
		m_pClient->Connect(dlg.m_address, nPort);
	}


}


void CtestApp::clientCleanUp(void)
{
	if(m_pClient)	delete m_pClient;
}


void CtestApp::clientSendData(CString strData)
{
}


void CtestApp::clientReceiveData(void)
{
}


void CtestApp::clientcloseChild(void)
{
}
