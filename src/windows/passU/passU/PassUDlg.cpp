
// PassUDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "PassU.h"
#include "PassUDlg.h"
#include "afxdialogex.h"
#include "PassUClientSocket.h"
#include "PassUTransparentDlg.h"
#include <Dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_TRAY_NOTIFICATION (WM_USER + 1023)

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.



	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//	afx_msg void OnTraymenuAbout();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPassUDlg 대화 상자



CPassUDlg::CPassUDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPassUDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPassUDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	DDX_Control(pDX, IDC_START, m_CBtn_Start);
	DDX_Control(pDX, IDC_STOP, m_CBtn_Stop);
}

BEGIN_MESSAGE_MAP(CPassUDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CPassUDlg::OnTcnSelchangeTab1)
	ON_MESSAGE(WM_TRAY_NOTIFICATION, &CPassUDlg::OnTrayNotification)
	ON_MESSAGE(WM_ARRIVED_SCREEN_EDGE, &CPassUDlg::OnArrivedScreenEdge)
	ON_WM_DESTROY()
	ON_WM_COPYDATA()
	ON_COMMAND(ID_TRAYMENU_OPEN, &CPassUDlg::OnTraymenuOpen)
	ON_COMMAND(ID_TRAYMENU_ABOUT, &CPassUDlg::OnTraymenuAbout)
	ON_WM_SIZE()
	ON_COMMAND(ID_TRAYMENU_CLOSE, &CPassUDlg::OnTraymenuClose)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_START, &CPassUDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CPassUDlg::OnBnClickedStop)
	//	ON_WM_INPUT()
END_MESSAGE_MAP()


// CPassUDlg 메시지 처리기

BOOL CPassUDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	tdlgIsCreated = FALSE;
	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	ShowCursorAll();

	CString tmp;
	tmp.Format(_T("SERVER"));
	m_Tab.InsertItem(0, tmp);
	tmp.Format(_T("CLIENT"));
	m_Tab.InsertItem(1, tmp);

	CRect rect;
	m_pServer = NULL;
	m_pClient = NULL;
	m_Tab.GetClientRect(&rect);
	m_tab1.Create(IDD_SERVER, &m_Tab);
	m_tab1.SetWindowPos(NULL, 5, 25, rect.Width() - 10 , rect.Height() - 30, SWP_SHOWWINDOW|SWP_NOZORDER);
	m_tab2.Create(IDD_CLIENT, &m_Tab);
	m_tab2.SetWindowPos(NULL, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	m_SorC = TRUE;
	m_pwndShow = &m_tab1;
	whereisPoint = 5;
	UpdateData(FALSE);
	m_CBtn_Stop.EnableWindow(FALSE);
	// 자신의 화면 구하기
	nWidth = GetSystemMetrics(SM_CXSCREEN);
	nHeight = GetSystemMetrics(SM_CYSCREEN);

	// 처음에는 클라이언트에 정보를 보내지 않는다.
	m_allowSend = FALSE;
	isCursorShow = TRUE;
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CPassUDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}

	// if((nID & 0xFFF0) == SC_MINIMIZE) ShowWindow(SW_HIDE);

	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPassUDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPassUDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPassUDlg::ShowCursorAll() {
	isCursorShow = TRUE;
	if(m_SorC && tdlgIsCreated) 
		tdlg->ShowWindow(SW_HIDE);
	SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
}

void CPassUDlg::HideCursorAll() {
	isCursorShow = FALSE;
	if(m_SorC && tdlgIsCreated)
		tdlg->ShowWindow(SW_SHOW);

	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32512);    // IDC_ARROW
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32513);    // IDC_IBEAM
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32514);    // IDC_WAIT
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32515);    // IDC_CROSS
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32516);    // IDC_UPARROW
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32640);    // IDC_SIZE        
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32641);    // IDC_ICON
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32642);    // IDC_SIZENWSE
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32644);    // IDC_SIZEWE
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32645);    // IDC_SIZENS
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32643);    // IDC_SIZENESW            
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32646);    // IDC_SIZEALL
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32648);    // IDC_NO    
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32649);    // IDC_NO    
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32650);    // IDC_APPSTARTING
	::SetSystemCursor(::LoadCursorFromFile("trans.cur"), 32651);    // IDC_HELP
}

void CPassUDlg::DestroyCursorAll() {
	SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
}

void CPassUDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if(m_pwndShow != NULL){
		m_pwndShow->ShowWindow(SW_HIDE);
		m_pwndShow = NULL;
	}
	int nIndex = m_Tab.GetCurSel();

	switch(nIndex){
	case 0:
		m_tab1.ShowWindow(SW_SHOW);
		m_pwndShow = &m_tab1;
		m_SorC = TRUE;
		if(m_pServer == NULL){
			m_CBtn_Start.EnableWindow(TRUE);
		}

		if(m_tab2.m_connectFlag == TRUE){
			m_CBtn_Stop.EnableWindow(FALSE);
			m_CBtn_Start.EnableWindow(FALSE);
		}

		if(m_pServer != NULL)
			m_CBtn_Stop.EnableWindow(TRUE);
		break;

	case 1:
		m_tab2.ShowWindow(SW_SHOW);
		m_pwndShow = &m_tab2;
		m_SorC = FALSE;
		m_CBtn_Start.EnableWindow(FALSE);
		if(m_pServer != NULL)
			m_CBtn_Stop.EnableWindow(FALSE);
		if(m_tab2.m_connectFlag == TRUE){
			m_CBtn_Stop.EnableWindow(TRUE);
		}
		break;
	}


	*pResult = 0;
}

