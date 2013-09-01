// Server.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "Server.h"
#include "afxdialogex.h"


// 후킹을 위한 함수 포인터
typedef HHOOK (*InstallKeyboardHook)();
typedef HHOOK (*InstallMouseHook)();

typedef void (*UnInstallKeyboardHook)();
typedef void  (*UnInstallMouseHook)();

InstallKeyboardHook installKeyhook;
InstallMouseHook installMousehook;

UnInstallKeyboardHook uninstallKeyhook;
UnInstallMouseHook uninstallMousehook;


// CServer 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServer, CDialogEx)

CServer::CServer(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServer::IDD, pParent)
	, serverIPAddress(_T(""))
{
	TRACE("CServer Init\n");
	
	/* 현재 서버가 될 컴퓨터의 IP를 알아내는 코드 */
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[255];
	PHOSTENT hostinfo;
	CString strIpAddress;
	wVersionRequested = MAKEWORD(2, 0);
	if(WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		if(gethostname(name, sizeof(name)) == 0)
		{
			if((hostinfo = gethostbyname(name)) != NULL)
				strIpAddress = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
		} 
		WSACleanup();
	}

	serverIPAddress.Append(strIpAddress);


	/*hinstDLL = LoadLibrary(_T("KeyHook.dll"));
	if(!hinstDLL)
		AfxMessageBox(_T("KeyHook.dll 로드 실패!"));*/

	installKeyhook = (InstallKeyboardHook)GetProcAddress(hinstDLL, "InstallKeyboardHook");
	installMousehook = (InstallMouseHook)GetProcAddress(hinstDLL, "InstallMouseHook");

	uninstallKeyhook = (UnInstallKeyboardHook)GetProcAddress(hinstDLL, "UnInstallKeyboardHook");
	uninstallMousehook = (UnInstallMouseHook)GetProcAddress(hinstDLL, "UnInstallMouseHook");

	
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return ;
	}

}

CServer::~CServer()
{
}

void CServer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IPADDRESS, serverIPAddress);
	DDX_Control(pDX, IDC_BUTTON1, m_cBtn_one);
	DDX_Control(pDX, IDC_BUTTON2, m_cBtn_two);
	DDX_Control(pDX, IDC_BUTTON3, m_cBtn_three);
	DDX_Control(pDX, IDC_BUTTON4, m_cBtn_four);
	DDX_Control(pDX, IDC_BUTTON5, m_cBtn_server);
	DDX_Control(pDX, IDC_BUTTON6, m_cBtn_six);
	DDX_Control(pDX, IDC_BUTTON7, m_cBtn_seven);
	DDX_Control(pDX, IDC_BUTTON8, m_cBtn_eight);
	DDX_Control(pDX, IDC_BUTTON9, m_cBtn_nine);
	DDX_Control(pDX, IDC_LIST1, m_cListCtrl);
}


BEGIN_MESSAGE_MAP(CServer, CDialogEx)
END_MESSAGE_MAP()


// CServer 메시지 처리기입니다.
