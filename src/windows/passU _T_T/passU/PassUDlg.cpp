
// PassUDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "PassU.h"
#include "PassUDlg.h"
#include "afxdialogex.h"
#include "PassUClientSocket.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	DDX_Control(pDX, IDC_BUTTON1, m_CBtn_Start);
	DDX_Control(pDX, IDC_BUTTON2, m_CBtn_Stop);
}

BEGIN_MESSAGE_MAP(CPassUDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CPassUDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_BUTTON1, &CPassUDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPassUDlg::OnBnClickedButton2)
	ON_WM_DESTROY()
	ON_WM_COPYDATA()
END_MESSAGE_MAP()


// CPassUDlg 메시지 처리기

BOOL CPassUDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	CString tmp;
	tmp.Format(_T("SERVER"));
	m_Tab.InsertItem(0, tmp);
	tmp.Format(_T("CLIENT"));
	m_Tab.InsertItem(1, tmp);

	CRect rect;

	m_Tab.GetClientRect(&rect);
	m_tab1.Create(IDD_SERVER, &m_Tab);
	m_tab1.SetWindowPos(NULL, 5, 25, rect.Width() - 10 , rect.Height() - 30, SWP_SHOWWINDOW|SWP_NOZORDER);
	m_tab2.Create(IDD_CLIENT, &m_Tab);
	m_tab2.SetWindowPos(NULL, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);
	nWidth = GetSystemMetrics(SM_CXSCREEN);
	nHeight = GetSystemMetrics(SM_CYSCREEN);

	m_pwndShow = &m_tab1;
	whereisPoint = 5;
	UpdateData(FALSE);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CPassUDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
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
		break;

	case 1:
		m_tab2.ShowWindow(SW_SHOW);
		m_pwndShow = &m_tab2;
		m_SorC = FALSE;
		break;
	}


	*pResult = 0;
}

void CPassUDlg::ReceiveData(CPassUChildSocket * s)
{
	PACKET tmp;
	s->Receive(&tmp, sizeof(PACKET));

	COPYDATASTRUCT CDS;

	CDS.dwData = 2; // receiveData
	CDS.cbData = sizeof(PACKET);
	CDS.lpData = &tmp;


	::SendMessage(m_tab1.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

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

	m_pSockList.AddTail(pChild);

	AfxMessageBox(_T("Accept COmplete!!"));
}

void CPassUDlg::CleanUp(void)
{
	if(m_pServer)	delete m_pServer;

	CPassUChildSocket *pChild;

	while(!m_pSockList.IsEmpty()){
		pChild = (CPassUChildSocket *)m_pSockList.RemoveHead();
		delete pChild;
	}
	AfxMessageBox(_T("Clean Up!"));
}

void CPassUDlg::CloseChild(CPassUChildSocket *s){

	CPassUChildSocket *pChild;
	POSITION pos = m_pSockList.GetHeadPosition();
	while(pos != NULL){
		pChild = (CPassUChildSocket *)m_pSockList.GetAt(pos);

		if(pChild == s){
			m_pSockList.RemoveAt(pos);
			delete	pChild;
			break;
		}
		m_pSockList.GetNext(pos);
	}
}

void CPassUDlg::OnStartServer()
{
	m_pServer = new CPassUServerSocket();
	m_pServer->Create(30000);
	m_pServer->Listen();
	const char opt_val = true;
	setsockopt(*m_pServer, IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(opt_val));
	AfxMessageBox(_T("InitServer"));
}

void CPassUDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_SorC){
		OnStartServer();
	} else{
		OnConnectStart();
	}

	m_CBtn_Start.EnableWindow(FALSE);
}


void CPassUDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_CBtn_Start.EnableWindow(TRUE);
	CleanUp();
	//CDialog::OnCancel();
}


void CPassUDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CPassUDlg::OnConnectStart(void)
{
	CPACKET tmp;

	m_pClient = new CPassUClientSocket();
	m_pClient->Create();
	m_pClient->Connect(m_tab2.m_address, 30000);

	tmp.msgType = 3;
	tmp.bye = 0;
	tmp.c_id = 0;
	tmp.hello = 1;

	tmp.pad3 = STATUS_PC;

	tmp.pad6 = m_tab2.ipFirst;
	tmp.pad7 = m_tab2.ipSecond;
	tmp.pad8 = m_tab2.ipThird;
	tmp.pad9 = m_tab2.ipForth;

	m_pClient->Send((LPCSTR *)&tmp, sizeof(CPACKET)); // 헬로 패킷 보냄


}