void CPassUDlg::ReceiveData(CPassUChildSocket * s)
{
	char buf[4096];
	ZeroMemory(buf, sizeof(buf));

	int len = s->Receive(&buf, sizeof(buf));

	ParseData(buf, len);

}

int CPassUDlg::ParseData(char *buf, int len)
{
	int msgType;
	char msgTypeBuf[4];
	memcpy(msgTypeBuf, buf, sizeof(msgTypeBuf));
	msgType = byteToint(msgTypeBuf, 4);

	if(msgType == MSG_KEYBOARD) {
		KPACKET packet;

		ParseKeyboardData(buf, &packet);

		COPYDATASTRUCT CDS;

		CDS.dwData = 2; // receiveData
		CDS.cbData = sizeof(KPACKET);
		CDS.lpData = &packet;
		::SendMessage(m_tab1.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
	} else if(msgType == MSG_MOUSE) {
		MPACKET packet;

		ParseMouseData(buf, &packet);
		COPYDATASTRUCT CDS;

		CDS.dwData = 2; // receiveData
		CDS.cbData = sizeof(MPACKET);
		CDS.lpData = &packet;
		::SendMessage(m_tab1.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
	} else if(msgType == MSG_CLIENT) {
		CPACKET packet;

		ParseClientData(buf, &packet);

		COPYDATASTRUCT CDS;

		CDS.dwData = 2; // receiveData
		CDS.cbData = sizeof(CPACKET);
		CDS.lpData = &packet;
		::SendMessage(m_tab1.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
	} else if(msgType == MSG_DATA) {
		DPACKET packet;
		ZeroMemory(&packet, sizeof(DPACKET));

		memcpy(&packet.usbdesc, buf + sizeof(packet.msgType) + sizeof(packet.len), packet.len);

		COPYDATASTRUCT CDS;

		CDS.dwData = 2; // receiveData
		CDS.cbData = sizeof(DPACKET);
		CDS.lpData = &packet;
		::SendMessage(m_tab1.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
	} else {
		return -1;
	}
	return 0;
}

int CPassUDlg::ParseKeyboardData(char *buf, KPACKET *packet)
{
	char msgTypeBuf[4];

	memcpy(msgTypeBuf, buf, sizeof(msgTypeBuf));
	packet->msgType = byteToint(msgTypeBuf, 4);

	char sendDevBuf[4];
	memcpy(sendDevBuf, buf + sizeof(msgTypeBuf), sizeof(sendDevBuf));
	packet->sendDev = byteToint(sendDevBuf, 4);

	char recvDevBuf[4];
	memcpy(recvDevBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf), sizeof(recvDevBuf));
	packet->recvDev = byteToint(recvDevBuf, 4);

	char deviceTypeBuf[1];
	memcpy(deviceTypeBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf), sizeof(deviceTypeBuf));
	packet->deviceType = byteToint(deviceTypeBuf, 1);

	char relativeFieldBuf[1];
	memcpy(relativeFieldBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf) + sizeof(deviceTypeBuf), sizeof(relativeFieldBuf));
	packet->relativeField = byteToint(relativeFieldBuf, 1);

	char updownFlagBuf[1];
	memcpy(updownFlagBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf) + sizeof(deviceTypeBuf) + sizeof(relativeFieldBuf), sizeof(updownFlagBuf));
	packet->updownFlag = byteToint(updownFlagBuf, 1);

	char keyCodeBuf[4];
	memcpy(keyCodeBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf) + sizeof(deviceTypeBuf) + sizeof(relativeFieldBuf) + sizeof(updownFlagBuf)
		, sizeof(keyCodeBuf));
	packet->keyCode = byteToint(keyCodeBuf, 4);

	return 0;
}

