// SimpleUDPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleUDP.h"
#include "SimpleUDPDlg.h"
#include "afxsock.h"
#include "ClientSend.h"
#include "packet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define ECHOMAX 1024

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
HANDLE thr;
unsigned long id1;
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleUDPDlg dialog

CSimpleUDPDlg::CSimpleUDPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimpleUDPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimpleUDPDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleUDPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimpleUDPDlg)
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSimpleUDPDlg, CDialog)
	//{{AFX_MSG_MAP(CSimpleUDPDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleUDPDlg message handlers
UINT ReceiveData(LPVOID pParam)
{
	CSimpleUDPDlg *dlg=(CSimpleUDPDlg*)pParam;
	AfxSocketInit(NULL);
	CSocket echoServer;  

	// Create socket for sending/receiving datagrams
	if (echoServer.Create(514, SOCK_DGRAM, NULL)== 0) {
		AfxMessageBox("Create() failed");
	}

	for(;;) { // Run forever

		// Client address
		SOCKADDR_IN echoClntAddr; 

		// Set the size of the in-out parameter
		int clntAddrLen = sizeof(echoClntAddr);

		// Buffer for echo string
		PACKET packet;

		// Block until receive message from a client
		int recvMsgSize = echoServer.ReceiveFrom(&packet, 
			sizeof(PACKET), (SOCKADDR*)&echoClntAddr, &clntAddrLen, 0);
		if (recvMsgSize < 0) {
			AfxMessageBox("RecvFrom() failed");
		}

		CString data;
		
		if(packet.inputDev == 0)		// 키보드
		{
			KPACKET *kpacket = (KPACKET*)&packet;

			data.Format(_T("KEYBO [%s][%d]\r\n"), 
				kpacket->relativeField == 0?_T("INIT"):_T("NONE"),
				kpacket->keyCode);
		} else if(packet.inputDev == 1) // 마우스
		{
			MPACKET *mpacket = (MPACKET*)&packet;
			data.Format(_T("MOUSE [%s][%d][%d][%d] x: %d, y: %d\r\n"), 
				mpacket->relativeField == 0?_T("INIT"):_T("NONE"),
				mpacket->leftmouse, mpacket->rightmouse,  mpacket->wheel,
				mpacket->xCoord, mpacket->yCoord);
		}
		dlg->m_edit.ReplaceSel(data);
	}
}




BOOL CSimpleUDPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	thr = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ReceiveData,this,NULL,&id1);
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSimpleUDPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSimpleUDPDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSimpleUDPDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



void CSimpleUDPDlg::OnStart() 
{
	CClientSend *client = new CClientSend();

	client->Create( IDD_CLIENT );
	client->ShowWindow( SW_SHOW );
}
