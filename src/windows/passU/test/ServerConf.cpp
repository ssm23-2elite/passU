// ServerConf.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "ServerConf.h"
#include "afxdialogex.h"



// ServerConf 대화 상자입니다.

IMPLEMENT_DYNAMIC(ServerConf, CDialogEx)

ServerConf::ServerConf(CWnd* pParent /*=NULL*/)
	: CDialogEx(ServerConf::IDD, pParent)
	, m_serverPortEdit(_T(""))
	, serverIPAddress(_T(""))
{
	
	m_applyFlag = FALSE;
	m_bDragFlag = FALSE;
	WORD wVersionRequested;
    WSADATA wsaData;
    char name[255];
    PHOSTENT hostinfo;
    CString strIpAddress;
    wVersionRequested = MAKEWORD(2, 0);
	if(WSAStartup(wVersionRequested, &wsaData) == 0)
    {
        if(gethostname(name, sizeof(name)) == 0)
        {
            if((hostinfo = gethostbyname(name)) != NULL)
                strIpAddress = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
        } 
        WSACleanup();
    }

	serverIPAddress.Append(strIpAddress);

	AfxSocketInit();

	m_pServer = NULL;
	m_pAccept = NULL;

}

ServerConf::~ServerConf()
{
}

void ServerConf::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_CButton_one);
	DDX_Control(pDX, IDC_BUTTON2, m_CButton_two);
	DDX_Control(pDX, IDC_BUTTON5, m_CButton_three);
	DDX_Control(pDX, IDC_BUTTON9, m_CButton_four);
	DDX_Control(pDX, IDC_BUTTON7, m_CButton_five);
	DDX_Control(pDX, IDC_BUTTON6, m_CButton_six);
	DDX_Control(pDX, IDC_BUTTON8, m_CButton_seven);
	DDX_Control(pDX, IDC_BUTTON10, m_CButton_eight);
	DDX_Control(pDX, IDC_BUTTON11, m_CButton_nine);
	DDX_Text(pDX, IDC_EDIT2, m_serverPortEdit);
	DDX_Control(pDX, IDC_BUTTON12, m_CButton_portApply);
	DDX_Control(pDX, IDC_BUTTON13, m_CButton_portCancel);
	DDX_Control(pDX, IDC_EDIT2, m_portEditControl);
	DDX_Text(pDX, IDC_IPTEXT, serverIPAddress);
}


BEGIN_MESSAGE_MAP(ServerConf, CDialogEx)
	ON_BN_CLICKED(IDOK, &ServerConf::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &ServerConf::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON5, &ServerConf::OnBnClickedServerButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &ServerConf::OnBnClickedServerButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &ServerConf::OnBnClickedServerButton2)
	ON_BN_CLICKED(IDC_BUTTON9, &ServerConf::OnBnClickedServerButton4)
	ON_BN_CLICKED(IDC_BUTTON7, &ServerConf::OnBnClickedServerButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &ServerConf::OnBnClickedServerButton6)
	ON_BN_CLICKED(IDC_BUTTON8, &ServerConf::OnBnClickedServerButton7)
	ON_BN_CLICKED(IDC_BUTTON10, &ServerConf::OnBnClickedServerButton8)
	ON_BN_CLICKED(IDC_BUTTON11, &ServerConf::OnBnClickedServerButton9)
	ON_BN_CLICKED(IDC_BUTTON12, &ServerConf::OnBnClickedPortApply)
	ON_BN_CLICKED(IDC_BUTTON13, &ServerConf::OnBnClickedPortCancel)
	ON_STN_CLICKED(IDC_PICCOM, &ServerConf::OnStnClickedPiccom)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// ServerConf 메시지 처리기입니다.


void ServerConf::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_applyFlag == FALSE){
		AfxMessageBox(_T("Plz Enter Port Number !!"));
		return ;
	}

	int nPort;

	nPort = _ttoi(m_serverPortEdit);

	AfxMessageBox(_T("Start!!"));

	initServer(nPort);

}


void ServerConf::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}



void ServerConf::OnBnClickedServerButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ServerConf::OnBnClickedServerButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ServerConf::OnBnClickedServerButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ServerConf::OnBnClickedServerButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ServerConf::OnBnClickedServerButton5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ServerConf::OnBnClickedServerButton6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ServerConf::OnBnClickedServerButton7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ServerConf::OnBnClickedServerButton8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ServerConf::OnBnClickedServerButton9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



void ServerConf::OnBnClickedPortApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();
	m_applyFlag = TRUE;
	m_portEditControl.EnableWindow(FALSE);
	m_CButton_portApply.EnableWindow(FALSE);
	//AfxMessageBox(m_serverPortEdit);
}


void ServerConf::OnBnClickedPortCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_CButton_portApply.EnableWindow(TRUE);
	m_portEditControl.EnableWindow(TRUE);


}


void ServerConf::OnStnClickedPiccom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ServerConf::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	//if(/* 만약 그림 위에서 Lbuttondown 됐으면 */){
	//	SetCapture();
	//	m_bDragFlag = true;
	//	RedrawWindow();
	//}
	
	
	//AfxMessageBox(_T("hello"));


	CDialogEx::OnLButtonDown(nFlags, point);
}


void ServerConf::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(m_bDragFlag == TRUE){
		m_bDragFlag = FALSE;

		m_ptltemText = point;
		RedrawWindow();
		ReleaseCapture();

		//if(/* 포인트가 버튼 사각형 아래이면 */){
		//	// 처음에 클릭한 그림이 스마트폰인지 컴퓨터인지 걸러내서
		//	// 컴퓨터이면 버튼에 컴퓨터 삽입, 스마트폰이면 버튼에 스마트폰 삽입
		//	// 
		//}
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}


void ServerConf::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CClientDC dc(this);
	CString str;
	 
	str.Format(_T("위치 : %d %d"), point.x, point.y);
//	AfxMessageBox(str);
	if(m_bDragFlag == TRUE){

		//RedrawWindow();
		
		dc.TextOutW(point.x, point.y, str);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

void ServerConf::initServer(int nPort)
{
	m_pServer = new CServerSock();
	m_pServer->Create(nPort);

	
	AfxMessageBox(_T("Create!!"));
	CDialogEx::OnOK();

	m_pServer->Listen();

	
	AfxMessageBox(_T("Listen Complete!!"));
}


void ServerConf::cleanUp(void)
{
	if(m_pServer)	delete m_pServer;
	if(m_pAccept)	delete m_pAccept;
}




void ServerConf::accept(void)
{
	m_pAccept = new CAcceptSock();

	BOOL check = m_pServer->Accept(*m_pAccept);

	if(check == FALSE){
		AfxMessageBox(_T("접속 허용 실패"));
		return ;
	}

	AfxMessageBox(_T("Accept COmplete!!"));



}


void ServerConf::closeAcceptSock(void)
{
	m_pAccept->Close();
	delete m_pAccept;
}