int CPassUDlg::ParseMouseData(char *buf, MPACKET *packet)
{
	char msgTypeBuf[4];

	memcpy(msgTypeBuf, buf, sizeof(msgTypeBuf));
	packet->msgType = byteToint(msgTypeBuf, 4);

	char sendDevBuf[4];
	memcpy(sendDevBuf, buf + sizeof(msgTypeBuf), sizeof(sendDevBuf));
	packet->sendDev = byteToint(sendDevBuf, 4);

	char recvDevBuf[4];
	memcpy(recvDevBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf), sizeof(recvDevBuf));
	packet->recvDev = byteToint(recvDevBuf, 4);

	char deviceTypeBuf[1];
	memcpy(deviceTypeBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf), sizeof(deviceTypeBuf));
	packet->deviceType = byteToint(deviceTypeBuf, 1);

	char relativeFieldBuf[1];
	memcpy(relativeFieldBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf) + sizeof(deviceTypeBuf), sizeof(relativeFieldBuf));
	packet->relativeField = byteToint(relativeFieldBuf, 1);

	char updownFlagBuf[1];
	memcpy(updownFlagBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf) + sizeof(deviceTypeBuf) + sizeof(relativeFieldBuf), sizeof(updownFlagBuf));
	packet->updownFlag = byteToint(updownFlagBuf, 1);

	char leftRightBuf[1];
	memcpy(leftRightBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf) + sizeof(deviceTypeBuf) + sizeof(relativeFieldBuf) + sizeof(updownFlagBuf)
		, sizeof(leftRightBuf));
	packet->leftRight = byteToint(leftRightBuf, 1);

	char wheelFlagBuf[4];
	memcpy(wheelFlagBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf) + sizeof(deviceTypeBuf) + sizeof(relativeFieldBuf) + sizeof(updownFlagBuf)
		+ sizeof(leftRightBuf) , sizeof(wheelFlagBuf));
	packet->wheelFlag = byteToint(wheelFlagBuf, 4);

	char xCoordBuf[4];
	memcpy(xCoordBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf) + sizeof(deviceTypeBuf) + sizeof(relativeFieldBuf) + sizeof(updownFlagBuf)
		+ sizeof(leftRightBuf) + sizeof(wheelFlagBuf), sizeof(xCoordBuf));
	packet->xCoord = byteToint(xCoordBuf, 4);

	char yCoordBuf[4];
	memcpy(yCoordBuf, buf + sizeof(msgTypeBuf) + sizeof(sendDevBuf) + sizeof(recvDevBuf) + sizeof(deviceTypeBuf) + sizeof(relativeFieldBuf) + sizeof(updownFlagBuf)
		+ sizeof(leftRightBuf) + sizeof(wheelFlagBuf) + sizeof(xCoordBuf), sizeof(yCoordBuf));
	packet->yCoord = byteToint(yCoordBuf, 4);

	return 0;
}

int CPassUDlg::ParseClientData(char *buf, CPACKET *packet)
{
	char msgTypeBuf[4];

	memcpy(msgTypeBuf, buf, sizeof(msgTypeBuf));
	packet->msgType = byteToint(msgTypeBuf, 4);

	char c_idBuf[4];
	memcpy(c_idBuf, buf + sizeof(msgTypeBuf), sizeof(c_idBuf));
	packet->c_id = byteToint(c_idBuf, 4);

	char pad3Buf[4];
	memcpy(pad3Buf, buf + sizeof(msgTypeBuf) + sizeof(c_idBuf), sizeof(pad3Buf));
	packet->pad3 = byteToint(pad3Buf, 4);

	char helloBuf[1];
	memcpy(helloBuf, buf + sizeof(msgTypeBuf) + sizeof(c_idBuf) + sizeof(pad3Buf), sizeof(helloBuf));
	packet->hello = byteToint(helloBuf, 1);

	char byeBuf[1];
	memcpy(byeBuf, buf + sizeof(msgTypeBuf) + sizeof(c_idBuf) + sizeof(pad3Buf) + sizeof(helloBuf), sizeof(byeBuf));
	packet->bye = byteToint(byeBuf, 1);

	char ipFirstBuf[4];
	memcpy(ipFirstBuf, buf + sizeof(msgTypeBuf) + sizeof(c_idBuf) + sizeof(pad3Buf) + sizeof(helloBuf) + sizeof(byeBuf), 
		sizeof(ipFirstBuf));
	packet->ipFirst = byteToint(ipFirstBuf, 4);

	char ipSecondBuf[4];
	memcpy(ipSecondBuf, buf + sizeof(msgTypeBuf) + sizeof(c_idBuf) + sizeof(pad3Buf) + sizeof(helloBuf) + sizeof(byeBuf) 
		+ sizeof(ipFirstBuf), sizeof(ipSecondBuf));
	packet->ipSecond = byteToint(ipSecondBuf, 4);

	char ipThirdBuf[4];
	memcpy(ipThirdBuf, buf + sizeof(msgTypeBuf) + sizeof(c_idBuf) + sizeof(pad3Buf) + sizeof(helloBuf) + sizeof(byeBuf) 
		+ sizeof(ipFirstBuf) + sizeof(ipSecondBuf), sizeof(ipThirdBuf));
	packet->ipThird = byteToint(ipThirdBuf, 4);

	char ipForthBuf[4];
	memcpy(ipForthBuf, buf + sizeof(msgTypeBuf) + sizeof(c_idBuf) + sizeof(pad3Buf) + sizeof(helloBuf) + sizeof(byeBuf) 
		+ sizeof(ipFirstBuf) + sizeof(ipSecondBuf) + sizeof(ipThirdBuf), sizeof(ipForthBuf));
	packet->ipForth = byteToint(ipForthBuf, 4);

	char nWidth[5];
	memcpy(nWidth, buf + sizeof(msgTypeBuf) + sizeof(c_idBuf) + sizeof(pad3Buf) + sizeof(helloBuf) + sizeof(byeBuf) 
		+ sizeof(ipFirstBuf) + sizeof(ipSecondBuf) + sizeof(ipThirdBuf) + sizeof(ipForthBuf), sizeof(nWidth));
	packet->nWidth = byteToint(nWidth, 5);

	char nHeight[5];
	memcpy(nHeight, buf + sizeof(msgTypeBuf) + sizeof(c_idBuf) + sizeof(pad3Buf) + sizeof(helloBuf) + sizeof(byeBuf) 
		+ sizeof(ipFirstBuf) + sizeof(ipSecondBuf) + sizeof(ipThirdBuf) + sizeof(ipForthBuf) + sizeof(nWidth), sizeof(nHeight));
	packet->nHeight = byteToint(nHeight, 5);
	return 0;
}

