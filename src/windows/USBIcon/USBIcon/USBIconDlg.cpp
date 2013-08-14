
// USBIconDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "USBIcon.h"
#include "USBIconDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CUSBIconDlg::CUSBIconDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBIconDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	Transparent = RGB(255, 255, 255);
}

void CUSBIconDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
} 

BEGIN_MESSAGE_MAP(CUSBIconDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

typedef BOOL(WINAPI * SLWA)(HWND, COLORREF, BYTE, DWORD);

void CUSBIconDlg::SetTransparency(int percent)
{
	SLWA pSetLayeredWindowAttributes = NULL;														// 함수포인터 선언, 초기화.
	HINSTANCE hmodUSER32 = LoadLibraryA("USER32.DLL");												// 인스턴스 얻음.
	pSetLayeredWindowAttributes = (SLWA)GetProcAddress(hmodUSER32,"SetLayeredWindowAttributes");	//함수포인터 얻음.
	HWND hwnd = this->m_hWnd;																		//다이얼로그의 핸들 얻음.
	SetWindowLong(hwnd, GWL_EXSTYLE,GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	pSetLayeredWindowAttributes(hwnd, 0, (255 * percent) / 100, LWA_ALPHA);
	pSetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), 0, LWA_COLORKEY);
}

// CUSBIconDlg 메시지 처리기

BOOL CUSBIconDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	
	m_background.LoadBitmapW(IDB_PASSU);
	m_bMouseDown = FALSE;

	// 투명도 50
	SetTransparency(90);

	// 시작표시줄에 표현되지 않도록 변경
	ModifyStyleEx (WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CUSBIconDlg::OnPaint()
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
HCURSOR CUSBIconDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CUSBIconDlg::OnEraseBkgnd(CDC* pDC)
{
	CDialog::OnEraseBkgnd(pDC);
	
	if(!m_background.m_hObject)
		return true;

	CRect rect;
	GetClientRect(&rect);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = memDC.SelectObject(&m_background);

	BITMAP bmap;
	m_background.GetBitmap(&bmap);
	pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, bmap.bmWidth, bmap.bmHeight, SRCCOPY);

	memDC.SelectObject(pOldBitmap);
	
	return true;
}


void CUSBIconDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bMouseDown = TRUE;
	SetCapture();
	m_mouseDownPoint = point;
	CDialog::OnLButtonDown(nFlags, point);
}


void CUSBIconDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bMouseDown = FALSE;
	ReleaseCapture();
	CDialog::OnLButtonUp(nFlags, point);
}


void CUSBIconDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( m_bMouseDown )
	{
		CSize size = m_mouseDownPoint - point;
		CRect rect;
		GetWindowRect(&rect);
		rect = size - &rect;
		MoveWindow(rect);
	}

	CDialog::OnMouseMove(nFlags, point);
}