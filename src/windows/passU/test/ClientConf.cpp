// ClientConf.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "ClientConf.h"
#include "afxdialogex.h"

#include <afxsock.h> // sock 함수 포함하는 라이브러리


// ClientConf 대화 상자입니다.

IMPLEMENT_DYNAMIC(ClientConf, CDialogEx)

	ClientConf::ClientConf(CWnd* pParent /*=NULL*/)
	: CDialogEx(ClientConf::IDD, pParent)
	, m_PortNum(_T(""))
{

	AfxSocketInit();
	clientID = 0;
	m_exitFlag = false;
}

ClientConf::~ClientConf()
{

}

void ClientConf::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Text(pDX, IDC_EDIT1, m_PortNum);
	DDV_MaxChars(pDX, m_PortNum, 5);
	DDX_Control(pDX, IDOK, m_CBtn_ClientConnect);
	DDX_Control(pDX, IDCANCEL, m_CBtn_ClientCancel);
}


BEGIN_MESSAGE_MAP(ClientConf, CDialogEx)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &ClientConf::OnIpnFieldchangedIpaddress)
	ON_BN_CLICKED(IDOK, &ClientConf::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &ClientConf::OnBnClickedCancel)
END_MESSAGE_MAP()


// ClientConf 메시지 처리기입니다.


void ClientConf::OnIpnFieldchangedIpaddress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);

	*pResult = 0;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void ClientConf::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE ipFirst, ipSecond, ipThird, ipForth;


	UpdateData();

	m_ip.GetAddress(ipFirst, ipSecond, ipThird, ipForth);
	m_address.Format(_T("%d.%d.%d.%d"), ipFirst, ipSecond, ipThird, ipForth);

	m_address.Format(_T("127.0.0.1"));
	int nPort;


	nPort = _ttoi(m_PortNum);

	nPort = 7000;

	connect(nPort);

	CPACKET c;

	c.msgType =3 ; // hello packet
	c.first = 1;

	clientSock.Send((LPCSTR *)&c, sizeof(CPACKET)); // hello Packet

}


void ClientConf::receiveData(CClientSocket *s)
{
	KPACKET tmp; // 구조체 임시 저장

	s->Receive((LPCSTR *)&tmp, sizeof(KPACKET));

	tmp = unpackMessage(tmp); // 메시지 언팩

	AfxMessageBox(_T("receiveData!!!"));
}


void ClientConf::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//	p.id = 

	//	sendData
	m_connectFlag = false;
	m_CBtn_ClientConnect.EnableWindow(TRUE);



	//	CDialogEx::OnCancel();
}

KPACKET ClientConf::packMessage(int msgType, int sendDev, int recvDev, int devType, int relativeField, int updownFlag, int pad1, int keyCode, int pad2, int pad3){
	keyboard.msgType = 0;
	keyboard.sendDev = 0;
	keyboard.recvDev = 0;
	keyboard.deviceType = 0;
	keyboard.relativeField = 0;
	keyboard.updownFlag = 0;
	keyboard.pad1 = 0;
	keyboard.keyCode = 0;
	keyboard.pad2 = 0;
	keyboard.pad3 = 0;

	switch(msgType){
	case 1: // Keyboard
		keyboard.msgType = msgType;
		keyboard.sendDev = sendDev;
		keyboard.recvDev = recvDev;
		keyboard.deviceType = devType;
		keyboard.relativeField = relativeField;
		keyboard.updownFlag = updownFlag;;
		keyboard.pad1 = pad1;
		keyboard.keyCode = keyCode;
		keyboard.pad2 = pad2;
		keyboard.pad3 = pad3;

		break;

	case 2: // Mouse
		mouse.msgType = msgType;
		mouse.sendDev = sendDev;
		mouse.recvDev = recvDev;
		mouse.deviceType = devType;
		mouse.relativeField = relativeField;
		mouse.updownFlag = updownFlag;
		mouse.leftRight = pad1;
		mouse.wheelFlag = keyCode;
		mouse.xCoord = pad2;
		mouse.yCoord = pad3;

		memcpy(&keyboard, &mouse, sizeof(KPACKET));

		break;

	case 3: // Client
		client.msgType = msgType;
		client.c_id = sendDev;
		client.first = recvDev;
		client.pad2 = devType;
		client.pad3 = relativeField;
		client.pad4 = updownFlag;
		client.pad5 = pad1;
		client.pad6 = keyCode;
		client.pad7 = pad2;
		client.pad8 = pad3;

		memcpy(&keyboard, &client, sizeof(KPACKET));
		break;

	case 4: // Data Pack
		data.msgType = msgType;
		data.len = sendDev;

		memcpy(&keyboard, &data, sizeof(KPACKET));

		break;

	}

	return keyboard;

}

KPACKET ClientConf::unpackMessage(KPACKET p){
	keyboard.msgType = 0;
	keyboard.sendDev = 0;
	keyboard.recvDev = 0;
	keyboard.deviceType = 0;
	keyboard.relativeField = 0;
	keyboard.updownFlag = 0;
	keyboard.pad1 = 0;
	keyboard.keyCode = 0;
	keyboard.pad2 = 0;
	keyboard.pad3 = 0;
	switch(p.msgType){
	case 1: // Keyboard
		keyboard.msgType = p.msgType;
		keyboard.sendDev = p.sendDev;
		keyboard.recvDev = p.recvDev;
		keyboard.deviceType = p.deviceType;
		keyboard.relativeField = p.relativeField;
		keyboard.updownFlag = p.updownFlag;
		keyboard.keyCode = p.keyCode;


	case 2: // Mouse
		
		mouse.msgType = p.msgType;
		mouse.sendDev = p.sendDev;
		mouse.recvDev = p.recvDev;
		mouse.deviceType = p.deviceType;
		mouse.relativeField = p.relativeField;
		mouse.updownFlag = p.updownFlag;
		mouse.leftRight = p.pad1;
		mouse.wheelFlag = p.keyCode;
		mouse.xCoord = p.pad2;
		mouse.yCoord = p.pad3;

		memcpy(&keyboard, &mouse, sizeof(KPACKET));

	case 3: // Client
		client.msgType = p.msgType;
		client.c_id = p.sendDev;
		clientID = client.c_id;


		memcpy(&keyboard, &mouse, sizeof(KPACKET));

	case 4: // Data
		data.msgType = p.msgType;
		data.len = p.sendDev;

		memcpy(&keyboard, &mouse, sizeof(KPACKET));

	}

	return keyboard;
}


BOOL ClientConf::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch(pMsg->message){
	case WM_KEYBOARD_MESSAGE:
		return TRUE;
		break;

	case WM_MOUSE_MESSAGE:
		return TRUE;
		break;

	case WM_CLIENT_MESSAGE:
		return TRUE;
		break;

	case WM_SERVER_MESSAGE:
		return TRUE;
		break;

	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void ClientConf::connect(int nPort)
{
	clientSock.Create();

	clientSock.Connect(m_address, 7000);


}


void ClientConf::cleanUp(void)
{

}

void ClientConf::closeSock(void)
{
	clientSock.Close();
}