int CPassUDlg::byteToint(char *data, int len) 
{
	int result = 0;
	for(int i = 0; i < len; i++) {
		if(data[i] == ' ')
			continue;
		result = result * 10 + (data[i] - '0');
	}
	return result;
}

void CPassUDlg::Accept(void)
{
	CPassUChildSocket *pChild = new CPassUChildSocket();

	BOOL check = m_pServer->Accept(*pChild);
	/* Nagle 알고리즘을 해제하는 코드, 우리 프로그램에서는 Nagle 알고리즘 필요없엉 */

	const char opt_val = true;
	setsockopt(*pChild, IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(opt_val));

	if(check == FALSE){
		AfxMessageBox(_T("Accept Failed"));
		return ;
	}

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = GetSafeHwnd();
	BOOL bRet = ::Shell_NotifyIcon(NIM_DELETE, &nid); //트레이아이콘 제거
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOW); //윈도우 활성화

	m_pSockList.AddTail(pChild);
}

void CPassUDlg::CleanUp(void)
{
	if(m_SorC){
		if(m_pServer == NULL)
			return ;
		m_pServer->Close();
		if(m_pServer)	m_pServer = NULL;

		m_tab1.m_waiting_client.DeleteAllItems();

		for(int i = 0 ; i < 9 ; i ++){
			int tmp = m_tab1.btn_Bind[i];
			m_tab1.btn_Bind[i] = 0;
			m_tab1.m_cBtn[i].SetBitmap(NULL);
			m_tab1.client_nWidth[i] = 0;
			m_tab1.client_nHeight[i] = 0;
			m_tab1.clientInfo[i].clientID = 0;
			m_tab1.clientInfo[i].setIP("0.0.0.0");
			m_tab1.clientInfo[i].setPosition(0);
			m_tab1.clientInfo[i].setStatus(STATUS_EMPTY);
		}

		UpdateData();
		Invalidate();

		CPassUChildSocket *pChild;

		while(!m_pSockList.IsEmpty()){
			pChild = (CPassUChildSocket *)m_pSockList.RemoveHead();
			delete pChild;
		}
	} else{
		if(m_pClient == NULL)
			return ;

		CPACKET tmp;

		tmp.bye = 1;
		tmp.c_id = m_tab2.client_ID;
		tmp.hello = 0;
		tmp.msgType = 3;

		char buf[1024];
		ZeroMemory(buf, sizeof(buf));
		sprintf_s(buf, "%4d%4d%4d%1d%1d%4d%4d%4d%4d%5d%5d",
			CONNECTED_CLIENT, m_tab2.client_ID, STATUS_PC, 0, 1, 0, 0, 0, 0, 0, 0);

		COPYDATASTRUCT CDS;

		CDS.dwData = CONNECTED_CLIENT;
		CDS.cbData = sizeof(CPACKET);
		CDS.lpData = &tmp;


		::SendMessage(m_tab2.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

		if(m_pClient)	m_pClient = NULL;
	}
}

void CPassUDlg::CloseChild(CPassUChildSocket *s)
{ // 클라이언트쪽에서 종료하였을 때 호출되는 함수
	int i;
	CPassUChildSocket *pChild;
	POSITION pos = m_pSockList.GetHeadPosition();

	CString tmpStr;
	ShowCursorAll();
	m_allowSend = FALSE;
	
	for(i = 0 ; i < 9 ; i ++){
		if(m_tab1.clientInfo[i].clientID == s->c_id){

			m_tab1.m_cBtn[((m_tab1.clientInfo[i]).getPosition())].SetBitmap(NULL);

			UpdateData();
			Invalidate();

			m_tab1.btn_Bind[(m_tab1.clientInfo[i]).getPosition()] = 0;
			::SendMessage(m_tab1.dllWnd, WM_CHILDCLOSE, (m_tab1.clientInfo[i]).getPosition(), 0);

			tmpStr.Format(_T("%d , IP : %s"), m_tab1.clientInfo[i].clientID, m_tab1.clientInfo[i].m_address);
			// 리스트 컨트롤에서 삭제
			LVFINDINFO find_item;
			find_item.flags = LVFI_STRING;
			find_item.psz = tmpStr;

			int index = m_tab1.m_waiting_client.FindItem(&find_item);
			if(-1 != index)	m_tab1.m_waiting_client.DeleteItem(index);

			m_tab1.clientInfo[i].setID(0);
			m_tab1.clientInfo[i].setIP("0.0.0.0");
			m_tab1.clientInfo[i].setPosition(0);
			m_tab1.clientInfo[i].setStatus(STATUS_EMPTY);
			break;
		}
	}
	
	while(pos != NULL){
		pChild = (CPassUChildSocket *)m_pSockList.GetAt(pos);

		if(pChild == s){
			m_pSockList.RemoveAt(pos);
			break;
		}
		m_pSockList.GetNext(pos);
	}
	whereisPoint = 5;
}

void CPassUDlg::OnStartServer()
{
	m_pServer = new CPassUServerSocket();
	m_pServer->Create(30000);
	m_pServer->Listen();

	//Nagle 알고리즘 해제
	const char opt_val = true;
	setsockopt(*m_pServer, IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(opt_val));

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));

	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = GetSafeHwnd();
	nid.uFlags = NIF_INFO | NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	lstrcpy( nid.szInfoTitle, _T( "PassU" ) ); //풍선의 타이틀.
	lstrcpy( nid.szInfo, "PassU 서버가 실행되었습니다." );
	lstrcpy(nid.szTip, "PassU Server");
	nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
	BOOL bRet = ::Shell_NotifyIcon(NIM_ADD,&nid); //트레이 아이콘 등록
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE); //윈도우 감추기
}