void CPassUDlg::ClientCleanUp(void)
{
	if(m_pClient)	delete m_pClient;
}


void CPassUDlg::ReceiveClientData(CPassUClientSocket * s)
{
	PACKET tmp;

	s->Receive(&tmp, sizeof(PACKET));

	if(tmp.msgType == MSG_KEYBOARD){
		if(tmp.updownFlag == 1) // up
			keybd_event(tmp.keyCode, 0, KEYEVENTF_KEYUP, 0);

		else if(tmp.updownFlag == 0) // down
			keybd_event(tmp.keyCode, 0, 0, 0);

		TRACE("Keybd_event success\n");
		return ;
	} else if(tmp.msgType == MSG_MOUSE){
		if(tmp.pad2 > nWidth)
			tmp.pad2 = nWidth;
		if(tmp.pad3 > nHeight)
			tmp.pad3 = nHeight;

		mouse_Move(tmp.pad2, tmp.pad3);

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
			mouse_event(MOUSEEVENTF_WHEEL,  0, 0, 120, 0);
		}
		TRACE("mouse_event success\n");
		return ;
	}

	COPYDATASTRUCT CDS;

	CDS.dwData = 0; // Client receiveData
	CDS.cbData = sizeof(PACKET);
	CDS.lpData = &tmp;


	::SendMessage(m_tab2.GetSafeHwnd(), WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

}


void CPassUDlg::CloseClient(CPassUClientSocket * s)
{
	CPACKET tmp;
	tmp.msgType = 3;
	tmp.c_id = m_tab2.client_ID;
	tmp.bye = 1;
	tmp.hello = 0;


	s->Send((LPCSTR *)&tmp, sizeof(CPACKET)); // bye 패킷 전송

	ClientCleanUp();
}


