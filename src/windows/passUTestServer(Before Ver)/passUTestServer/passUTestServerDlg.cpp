
// passUTestServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "passUTestServer.h"
#include "passUTestServerDlg.h"
#include "PassUListen.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define ECHOMAX 1024

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
UINT ReceiveData(LPVOID pParam);

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


// CpassUTestServerDlg 대화 상자


HANDLE thr;
unsigned long id1;

CpassUTestServerDlg::CpassUTestServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CpassUTestServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CpassUTestServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
}

BEGIN_MESSAGE_MAP(CpassUTestServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CpassUTestServerDlg 메시지 처리기

BOOL CpassUTestServerDlg::OnInitDialog()
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
	CPassUListen listen;
	CSocket client;
	// 초기화
	if(!AfxSocketInit()) {
		AfxMessageBox(_T("Socket 초기화 실패"));
	}
	// 포트 열기 : 9000번
	if(!listen.Create(9000)) {
		AfxMessageBox(_T("Port 열기 실패"));
	}
	// Listen 
	if(!listen.Listen())  {
		AfxMessageBox(_T("Listen 실패"));
	}
	// Aceept
	if(!listen.Accept(client)) {
		AfxMessageBox(_T("Accept 실패"));
	}
	int bytes, i=0;
	char buf[20];
	ZeroMemory(buf, sizeof(buf));
	strcpy_s(buf, "250 Conect OK. Start Message!");
	// Send로 데이터 전송 
	bytes = client.Send(buf, strlen(buf));
	while( i != 10) {                          
		ZeroMemory(buf, sizeof(buf));
		// 데이터 받기 시작
		bytes = client.Receive(buf, sizeof(buf));
		// 데이터 출력
		TRACE(buf);
	}
	client.Close();
	listen.Close();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CpassUTestServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CpassUTestServerDlg::OnPaint()
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
HCURSOR CpassUTestServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CpassUTestServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)    { // if message is key down
		CString key;

		switch (pMsg->wParam){
		case VK_UP:     // if Up arrow  
			key = "1";
			break;
		case VK_DOWN:   // if down arrow 
			key = "2";
			break;
		case VK_LEFT:   // if down arrow 
			key = "3";
			break;
		case VK_RIGHT:   // if down arrow 
			key = "4";
			break;
		} // end of switch

		CSocket passUClient;  

		if (passUClient.Create(0,SOCK_DGRAM,NULL) == 0) {
			AfxMessageBox(_T("Create() failed"));
		}

		if (passUClient.SendTo((void *)&key, sizeof(key),3737,
			(LPCTSTR)_T("223.62.180.126"), 0) != sizeof(key)) {
				AfxMessageBox(_T("SendTo() sent a different number of bytes than expected"));
		}
		passUClient.Close();
	} // end of if
	return CDialogEx::PreTranslateMessage(pMsg);
}