void CPassUDlg::OnDestroy()
{
	if(m_SorC){ // Server일 시
		delete tdlg;
		CleanUp();
	} else{
		CleanUp();
	}
	
	DestroyCursorAll();
	CDialogEx::OnDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CPassUDlg::OnConnectStart(void)
{
	m_pClient = new CPassUClientSocket();
	m_pClient->Create();
	m_pClient->Connect(m_tab2.m_address, 30000);

	m_tab1.strIpAddress;

	int ipFirst = 0, ipSecond = 0, ipThird = 0, ipForth = 0;
	int i = 0;

	while(m_tab1.strIpAddress.GetAt(i) != '.' ){
		ipFirst = ipFirst * 10 + (m_tab1.strIpAddress.GetAt(i) - '0');
		i ++ ;
	}

	i ++ ;

	while(m_tab1.strIpAddress.GetAt(i) != '.'){
		ipSecond = ipSecond * 10 + (m_tab1.strIpAddress.GetAt(i) - '0');
		i ++ ;
	}

	i ++ ;

	while(m_tab1.strIpAddress.GetAt(i) != '.'){
		ipThird = ipThird * 10 + (m_tab1.strIpAddress.GetAt(i) - '0');
		i ++ ;
	}

	i ++ ;

	while(m_tab1.strIpAddress.GetAt(i) != '.' && (m_tab1.strIpAddress.GetLength() != i)){
		ipForth = ipForth * 10 + (m_tab1.strIpAddress.GetAt(i) - '0');
		i ++ ;
	}


	char buf[1024];
	ZeroMemory(buf, sizeof(buf));
	sprintf_s(buf, "%4d%4d%4d%1d%1d%4d%4d%4d%4d%5d%5d",
		MSG_CLIENT, 0, STATUS_PC, 1, 0, ipFirst, ipSecond, ipThird, ipForth, nWidth, nHeight);

	if(m_pClient != NULL)
		m_pClient->Send((LPCSTR *)&buf, SIZEOFPACKET); // 헬로 패킷 보냄

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));

	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = GetSafeHwnd();
	nid.uFlags = NIF_INFO | NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	lstrcpy( nid.szInfoTitle, _T( "PassU" ) ); //풍선의 타이틀.
	lstrcpy( nid.szInfo, "PassU 클라이언트가 실행되었습니다." );
	lstrcpy(nid.szTip, "PassU Client");
	HideCursorAll();
	nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
	BOOL bRet = ::Shell_NotifyIcon(NIM_ADD,&nid); //트레이 아이콘 등록
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE); //윈도우 감추기
}

void CPassUDlg::ClientCleanUp(void)
{

}

