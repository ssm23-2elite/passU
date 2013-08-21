
// TestServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TestServer.h"
#include "TestServerDlg.h"
#include "afxdialogex.h"
#include "Mmsystem.h"

#define _WIN32_WINNT 0x0400

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestServerDlg 대화 상자



CTestServerDlg::CTestServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestServerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SERVERON, &CTestServerDlg::OnBnClickedServeron)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()


// CTestServerDlg 메시지 처리기

BOOL CTestServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	prevTime = 0;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestServerDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestServerDlg::OnBnClickedServeron()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	((CTestServerApp *)AfxGetApp())->initServer(3737);
}

void CTestServerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DWORD curTime = timeGetTime();

	CString str;
		str.Format(_T("x: %d y: %d"), point.x, point.y);
		SetDlgItemText(IDC_LOC, str);

		prevTime = curTime;

		((CTestServerApp *)AfxGetApp())->sendMouseMove(point.x, point.y);

	CDialog::OnMouseMove(nFlags, point);
}


void CTestServerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DWORD curTime = timeGetTime();

	CString str;
		str.Format(_T("x: %d y: %d"), point.x, point.y);
		SetDlgItemText(IDC_LOC, str);

		prevTime = curTime;

		((CTestServerApp *)AfxGetApp())->sendMouseDown(point.x, point.y);

	CDialog::OnLButtonDown(nFlags, point);
}


void CTestServerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DWORD curTime = timeGetTime();

	CString str;
		str.Format(_T("x: %d y: %d"), point.x, point.y);
		SetDlgItemText(IDC_LOC, str);

		prevTime = curTime;

		((CTestServerApp *)AfxGetApp())->sendMouseUp(point.x, point.y);

	CDialog::OnLButtonUp(nFlags, point);
}


void CTestServerDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DWORD curTime = timeGetTime();

	CString str;
		str.Format(_T("keyCode: %d Down"), nChar);
		SetDlgItemText(IDC_KEY, str);

		prevTime = curTime;

		((CTestServerApp *)AfxGetApp())->sendKeyDown(nChar);

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CTestServerDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DWORD curTime = timeGetTime();

	CString str;
	str.Format(_T("keyCode: %d Up"), nChar);
	SetDlgItemText(IDC_KEY, str);

	prevTime = curTime;

	((CTestServerApp *)AfxGetApp())->sendKeyUp(nChar);

	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CTestServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->message == WM_KEYDOWN )  
	{
		CString str;
		str.Format(_T("keyCode: %d Down"), pMsg->wParam);
		SetDlgItemText(IDC_KEY, str);

		((CTestServerApp *)AfxGetApp())->sendKeyDown(pMsg->wParam);
	} else if (pMsg->message == WM_KEYUP )
	{
		CString str;
		str.Format(_T("keyCode: %d Up"), pMsg->wParam);
		SetDlgItemText(IDC_KEY, str);

		((CTestServerApp *)AfxGetApp())->sendKeyUp(pMsg->wParam);
	}
	return CDialog::PreTranslateMessage(pMsg);
}
