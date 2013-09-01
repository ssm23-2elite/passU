
// testDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"
#include "afxdialogex.h"
#include "ServerConf.h"
#include "ClientConf.h"
#include <Dbt.h>

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


// CtestDlg 대화 상자



CtestDlg::CtestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtestDlg::IDD, pParent)
	, CRadioButton_Server(false)
	, CRadioButton_Client(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON11, CButton_clientconfiguration);
	DDX_Control(pDX, IDC_BUTTON10, CButton_ServerConfig);
}

BEGIN_MESSAGE_MAP(CtestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CtestDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_BUTTON10, &CtestDlg::OnBnClickedServerConfiguration)
	ON_BN_CLICKED(IDC_BUTTON11, &CtestDlg::OnBnClickedClientConfiguration)
	ON_BN_CLICKED(IDC_RADIO2, &CtestDlg::OnBnClickedServerRadio)
	ON_BN_CLICKED(IDC_RADIO1, &CtestDlg::OnBnClickedClientRadio)
	ON_BN_CLICKED(IDC_BUTTON2, &CtestDlg::OnBnClickedExitButton)
END_MESSAGE_MAP()


// CtestDlg 메시지 처리기

BOOL CtestDlg::OnInitDialog()
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

	// Button 처음엔 둘 다 비활성화
	CButton_ServerConfig.EnableWindow(FALSE);
	CButton_clientconfiguration.EnableWindow(FALSE);

	usbIcon = NULL;
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CtestDlg::OnPaint()
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
HCURSOR CtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CtestDlg::OnBnClickedStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CtestDlg::OnBnClickedServerConfiguration()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	server.DoModal();
}


void CtestDlg::OnBnClickedClientConfiguration()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	client.DoModal();
}


void CtestDlg::OnBnClickedServerRadio()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.t
		CButton_ServerConfig.EnableWindow(TRUE);
		CButton_clientconfiguration.EnableWindow(FALSE);
		
}


void CtestDlg::OnBnClickedClientRadio()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton_ServerConfig.EnableWindow(FALSE);
	CButton_clientconfiguration.EnableWindow(TRUE);
}


void CtestDlg::OnBnClickedExitButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}


LRESULT CtestDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(message == WM_DEVICECHANGE){
		UINT event = (UINT)wParam;
		switch(event){

		case DBT_DEVICEARRIVAL :
			// USB에 새로운 장치가 연결되어 사용가능한 상태가 된 경우...
			if(usbIcon == NULL)
			{ 
				usbIcon = new CUSBIconDlg;
				usbIcon->Create(IDD_USBICON_DIALOG);
				usbIcon->ShowWindow(SW_SHOW);
			}
			break;
		case DBT_DEVICEREMOVECOMPLETE :
			// USB에서 어떤 장치가 제거된 경우...
			if(usbIcon != NULL)
			{
				usbIcon->DestroyWindow();
				usbIcon = NULL;
			}
			break;
		}
	}
	return CDialogEx::DefWindowProc(message, wParam, lParam);
}