void CPassUDlg::ReceiveClientData(CPassUClientSocket * s)
{
	char buf[4096];
	ZeroMemory(buf, sizeof(buf));
	s->Receive(&buf, sizeof(buf));


	int msgType;
	char msgTypeBuf[4];
	memcpy(msgTypeBuf, buf, sizeof(msgTypeBuf));
	msgType = byteToint(msgTypeBuf, 4);

	if(msgType == MSG_KEYBOARD) {
		KPACKET packet;

		ParseKeyboardData(buf, &packet);

		if(packet.updownFlag == 1)
			keybd_event(packet.keyCode, 0, KEYEVENTF_KEYUP, 0);
		else if(packet.updownFlag == 0){ // down
			keybd_event(packet.keyCode, 0, 0, 0);
		}
	} else if(msgType == MSG_MOUSE) {
		MPACKET packet;

		ParseMouseData(buf, &packet);

		if(CursorFlag == TRUE) {

		} else {
			if(packet.xCoord <= 2 ||
				packet.yCoord <= 2 ||
				packet.xCoord >= nWidth - 2 ||
				packet.yCoord >= nHeight - 2 ) {
				HideCursorAll();
			} else {
				ShowCursorAll();
			}
		}

		SetCursorPos(packet.xCoord, packet.yCoord);

		if(packet.leftRight == 1 && packet.updownFlag== 0){ // right up
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
		} else if(packet.leftRight == 1 && packet.updownFlag == 1){ // right down
			mouse_event(MOUSEEVENTF_RIGHTDOWN,  0, 0, 0, 0);
		} else if(packet.leftRight == 0 && packet.updownFlag == 0){ // left up
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		} else if(packet.leftRight == 0 && packet.updownFlag == 1){ // left down
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		}
		if(packet.wheelFlag == 1){ // wheel btn up
			mouse_event(MOUSEEVENTF_MIDDLEUP,  0, 0, 0, 0);
		} else if(packet.wheelFlag == 2){ // wheel btn down
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
		} else if(packet.wheelFlag == 3){ // wheel move
			int tmp = 120;
			mouse_event(MOUSEEVENTF_WHEEL,  0, 0, tmp, 0);
		} else if(packet.wheelFlag == 4){ // wheel move
			int tmp = -120;
			mouse_event(MOUSEEVENTF_WHEEL,  0, 0, tmp, 0);
		}
	} else if(msgType == MSG_CLIENT){
		CPACKET packet;
		ZeroMemory( &packet, sizeof(CPACKET));
		ParseClientData(buf, &packet);

		COPYDATASTRUCT CDS;

		CDS.dwData = CONNECTED_CLIENT; // client
		CDS.cbData = sizeof(CPACKET);
		CDS.lpData = &packet;
		::SendMessage(m_tab2.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

	} else if(msgType == MSG_DATA && USE_USB == TRUE) {
		DPACKET packet;
		ZeroMemory(&packet, sizeof(DPACKET));

		packet.msgType = msgType;

		char lenBuf[4];
		memcpy(lenBuf, buf + sizeof(packet.msgType), sizeof(lenBuf));
		packet.len = byteToint(lenBuf, 4);

		memcpy(&packet.usbdesc, buf + sizeof(packet.msgType) + sizeof(packet.len), packet.len);

		COPYDATASTRUCT CDS;

		CDS.dwData = START_SERVER; // receiveData
		CDS.cbData = sizeof(DPACKET);
		CDS.lpData = &packet;
		::SendMessage(m_tab2.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
	} else if(msgType == MSG_REMOVE_USB && USE_USB == TRUE) {
		DPACKET packet;
		ZeroMemory(&packet, sizeof(DPACKET));

		COPYDATASTRUCT CDS;

		CDS.dwData = STOP_SERVER; // receiveData
		CDS.cbData = sizeof(DPACKET);
		CDS.lpData = &packet;
		::SendMessage(m_tab2.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
	} else if(msgType == MSG_RECEIVE) {
		int a= 0;
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32512);    // IDC_ARROW
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32513);    // IDC_IBEAM
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32514);    // IDC_WAIT
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32515);    // IDC_CROSS
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32516);    // IDC_UPARROW
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32640);    // IDC_SIZE        
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32641);    // IDC_ICON
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32642);    // IDC_SIZENWSE
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32644);    // IDC_SIZEWE
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32645);    // IDC_SIZENS
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32643);    // IDC_SIZENESW            
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32646);    // IDC_SIZEALL
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32648);    // IDC_NO    
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32649);    // IDC_NO    
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32650);    // IDC_APPSTARTING
		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32651);    // IDC_HELP

		CursorFlag = TRUE;
		cT = CTime::GetCurrentTime();
		htimer = SetTimer(1, 1, NULL);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	} else if(msgType == MSG_RETURN) {
		int a = 1;
	}
}

