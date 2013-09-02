// Client.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "Client.h"
#include "afxdialogex.h"


// CClient 대화 상자입니다.

IMPLEMENT_DYNAMIC(CClient, CDialogEx)

	CClient::CClient(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClient::IDD, pParent)
	, m_connectFlag(false)
	, client_ID(0)
{

}

CClient::~CClient()
{
}

void CClient::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IpAddressCtrl);
	DDX_Control(pDX, IDC_BUTTON1, m_cBtn_connect);
}


BEGIN_MESSAGE_MAP(CClient, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CClient::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_BUTTON2, &CClient::OnBnClickedCancel)
END_MESSAGE_MAP()


// CClient 메시지 처리기입니다.


void CClient::OnConnectServer(void)
{
	BYTE ipFirst, ipSecond, ipThird, ipForth;


	UpdateData();

	m_IpAddressCtrl.GetAddress(ipFirst, ipSecond, ipThird, ipForth);
	m_address.Format(_T("%d.%d.%d.%d"), ipFirst, ipSecond, ipThird, ipForth);

	m_clientSock.Create();
	m_clientSock.Connect(m_address, 30000);

	CPACKET tmp;
	
	tmp.msgType = 3;
	tmp.c_id = 0; // 아직 모르니깐.
	tmp.hello = 1; // hello 패킷
	tmp.bye = 0;

	m_clientSock.Send((LPCSTR *)&tmp, sizeof(CPACKET));
	// HELLO 패킷을 보냄.

	HandleClient();
}

void CClient::OnDisconnect(void)
{

	// BYE 패킷을 보냄
	m_clientSock.Close();
}


void CClient::OnBnClickedConnect() // Connect 버튼을 눌렀을 때
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_address.GetLength() == 0){
		AfxMessageBox(_T("IP 주소를 입력하세요."));
		return ;
	}

	m_cBtn_connect.EnableWindow(FALSE);
	OnConnectServer();
	m_connectFlag = true;
}


void CClient::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_cBtn_connect.EnableWindow(TRUE);
	OnDisconnect();
	m_connectFlag = false;
}


void CClient::HandleClient(void) // 메시지를 받고 바로 처리해주는 함수, unpackmessage, 이 함수 쓰레드로 돌려야 하나?
{
	KPACKET tmp;
	while(m_connectFlag != false){
		m_clientSock.Receive((LPCSTR *)&tmp, sizeof(KPACKET));

		switch(tmp.msgType){
		case 1: // keyboard event를 받았을 때
			keybd_event(tmp.keyCode, 0, 0, 0);
			TRACE("Keybd_event success\n");
			break;
		case 2: // mouse event를 받았을 때
			TRACE("Mouse Event\n");
			SetCursorPos(tmp.pad2, tmp.pad3);

			if(tmp.pad1 == 1 && tmp.updownFlag== 0){ // right up
				mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			} else if(tmp.pad1 == 1 && tmp.updownFlag == 1){ // right down
				mouse_event(MOUSEEVENTF_RIGHTDOWN,  0, 0, 0, 0);
			} else if(tmp.pad1 == 0 && tmp.updownFlag == 0){ // left up
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			} else if(tmp.pad1 == 0 && tmp.updownFlag == 1){ // left down
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			}
			if(tmp.keyCode == 2){ // wheel btn up
				mouse_event(MOUSEEVENTF_MIDDLEUP,  0, 0, 0, 0);
			} else if(tmp.keyCode == 2){ // wheel btn down
				mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
			} else if(tmp.keyCode == 3){ // wheel move
				mouse_event(MOUSEEVENTF_WHEEL,  0, 0, 0, 5);
			}

			break;
		case 3: // client
			if(tmp.deviceType == 1){ // hello packet에 대한 ACK가 왔을 때
				// Client ID를 부여받는다.
				client_ID = tmp.sendDev;

			} else if(tmp.deviceType == 2){ // bye 패킷을 받았을 때

				OnDisconnect();
				m_connectFlag = false;

			}

			break;
		case 4:
			break;

		default:
			break;
		}

	}
}
