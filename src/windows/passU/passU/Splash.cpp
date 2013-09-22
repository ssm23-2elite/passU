// Splash.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "Splash.h"
#include "afxdialogex.h"


// CSplash 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSplash, CDialog)

CSplash::CSplash(CWnd* pParent /*=NULL*/)
	: CDialog(CSplash::IDD, pParent)
{

}

CSplash::~CSplash()
{
}

void CSplash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSplash, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSplash 메시지 처리기입니다.


BOOL CSplash::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetTimer(0, 2000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSplash::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	EndDialog(0);

	CDialog::OnTimer(nIDEvent);
}