void CPassUDlg::CloseClient(CPassUClientSocket * s)
{
	COPYDATASTRUCT CDS;
	CPACKET tmp;

	CDS.dwData = STOP_SERVER;
	CDS.cbData = sizeof(CPACKET);
	CDS.lpData = &tmp;

	::SendMessage(m_tab2.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

	m_pClient->Close();
	if(m_pClient)	delete m_pClient;
	m_tab2.m_cBtn_connect.EnableWindow(TRUE);
	m_tab2.ipFirst = 0;
	m_tab2.ipSecond = 0;
	m_tab2.ipThird = 0;
	m_tab2.ipForth = 0;
	m_tab2.m_IpAddressCtrl.ClearAddress();

	m_tab2.m_connectFlag = false;
	m_CBtn_Start.EnableWindow(FALSE);
	m_CBtn_Stop.EnableWindow(FALSE);
}

BOOL CPassUDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	HEVENT *hEVENT;
	MPACKET *mEVENT;

	POSITION pos = m_pSockList.GetHeadPosition();
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(pCopyDataStruct->dwData){
	case KEYBOARD_DATA:
		hEVENT = (tagHEVENT *) pCopyDataStruct->lpData; // hEvent 구조체 연결(후킹된 자료)

		if(hEVENT->lParam >= 0){ // 키가 눌렸을 때
			if(m_allowSend == TRUE){
				char buf[1024];
				ZeroMemory(buf, sizeof(buf));
				sprintf_s(buf, "%4d%4d%4d%1d%1d%1d%4d%8d",
					MSG_KEYBOARD, 0, 0, 0,
					0, hEVENT->updown, hEVENT->keyCode, 0);

				CPassUChildSocket *s = (CPassUChildSocket *)m_pSockList.GetAt(pos);

				while((m_tab1.btn_Bind[whereisPoint - 1] != s->c_id) && (pos != NULL)){
					(CPassUChildSocket *)m_pSockList.GetNext(pos);
					s = (CPassUChildSocket *)m_pSockList.GetAt(pos);
				}

				if(pos != NULL)
					((CPassUChildSocket *)m_pSockList.GetAt(pos))->Send(buf, SIZEOFPACKET);
				break;
			}
		}
		break;

	case MOUSE_DATA:
		mEVENT = (MPACKET *)pCopyDataStruct->lpData; // mEvent 구조체 연결(후킹된 자료)
		int i = 0;

		if(m_allowSend == TRUE){ // 평소에는 클라이언트에 전송을 안하다가 서버가 아닌 다른곳으로 갔을때만 한다.
			// 클라이언트 좌표 전환. 
			// 서버 해상도 : 클라이언트 해상도 = 서버좌표 : 클라이언트 좌표 형식으로 변환
			// Client 좌표 = (서버좌표 * 클라이언트 해상도[ Client ID ]) / 서버해상도
			mEVENT->xCoord = (mEVENT->xCoord * m_tab1.client_nWidth[m_tab1.btn_Bind[whereisPoint - 1]]) / nWidth;
			mEVENT->yCoord = (mEVENT->yCoord * m_tab1.client_nHeight[m_tab1.btn_Bind[whereisPoint - 1]]) / nHeight;

			char buf[1024];
			ZeroMemory(buf, sizeof(buf));
			sprintf_s(buf, "%4d%4d%4d%1d%1d%1d%1d%4d%4d%4d%8d",
				MSG_MOUSE, mEVENT->sendDev, mEVENT->recvDev, mEVENT->deviceType,
				mEVENT->relativeField, mEVENT->updownFlag, mEVENT->leftRight, mEVENT->wheelFlag, mEVENT->xCoord, mEVENT->yCoord, 0);
			TRACE("x : %d, y : %d\n", mEVENT->xCoord, mEVENT->yCoord);
			CPassUChildSocket * s = (CPassUChildSocket *)m_pSockList.GetAt(pos);

			while((m_tab1.btn_Bind[whereisPoint - 1] != s->c_id) && (pos != NULL)){
				(CPassUChildSocket *)m_pSockList.GetNext(pos);
				s =  ((CPassUChildSocket *)m_pSockList.GetAt(pos));
			}
			if(pos != NULL)
					((CPassUChildSocket *)m_pSockList.GetAt(pos))->Send(buf, SIZEOFPACKET);
		}

		break;
	}
	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}

LRESULT CPassUDlg::OnArrivedScreenEdge(WPARAM wParam, LPARAM lParam)
{
	POSITION pos = m_pSockList.GetHeadPosition();

	BOOL forClient = wParam;
	whereisPoint = lParam;
	if(forClient == TRUE) {// SEND 할 수 있도록 바꿔주자
		char buf[1024];
		ZeroMemory(buf, sizeof(buf));
		sprintf_s(buf, "%4d", MSG_RECEIVE);

		CPassUChildSocket * s = (CPassUChildSocket *)m_pSockList.GetAt(pos);

		while((m_tab1.btn_Bind[whereisPoint - 1] != s->c_id) && (pos != NULL)){
			(CPassUChildSocket *)m_pSockList.GetNext(pos);
			s =  ((CPassUChildSocket *)m_pSockList.GetAt(pos));
		} 
		if(pos != NULL)
			((CPassUChildSocket *)m_pSockList.GetAt(pos))->Send(buf, SIZEOFPACKET);

		m_allowSend = TRUE;
		HideCursorAll();
	} else if(forClient == FALSE) {// SEND 하지 않도록 바꿔주자
		
		m_allowSend = FALSE;
		ShowCursorAll();

		::SetSystemCursor(::LoadCursorFromFile("animated_pasu.ani"), 32512);    // IDC_ARROW
		CursorFlag = TRUE;
		cT = CTime::GetCurrentTime();
		htimer = SetTimer(1, 1, NULL);
	}
	return 1;
}

BOOL CPassUDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	} 

	return CDialogEx::PreTranslateMessage(pMsg);
}


LRESULT CPassUDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(USE_USB == TRUE && m_SorC == TRUE && message == WM_DEVICECHANGE && m_pSockList.GetCount() != 0){
		TRACE("USB 꽂았음\n");
		USBDeviceChange(wParam, lParam);
	}
	return CDialogEx::DefWindowProc(message, wParam, lParam);
}

