// PassUTransparentDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "PassUTransparentDlg.h"
#include "afxdialogex.h"


// PassUTransparentDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPassUTransparentDlg, CDialog)

CPassUTransparentDlg::CPassUTransparentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPassUTransparentDlg::IDD, pParent)
{

}

CPassUTransparentDlg::~CPassUTransparentDlg()
{
}

void CPassUTransparentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPassUTransparentDlg, CDialog)
END_MESSAGE_MAP()


// CPassUTransparentDlg 메시지 처리기입니다.


BOOL CPassUTransparentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	LONG ExtendedStyle = GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE);
	ExtendedStyle &= ~WS_CAPTION;
	ExtendedStyle &= ~WS_SYSMENU;
	SetWindowLong( GetSafeHwnd(), GWL_EXSTYLE, ExtendedStyle | WS_EX_LAYERED );

	double TransparencyPercentage = 1;

	double fAlpha = TransparencyPercentage * ( 255.0 / 100 );
	BYTE byAlpha = static_cast<BYTE>( fAlpha );
	SetLayeredWindowAttributes( 0, byAlpha, LWA_ALPHA );

	int screenx = GetSystemMetrics( SM_CXSCREEN );
	int screeny = GetSystemMetrics( SM_CYSCREEN );

	SetWindowPos( NULL, -4, -4, screenx + 8, screeny + 4, SWP_NOZORDER );
	::SetWindowPos(this->GetSafeHwnd(), HWND_TOPMOST, 0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	::SetForegroundWindow(this->GetSafeHwnd());
	::SetActiveWindow(this->GetSafeHwnd());
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