BOOL CPassUDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	HEVENT *hEVENT;
	MPACKET *mEVENT;

	

	// Client한테 전송할 구조체(K,M : 후킹자료)
	KPACKET keyP;
	MPACKET mouseP;

	COPYDATASTRUCT CDS;
	POSITION pos = m_pSockList.GetHeadPosition();
	TRACE("WhereisPoint : %d\n", whereisPoint);
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(pCopyDataStruct->dwData){
	case KEYBOARD_DATA:
		hEVENT = (tagHEVENT *) pCopyDataStruct->lpData; // hEvent 구조체 연결(후킹된 자료)

		if(hEVENT->lParam >= 0){ // 키가 눌렸을 때
			TRACE("KEY CODE 도착\n");

			for(int i = 0 ; i < 9 ; i ++){
				//TRACE("m_tab1.btn_Bind[i] : %d\n", m_tab1.btn_Bind[i]);
				if((m_tab1.btn_Bind[i]) != 0){
					keyP.deviceType = 1;
					keyP.msgType = MSG_KEYBOARD;
					keyP.keyCode = hEVENT->keyCode;
					keyP.updownFlag = hEVENT->updown;
					((CPassUClientSocket *)m_pSockList.GetAt(pos))->Send((LPCSTR *)&keyP, sizeof(KPACKET));
					break;
				} 
			}
		}
		break;

	case MOUSE_DATA:
		mEVENT = (MPACKET *)pCopyDataStruct->lpData; // mEvent 구조체 연결(후킹된 자료)
		TRACE("MOUSE DATA 도착\n");
		for(int i = 0 ; i < 9 ; i ++){
			//TRACE("m_tab1.btn_Bind[i] : %d\n", m_tab1.btn_Bind[i]);
			if((m_tab1.btn_Bind[i]) != 0){
				if(mEVENT->xCoord <= 2){ // 화면 왼쪽에 붙을 때
					if(whereisPoint == 5){ // 바인딩이 3에 되어 있을 때(4번 버튼)
						mEVENT->xCoord = nWidth - 15;
						mouse_Move(mEVENT->xCoord, mEVENT->yCoord);
				//		whereisPoint = 4;
						//SetCursor(NULL);
					//	ShowCursor(FALSE);

						// m_tab1.btn_bind[4] = 1; -> 서버가 아닌 다른 곳에 커서가 있음.
						// mouseevent 서버에서는 더이상 그냥 리턴해주고 그렇게 하도록
						// 좌표값이랑 이벤트값 전송은 그대로 해주고.
					} else if(whereisPoint == 6){//mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, (nWidth - 15)* 65535 / nWidth, mEVENT->yCoord * 65535 / nHeight, 0, 0);
						mEVENT->xCoord = nWidth - 15;
						mouse_Move(mEVENT->xCoord, mEVENT->yCoord);
				//		whereisPoint = 5;
						//	SetCursor();
					//	ShowCursor(TRUE);		
					}
				}

				if(mEVENT->yCoord<= 2) { // 화면 위족에 붙을 때
					if(whereisPoint == 5){ // 바인딩이 2번버튼에있을때
						mEVENT->yCoord = nHeight - 15;
						mouse_Move(mEVENT->xCoord, mEVENT->yCoord);
						//			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, mEVENT->xCoord * 65535 / nWidth, (nHeight - 15) * 65535 / nHeight, 0, 0);
					//	whereisPoint = 2;
					//	ShowCursor(FALSE);
					} else if(whereisPoint == 8){
						mEVENT->yCoord = nHeight - 15;
						mouse_Move(mEVENT->xCoord, mEVENT->yCoord);
						//			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, mEVENT->xCoord * 65535 / nWidth, (nHeight - 15) * 65535 / nHeight, 0, 0);
				//		whereisPoint = 5;
				//		ShowCursor(TRUE);

					}
				} 

				if(mEVENT->xCoord >= nWidth - 2){	 // 화면 오른 쪽에 붙을 때
					//	m_whereisPoint = 6;
					if(whereisPoint == 5){ // 바인딩이 6번버튼에 있을 때
						mEVENT->xCoord = 15;
						mouse_Move(mEVENT->xCoord, mEVENT->yCoord);
					//	whereisPoint = 6;
						//ShowCursor(FALSE);
					} else if(whereisPoint == 4){
						mEVENT->xCoord = 15;
						mouse_Move(mEVENT->xCoord, mEVENT->yCoord);
					//	whereisPoint = 5;
						//ShowCursor(TRUE);

					}
				}

				if(mEVENT->yCoord >= nHeight - 2){ // 화면 아래쪽에 붙을 때
					if(whereisPoint == 5){ // 바인딩이 8번 버튼에 있을 때
						mEVENT->yCoord = 15;
						mouse_Move(mEVENT->xCoord, mEVENT->yCoord);
					//	whereisPoint = 8;
					//	ShowCursor(FALSE);
					} else if(whereisPoint == 2){
						mEVENT->yCoord = 15;
						mouse_Move(mEVENT->xCoord, mEVENT->yCoord);
					//	whereisPoint = 5;
					//	ShowCursor(TRUE);

					}
				}

				TRACE("%d %d\n", mEVENT->xCoord, mEVENT->yCoord);


				mouseP.msgType = MSG_MOUSE;
				mouseP.deviceType = mEVENT->deviceType;
				mouseP.leftRight = mEVENT->leftRight;
				mouseP.wheelFlag = mEVENT->wheelFlag;
				mouseP.updownFlag = mEVENT->updownFlag;
				mouseP.xCoord = mEVENT->xCoord;
				mouseP.yCoord = mEVENT->yCoord;

				((CPassUClientSocket *)m_pSockList.GetAt(pos))->Send((LPCSTR *)&mouseP, sizeof(MPACKET));
				break;
			} 
		}
		break;
	}
	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}


BOOL CPassUDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message  == WM_KEYDOWN)
		return TRUE;



	return CDialogEx::PreTranslateMessage(pMsg);
}


void CPassUDlg::mouse_Move(int x, int y)
{
   x = (65535 * x / GetSystemMetrics(SM_CXSCREEN));

   y = (65535 * y / GetSystemMetrics(SM_CYSCREEN));

   

   mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE,x,y,0,NULL);

}
