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


	hinstDLL = LoadLibrary(_T("KeyHook.dll"));
	if(!hinstDLL)
		AfxMessageBox(_T("KeyHook.dll 로드 실패!"));

	installKeyhook = (InstallKeyboardHook)GetProcAddress(hinstDLL, "InstallKeyboardHook");
	installMousehook = (InstallMouseHook)GetProcAddress(hinstDLL, "InstallMouseHook");

	uninstallKeyhook = (UnInstallKeyboardHook)GetProcAddress(hinstDLL, "UnInstallKeyboardHook");
	uninstallMousehook = (UnInstallMouseHook)GetProcAddress(hinstDLL, "UnInstallMouseHook");


	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return ;
	}
	CBitmap bitmap;

	bitmap.LoadBitmapA(IDB_BITMAP2);
	m_imgList.Create(15, 15, ILC_COLOR8, 1, 1);
	m_imgList.Add(&bitmap, RGB(255, 0, 255));
	m_wating_client.SetImageList(&m_imgList, LVSIL_SMALL);
	m_wating_client.InsertItem(0, "tmp", 0);

}

CServer::~CServer()
{
}

void CServer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IPADDRESS, serverIPAddress);
	DDX_Control(pDX, IDC_BUTTON1, m_location_info[0]);
	DDX_Control(pDX, IDC_BUTTON2, m_location_info[1]);
	DDX_Control(pDX, IDC_BUTTON3, m_location_info[2]);
	DDX_Control(pDX, IDC_BUTTON4, m_location_info[3]);
	DDX_Control(pDX, IDC_BUTTON5, m_location_info[4]);
	DDX_Control(pDX, IDC_BUTTON6, m_location_info[5]);
	DDX_Control(pDX, IDC_BUTTON7, m_location_info[6]);
	DDX_Control(pDX, IDC_BUTTON8, m_location_info[7]);
	DDX_Control(pDX, IDC_BUTTON9, m_location_info[8]);
	DDX_Control(pDX, IDC_LIST1, m_wating_client);
	DDX_Control(pDX, IDC_BUTTON10, m_CBtn_ServerStart);
}


BEGIN_MESSAGE_MAP(CServer, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON10, &CServer::OnBnClickedServerStart)
	ON_BN_CLICKED(IDC_BUTTON11, &CServer::OnBnClickedServerStop)
	ON_BN_CLICKED(IDC_BUTTON1, &CServer::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CServer::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CServer::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CServer::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, &CServer::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CServer::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CServer::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CServer::OnBnClickedButton9)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_COPYDATA()
END_MESSAGE_MAP()


// CServer 메시지 처리기입니다.


void CServer::OnStartServer()
{
	listen.Create(30000);
	listen.Listen();

	AfxMessageBox(_T("InitServer"));
}


void CServer::OnChangeLocationInfo(int index)
{

}


void CServer::bindWatingClient(int client_index)
{

}


void CServer::OnDisconnectedClient(int client_index)
{
	POSITION pos = listen.m_sockList.GetHeadPosition();

	CPACKET tmp;
	tmp.msgType = 3;
	tmp.c_id = client_index;
	tmp.hello = 0;
	tmp.bye = 1;

	// 소켓 리스트에서 client_index만큼 가져와서 그 소켓한테 보냄
	int cindex = client_index;

	while(cindex != 0){
		listen.m_sockList.GetNext(pos);
		cindex -= 1;
	}

	cThread = (CPassUServerThread *)listen.m_sockList.GetAt(pos);

	cThread->m_passUSocket->Send((LPCSTR *)&tmp, sizeof(CPACKET)); // BYE 패킷 전송

	//socket list에서 제거
	listen.m_sockList.RemoveAt(pos);

}


void CServer::OnArrivedScreenEdge(int position)
{

}


void CServer::OnBnClickedServerStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_CBtn_ServerStart.EnableWindow(FALSE);
	OnStartServer();
}


void CServer::OnBnClickedServerStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_CBtn_ServerStart.EnableWindow(TRUE);

}


void CServer::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}


void CServer::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CServer::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CServer::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CServer::OnBnClickedButton6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CServer::OnBnClickedButton7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CServer::OnBnClickedButton8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CServer::OnBnClickedButton9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CServer::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CServer::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CServer::ReceiveData(CPassUServerSocket * s)
{
	KPACKET tmp;
	s->Receive(&tmp, sizeof(KPACKET));
	COPYDATASTRUCT CDS;

	CDS.dwData = 0; // receiveData
	CDS.cbData = sizeof(KPACKET);
	CDS.lpData = &tmp;
	SendMessage(WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
}


BOOL CServer::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	POSITION pos = listen.m_sockList.GetHeadPosition();

	switch(pCopyDataStruct->dwData){
	case 0: // receiveData
		k = (KPACKET *) pCopyDataStruct->lpData; // 구조체 연결

		switch(k->msgType){
		case 1: // keyboard
			break;

		case 2: // mouse
			break;


		case 3: // client
			if(k->deviceType == 1){ // hello packet
				// 헬로 패킷이면
				// 버튼에 클라이언트 연결해주고, 첫번째 클라이언트면 후킹 시작하고
				// 클라이언트에 자신의 id 알려주고
				// 리스트 컨트롤에 아이콘 하나 추가
				CBitmap bitmap;

				bitmap.LoadBitmapA(IDB_BITMAP2);
				m_imgList.Create(15, 15, ILC_COLOR8, 1, 1);
				m_imgList.Add(&bitmap, RGB(255, 0, 255));
				m_wating_client.SetImageList(&m_imgList, LVSIL_SMALL);

				m_wating_client.InsertItem(0, "tmp", 0);



				cThread = (CPassUServerThread *)listen.m_sockList.GetAt(pos);




			} else if(k->relativeField == 1){ // bye packet
				// 굿바이패킷이면
				// 버튼에 클라이언트 해제


				OnDisconnectedClient(k->sendDev); // 클라이언트 디스커넥트
			}
			break;

		case 4:

			break;

		}

		break;
	}
	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}
