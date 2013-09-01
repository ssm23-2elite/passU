/* _______________________________________________________________________________ 
*
*		Written by Panz
*		e-mail : eyes9585@korea.com
*		date : 11 / 20 / 2002
* _______________________________________________________________________________  */

// TestKeyHookDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestKeyHook.h"
#include "TestKeyHookDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

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
// CTestKeyHookDlg dialog

CTestKeyHookDlg::CTestKeyHookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestKeyHookDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestKeyHookDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestKeyHookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestKeyHookDlg)
	DDX_Control(pDX, IDC_STATUS, m_ctrlStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestKeyHookDlg, CDialog)
	//{{AFX_MSG_MAP(CTestKeyHookDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_JOURNAL, OnJournal)
	ON_BN_CLICKED(IDC_KEYBOARD, OnKeyboard)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestKeyHookDlg message handlers

BOOL CTestKeyHookDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	m_bJournalRecording = FALSE;
	m_bKeyboardHooking = FALSE;
	m_nMode = -1;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestKeyHookDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestKeyHookDlg::OnPaint() 
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
HCURSOR CTestKeyHookDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestKeyHookDlg::OnJournal() 
{
	m_nMode = JOURNAL;
	m_ctrlStatus.SetWindowText("Save Mouse&Keyboard message and Play");
}

void CTestKeyHookDlg::OnKeyboard() 
{
	m_nMode = KEYBOARD;	
	m_ctrlStatus.SetWindowText("Keyboard Hooking");
}

/* _______________________________________________________________________________ 

		Import Method (Hook)
_______________________________________________________________________________  */

extern "C" __declspec(dllimport)
HHOOK InstallJournalRecord();		// Install Record Hook

extern "C" __declspec(dllimport)
HHOOK InstallJournalPlay();			// Install Play Hook

extern "C" __declspec(dllimport)
HHOOK InstallKeyboardHook();		// Install Keyboard Hook


extern "C" __declspec(dllimport)
void UnInstallJournalRecord();		// Uninstall Record Hook

extern "C" __declspec(dllimport)
void UnInstallJournalPlay();		// Uninstall Play Hook

extern "C" __declspec(dllimport)
void UnInstallKeyboardHook();		// Uninstall Keyboard Hook


// Message Parsing
void CTestKeyHookDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CTestKeyHookDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_F2:
			switch(m_nMode)
			{
			case JOURNAL:
				if(m_bJournalRecording)			// 이미 레코딩 중이면 스킵.
					break;

				m_bJournalRecording = TRUE;

				CButton *pBtn;
				pBtn = (CButton*)GetDlgItem(IDC_KEYBOARD);
				pBtn->ShowWindow(SW_HIDE);		// 다른 옵션을 숨긴다. 왜? 레코딩중에 다른것을 선택 못하게 하려고..

				InstallJournalRecord();			// Install Record Hook at System
				return 1;
				
			case KEYBOARD:

				// 이미 키보드 후킹중이면 무시.
				if(m_bKeyboardHooking)
					break;

				// 확인 메세지박스 출력.
				if(IDNO == MessageBox("Do You want to install keyboard hook procedure?", "Warning", MB_YESNO))
					break;

				m_bKeyboardHooking = TRUE;

				pBtn = (CButton*)GetDlgItem(IDC_JOURNAL);
				pBtn->ShowWindow(SW_HIDE);		// 다른 옵션 버튼 숨기기.

				InstallKeyboardHook();			// Install Keyboard Hook at System
				return 1;
			}
			break;
			
		case VK_F3:
			if(m_bJournalRecording)
			{
				UnInstallJournalRecord();
				m_bJournalRecording = FALSE;
				return 1;
			}				
			break;
				
		case VK_F4:
			switch(m_nMode)
			{
			case JOURNAL:
				if(m_bJournalRecording)
				break;

				InstallJournalPlay();

				CButton *pBtn;
				pBtn = (CButton*)GetDlgItem(IDC_KEYBOARD);
				pBtn->ShowWindow(SW_SHOW);		// 다른 옵션 버튼 표시.

				return 1;
					
			case KEYBOARD:
				UnInstallKeyboardHook();

				m_bKeyboardHooking = FALSE;

				pBtn = (CButton*)GetDlgItem(IDC_JOURNAL);
				pBtn->ShowWindow(SW_SHOW);		// 다른 옵션 버튼 표시.

				return 1;
			}
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
