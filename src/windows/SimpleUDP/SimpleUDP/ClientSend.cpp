// ClientSend.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleUDP.h"
#include "ClientSend.h"
#include "afxsock.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSend dialog


CClientSend::CClientSend(CWnd* pParent /*=NULL*/)
	: CDialog(CClientSend::IDD, pParent)
{
	isConnected = FALSE;
	prev_x = 0;
	prev_y = 0;
	mpacket.sendDev = 0x5;
	mpacket.recvDev = 0x6;
	mpacket.wheel = 0;
	mpacket.leftmouse = 0;
	mpacket.rightmouse = 0;
	mpacket.relativeField = 0;
	mpacket.inputDev = 1;

	kpacket.sendDev = 0x5;
	kpacket.recvDev = 0x6;
	kpacket.relativeField = 0;
	kpacket.inputDev = 0;
	//{{AFX_DATA_INIT(CClientSend)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CClientSend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CONNECT, m_connect);
}


BEGIN_MESSAGE_MAP(CClientSend, CDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CONNECT, &CClientSend::OnBnClickedConnect)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	//	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientSend message handlers

void CClientSend::OnMouseEvent(CPoint point)
{
	if(prev_x == 0 && prev_y == 0) {
		mpacket.relativeField = 0;
		mpacket.xCoord = GetSystemMetrics(SM_CXSCREEN);
		mpacket.yCoord = GetSystemMetrics(SM_CYSCREEN);
	} else {
		mpacket.relativeField = 1;
		mpacket.xCoord = prev_x - point.x;
		mpacket.yCoord = prev_y - point.y;
	}
	prev_x = point.x;
	prev_y = point.y;

	AfxSocketInit( NULL);

	CSocket passUClient;  

	if (passUClient.Create(0,SOCK_DGRAM,NULL) == 0) {
		AfxMessageBox("Create() failed");
	}

	if (passUClient.SendTo((void *)&mpacket, sizeof(MPACKET),514,
		(LPCSTR)"localhost", 0) != sizeof(MPACKET)) {
			AfxMessageBox("SendTo() sent a different number of bytes than expected");
	}
	passUClient.Close();
}


void CClientSend::OnBnClickedConnect()
{
	if(m_connect.GetCheck() == TRUE) {
		isConnected = TRUE;
	} else {
		isConnected = FALSE;
	}
}


void CClientSend::OnMouseMove(UINT nFlags, CPoint point)
{
	if(isConnected) {
		OnMouseEvent(point);
	}

	CDialog::OnMouseMove(nFlags, point);
}


void CClientSend::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(isConnected) {
		mpacket.leftmouse = 1;
		OnMouseEvent(point);
	}

	CDialog::OnLButtonDown(nFlags, point);
}
void CClientSend::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(isConnected) {
		mpacket.leftmouse = 0;
		OnMouseEvent(point);
	}
	CDialog::OnLButtonUp(nFlags, point);
}

void CClientSend::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(isConnected) {
		mpacket.rightmouse = 1;
		OnMouseEvent(point);
	}

	CDialog::OnRButtonDown(nFlags, point);
}


void CClientSend::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(isConnected) {
		mpacket.rightmouse = 0;
		OnMouseEvent(point);
	}
	CDialog::OnRButtonUp(nFlags, point);
}



BOOL CClientSend::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CClientSend::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(isConnected == TRUE && pMsg->message == WM_KEYDOWN) 
	{
		AfxSocketInit( NULL);

		kpacket.keyCode = pMsg->wParam;

		CSocket passUClient;  

		if (passUClient.Create(0,SOCK_DGRAM,NULL) == 0) {
			AfxMessageBox("Create() failed");
		}

		if (passUClient.SendTo((void *)&kpacket, sizeof(KPACKET),514,
			(LPCSTR)"localhost", 0) != sizeof(MPACKET)) {
				AfxMessageBox("SendTo() sent a different number of bytes than expected");
		}
		passUClient.Close();

	}

	return CDialog::PreTranslateMessage(pMsg);
}