BOOL CPassUDlg::USBDeviceChange(UINT uEvent, DWORD dwEventData)
{
	COPYDATASTRUCT CDS;
	CPACKET packet;

	switch (uEvent)
	{
	case DBT_DEVICEARRIVAL:
		// 새로운 장치 발생
		CDS.dwData = NEW_USB; // Send New Device
		CDS.cbData = sizeof(CPACKET);
		CDS.lpData = &packet;
		::SendMessage(m_tab1.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
		break;
	case DBT_DEVICEREMOVECOMPLETE:
		// 장치 연결 해제
		CPACKET packet;
		CDS.dwData = REMOVE_USB; // Remove Device
		CDS.cbData = sizeof(CPACKET);
		CDS.lpData = &packet;
		::SendMessage(m_tab1.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
		break;
	default:
		break;
	}
	return TRUE;
}

LRESULT CPassUDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
	case WM_LBUTTONDOWN:
		OnTraymenuOpen();

		break;
	case WM_RBUTTONDOWN:
		{
			CPoint ptMouse;
			::GetCursorPos(&ptMouse);
			CMenu menu;
			menu.LoadMenu(IDR_TRAY_MENU);
			CMenu *pMenu = menu.GetSubMenu(0); //활성화 할 메뉴 지정
			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ptMouse.x, ptMouse.y, AfxGetMainWnd());
		}
		break;
	}
	return 1;
}

void CPassUDlg::OnTraymenuOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = GetSafeHwnd();
	BOOL bRet = ::Shell_NotifyIcon(NIM_DELETE, &nid); //트레이아이콘 제거

	AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOW); //윈도우 활성화
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWNORMAL);  // 최대크기만들기
}

void CPassUDlg::OnTraymenuClose()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);
}

void CPassUDlg::OnTraymenuAbout()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CPassUDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(nType == SIZE_MINIMIZED) {
		NOTIFYICONDATA nid;
		ZeroMemory(&nid, sizeof(nid));

		nid.cbSize = sizeof(nid);
		nid.uID = 0;
		nid.hWnd = GetSafeHwnd();
		nid.uFlags = NIF_INFO | NIF_ICON | NIF_TIP | NIF_MESSAGE;
		nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		lstrcpy( nid.szInfoTitle, _T( "PassU" ) ); //풍선의 타이틀.
		lstrcpy( nid.szInfo, "PassU가 최소화 되었습니다.\n화면을 다시 보려면 클릭하세요." );
		lstrcpy( nid.szTip, "PassU" );
		nid.uCallbackMessage = WM_TRAY_NOTIFICATION;
		BOOL bRet = ::Shell_NotifyIcon(NIM_ADD,&nid); //트레이 아이콘 등록
		AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE); //윈도우 감추기
	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CPassUDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	bWait = FALSE;

	if(CursorFlag == TRUE)
	{

		CTime cT2 = CTime::GetCurrentTime();
		CTimeSpan ts = cT2 - cT;

		if(ts.GetTotalSeconds() > 2)
		{
			CursorFlag = FALSE;
			if(isCursorShow)
				SystemParametersInfo(SPI_SETCURSORS,0,NULL,0);
			KillTimer(htimer);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CPassUDlg::OnBnClickedStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_SorC){
		tdlg = new CPassUTransparentDlg;
		tdlg->Create(IDD_TRANSPARENT, GetDesktopWindow());
		tdlg->ShowWindow(SW_HIDE);
		tdlgIsCreated = TRUE;
		OnStartServer();
		m_tab2.EnableWindow(FALSE);
	} else{
		OnConnectStart();
		m_tab2.m_CBtn_Cancel.EnableWindow(FALSE);
		m_tab2.m_connectFlag = TRUE;
		m_tab1.EnableWindow(FALSE);
	}

	m_CBtn_Start.EnableWindow(FALSE);
	m_CBtn_Stop.EnableWindow(TRUE);
}

void CPassUDlg::OnBnClickedStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if(m_SorC){
		m_CBtn_Start.EnableWindow(TRUE);
		m_CBtn_Stop.EnableWindow(FALSE);

		::SendMessage(m_tab1.dllWnd, WM_INPUTDEVICE_TRUE, 0, 0);
	} else{
		m_CBtn_Start.EnableWindow(FALSE);
		m_CBtn_Stop.EnableWindow(FALSE);
		m_tab2.m_CBtn_Cancel.EnableWindow(TRUE);

		DPACKET packet;
		ZeroMemory(&packet, sizeof(DPACKET));

		COPYDATASTRUCT CDS;

		CDS.dwData = STOP_SERVER; // receiveData
		CDS.cbData = sizeof(DPACKET);
		CDS.lpData = &packet;
		::SendMessage(m_tab2.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
	}

	m_tab2.EnableWindow(TRUE);
	m_tab1.EnableWindow(TRUE);
	CleanUp();
	//CDialog::OnCancel();
}

