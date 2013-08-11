
// passUTestServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "passUTestServer.h"
#include "passUTestServerDlg.h"
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
	thr = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveData, this, NULL, &id1);

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

UINT ReceiveData(LPVOID pParam)
{
	CpassUTestServerDlg *dlg = (CpassUTestServerDlg*)pParam;
	AfxSocketInit(NULL);
	CSocket echoServer;  

	// Create socket for sending/receiving datagrams
	if (echoServer.Create(3737, SOCK_DGRAM, NULL)== 0) {
		AfxMessageBox(_T("Create() failed"));
	}

	for(;;) { // Run forever
		// Client address
		SOCKADDR_IN echoClntAddr; 

		// Set the size of the in-out parameter
		int clntAddrLen = sizeof(echoClntAddr);

		// Buffer for echo string
		char echoBuffer[ECHOMAX];

		// Block until receive message from a client
		int recvMsgSize = echoServer.ReceiveFrom(echoBuffer, 
			ECHOMAX, (SOCKADDR*)&echoClntAddr, &clntAddrLen, 0);
		if (recvMsgSize < 0) {
			AfxMessageBox(_T("RecvFrom() failed"));
		}

		int ret;
		if ((ret = echoServer.SendTo(_T("NICE"), sizeof(_T("NICE")), (SOCKADDR*)&echoClntAddr,
			clntAddrLen, 0)) != sizeof(_T("NICE"))) {
			CString data;
			data.Format(_T("%d %d %d Failue"), ret, recvMsgSize, GetLastError());
			dlg->m_edit.ReplaceSel(data);
			dlg->m_edit.ReplaceSel((LPCTSTR)_T("\r\n"));
		} else { 
			dlg->m_edit.ReplaceSel((LPCTSTR)_T("Success"));
			dlg->m_edit.ReplaceSel((LPCTSTR)_T("\r\n"));
		}

		//echoBuffer[recvMsgSize] = '\0';

		//dlg->m_edit.ReplaceSel((LPCTSTR)echoBuffer);
		//dlg->m_edit.ReplaceSel((LPCTSTR)_T("\r\n"));
	}
}