// ServerConf.cpp : 구현 파일입니다.
//

#include <afxwin.h>
#include "stdafx.h"
#include "test.h"
#include "ServerConf.h"
#include "afxdialogex.h"
#include "MyThread.h"
#include "packet.h"

// ServerConf 대화 상자입니다.

typedef HHOOK (*InstallKeyboardHook)();
typedef HHOOK (*InstallMouseHook)();

typedef void (*UnInstallKeyboardHook)();
typedef void  (*UnInstallMouseHook)();

InstallKeyboardHook installKeyhook;
InstallMouseHook installMousehook;

UnInstallKeyboardHook uninstallKeyhook;
UnInstallMouseHook uninstallMousehook;
IMPLEMENT_DYNAMIC(ServerConf, CDialogEx)

	ServerConf::ServerConf(CWnd* pParent /*=NULL*/)
	: CDialogEx(ServerConf::IDD, pParent)
	, m_serverPortEdit(_T(""))
	, serverIPAddress(_T(""))
{
	TRACE("초기화 코드 - 생성자\n");
	/* 현재 서버가 될 컴퓨터의 IP를 알아내는 코드 */
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
	serverIPAddress.Append(strIpAddress); // static control에 서버 IP 주소를 붙여넣음
	initFlag(); // 각종 Flag 초기화

	hinstDLL = NULL;
	hHook = NULL;
	cThread = NULL;
	tmp = NULL;

	hinstDLL = LoadLibrary(_T("KeyHook.dll"));
	if(!hinstDLL)
		AfxMessageBox(_T("KeyHook.dll 로드 실패!"));

	installKeyhook = (InstallKeyboardHook)GetProcAddress(hinstDLL, "InstallKeyboardHook");
	installMousehook = (InstallMouseHook)GetProcAddress(hinstDLL, "InstallMouseHook");

	uninstallKeyhook = (UnInstallKeyboardHook)GetProcAddress(hinstDLL, "UnInstallKeyboardHook");
	uninstallMousehook = (UnInstallMouseHook)GetProcAddress(hinstDLL, "UnInstallMouseHook");


	AfxSocketInit(); // 소켓 초기화
}

ServerConf::~ServerConf()
{

}

void ServerConf::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_serverPortEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_CButton_one);
	DDX_Control(pDX, IDC_BUTTON2, m_CButton_two);
	DDX_Control(pDX, IDC_BUTTON3, m_CButton_three);
	DDX_Control(pDX, IDC_BUTTON4, m_CButton_four);
	DDX_Control(pDX, IDC_BUTTON5, m_CButton_five);
	DDX_Control(pDX, IDC_BUTTON6, m_CButton_six);
	DDX_Control(pDX, IDC_BUTTON7, m_CButton_seven);
	DDX_Control(pDX, IDC_BUTTON8, m_CButton_eight);
	DDX_Control(pDX, IDC_BUTTON9, m_CButton_nine);

	DDX_Control(pDX, IDC_BUTTON12, m_CButton_portApply);
	DDX_Control(pDX, IDC_BUTTON13, m_CButton_portCancel);
	DDX_Control(pDX, IDC_EDIT2, m_portEditControl);
	DDX_Text(pDX, IDC_IPTEXT, serverIPAddress);
	DDX_Control(pDX, IDOK, m_CBtn_Start);
	DDX_Control(pDX, IDCANCEL, m_CBtn_stop);
}


BEGIN_MESSAGE_MAP(ServerConf, CDialogEx)
	ON_BN_CLICKED(IDOK, &ServerConf::OnBnClickedStart)
	ON_BN_CLICKED(IDCANCEL, &ServerConf::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON12, &ServerConf::OnBnClickedPortApply)
	ON_BN_CLICKED(IDC_BUTTON13, &ServerConf::OnBnClickedPortCancel)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON1, &ServerConf::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &ServerConf::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &ServerConf::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &ServerConf::OnBnClickedButton4)
	//ON_BN_CLICKED(IDC_BUTTON5, &ServerConf::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &ServerConf::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &ServerConf::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &ServerConf::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &ServerConf::OnBnClickedButton9)
	//	ON_WM_KEYDOWN()
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_BUTTON14, &ServerConf::OnBnClickedButton14)
END_MESSAGE_MAP()


// ServerConf 메시지 처리기입니다.


void ServerConf::OnBnClickedStart()
{
	TRACE("---------------------------Start Button Click---------------------------\n");
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_applyFlag == FALSE){
		AfxMessageBox(_T("Plz Enter Port Number !!"));
		return ;
	}

	int nPort;

	nPort = _ttoi(m_serverPortEdit);

	AfxMessageBox(_T("Start!!"));

	m_startFlag = TRUE;

	initServer(nPort);

	m_CBtn_Start.EnableWindow(FALSE);


}
void ServerConf::initServer(int nPort)
 {
	TRACE("---------------------------initServer---------------------------\n");
	listen.Create(nPort);
	listen.Listen();

	m_startFlag = TRUE; // 서버 시작 플래그 True
	AfxMessageBox(_T("Accept Complete"));
}

void ServerConf::OnBnClickedCancel()
{
	TRACE("---------------------------OnBnClickedCancel---------------------------\n");
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	initFlag();

	uninstallKeyhook();
	uninstallMousehook();

	//tmp = &listen.m_sockList; // 쓰레드 리스트 가져옴
	//int count = tmp->GetCount();
	//POSITION pos = tmp->GetHeadPosition(); // 맨 처음 헤드를 포인팅함

	//clientP.msgType = 3;
	//clientP.hello = 0;
	//clientP.bye = 1; // bye 패킷

	//while(pos != NULL){
	//	cThread = (CMyThread *)tmp->GetAt(pos);

	//	cThread->m_mySocket->Send((LPCSTR)&clientP, sizeof(CPACKET));


	//	tmp->GetNext(pos);
	//}

	//tmp->RemoveAll();

	//tmp = NULL;
	CDialogEx::OnCancel();
}


void ServerConf::OnBnClickedPortApply()
{
	TRACE("---------------------------OnBnClickedPortApply---------------------------\n");
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData();
	CString str;

	GetDlgItemText(IDC_EDIT2, str);

	if(str.GetLength() == 0){
		AfxMessageBox(_T("포트 번호를 입력하세요."));
		return ;
	}

	m_applyFlag = TRUE;
	m_portEditControl.EnableWindow(FALSE);
	m_CButton_portApply.EnableWindow(FALSE);
	//AfxMessageBox(m_serverPortEdit);
}


void ServerConf::OnBnClickedPortCancel()
{
	TRACE("---------------------------OnBnClickedPortCancel---------------------------\n");
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_CButton_portApply.EnableWindow(TRUE);
	m_portEditControl.EnableWindow(TRUE);
}


void ServerConf::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	TRACE("---------------------------OnLButtonDown---------------------------\n");
	picControl[0] = this->GetDlgItem(IDC_PICCOM);
	picControl[1] = this->GetDlgItem(IDC_PICPHN);

	picControl[0]->GetWindowPlacement(&getCoord[9]);
	picControl[1]->GetWindowPlacement(&getCoord[10]);

	if(point.x >= getCoord[9].rcNormalPosition.left && 
		point.x <= getCoord[9].rcNormalPosition.right && 
		point.y >= getCoord[9].rcNormalPosition.top && 
		point.y <= getCoord[9].rcNormalPosition.bottom){ // 모니터
			m_deviceFlag = 0;
			SetCapture();
			m_bDragFlag = true;
			//	AfxMessageBox(_T("Monitor"));
			RedrawWindow(); 
	} else if (point.x >= getCoord[10].rcNormalPosition.left && 
		point.x <= getCoord[10].rcNormalPosition.right && 
		point.y >= getCoord[10].rcNormalPosition.top && 
		point.y <= getCoord[10].rcNormalPosition.bottom){ // 스마트폰
			m_deviceFlag = 1;
			SetCapture();
			m_bDragFlag = true;
			//	AfxMessageBox(_T("Smart Phone"));
			RedrawWindow(); 
	} else{
		m_deviceFlag = -1;
		m_bDragFlag = false;
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void ServerConf::OnLButtonUp(UINT nFlags, CPoint point)
{
	TRACE("---------------------------OnLButtonUp---------------------------\n");
	CPoint m_tmpPoint;
	CBitmap m_tmpBitmap;

	btnControl[0] = this->GetDlgItem(IDC_BUTTON1);
	btnControl[1] = this->GetDlgItem(IDC_BUTTON2);
	btnControl[2] = this->GetDlgItem(IDC_BUTTON3);
	btnControl[3] = this->GetDlgItem(IDC_BUTTON4);
	btnControl[4] = this->GetDlgItem(IDC_BUTTON5);
	btnControl[5] = this->GetDlgItem(IDC_BUTTON6);
	btnControl[6] = this->GetDlgItem(IDC_BUTTON7);
	btnControl[7] = this->GetDlgItem(IDC_BUTTON8);
	btnControl[8] = this->GetDlgItem(IDC_BUTTON9);

	for(int i = 0 ; i < 9 ; i ++)
		btnControl[i]->GetWindowPlacement(&getCoord[i]);


	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(m_bDragFlag == TRUE){
		m_bDragFlag = FALSE;

		m_ptltemText = point;
		RedrawWindow();
		ReleaseCapture();

		if(point.x >= getCoord[0].rcNormalPosition.left &&
			point.x <= getCoord[0].rcNormalPosition.right &&
			point.y <= getCoord[0].rcNormalPosition.bottom &&
			point.y >= getCoord[0].rcNormalPosition.top){ // 드래그 놓은 곳이 버튼 1일 때

				// 처음에 클릭한 그림이 스마트폰인지 컴퓨터인지 걸러내서
				// 컴퓨터이면 버튼에 컴퓨터 삽입, 스마트폰이면 버튼에 스마트폰 삽입


				if(m_deviceFlag == 0){ // 컴퓨터
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP1);
					m_settingFlag[0] = 0;
				} else if(m_deviceFlag == 1){ // 스마트폰
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP2);
					m_settingFlag[0] =  1;
				} else{ // 둘 다 아니면 그냥 리턴6
					m_settingFlag[0] = -1;
					return;
				}

				m_CButton_one.SetBitmap(m_tmpBitmap); // 버튼에 이미지 로딩

				UpdateData();Invalidate();RedrawWindow();
				// 
		} else if(point.x >= getCoord[1].rcNormalPosition.left &&
			point.x <= getCoord[1].rcNormalPosition.right &&
			point.y <= getCoord[1].rcNormalPosition.bottom &&
			point.y >= getCoord[1].rcNormalPosition.top){ // 드래그 놓은 곳이 버튼 2일 때
				if(m_deviceFlag == 0){ // 컴퓨터
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP1);


					m_settingFlag[1] = 0;
				} else if(m_deviceFlag == 1){ // 스마트폰
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP2);
					m_settingFlag[1] = 1;
				} else{ // 둘 다 아니면 그냥 리턴

					m_settingFlag[1] = -1;
					return;
				}

				m_CButton_two.SetBitmap(m_tmpBitmap); // 버튼에 이미지 로딩				

				UpdateData();Invalidate();RedrawWindow();
		} else if(point.x >= getCoord[2].rcNormalPosition.left &&
			point.x <= getCoord[2].rcNormalPosition.right &&
			point.y <= getCoord[2].rcNormalPosition.bottom &&
			point.y >= getCoord[2].rcNormalPosition.top){ // 드래그 놓은 곳이 버튼 3일 때
				if(m_deviceFlag == 0){ // 컴퓨터
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP1);

					m_settingFlag[2] = 0;
				} else if(m_deviceFlag == 1){ // 스마트폰
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP2);

					m_settingFlag[2] = 1;
				} else{ // 둘 다 아니면 그냥 리턴

					m_settingFlag[2] = -1;
					return;
				}
				m_CButton_three.SetBitmap(m_tmpBitmap); // 버튼에 이미지 로딩


				UpdateData();Invalidate();RedrawWindow();
		} else if(point.x >= getCoord[3].rcNormalPosition.left &&
			point.x <= getCoord[3].rcNormalPosition.right &&
			point.y <= getCoord[3].rcNormalPosition.bottom &&
			point.y >= getCoord[3].rcNormalPosition.top){ // 드래그 놓은 곳이 버튼 4일 때
				if(m_deviceFlag == 0){ // 컴퓨터
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP1);

					m_settingFlag[3] = 0;
				} else if(m_deviceFlag == 1){ // 스마트폰

					m_settingFlag[3] = 1;
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP2);
				} else{ // 둘 다 아니면 그냥 리턴

					m_settingFlag[3] = -1;
					return;
				}

				m_CButton_four.SetBitmap(m_tmpBitmap); // 버튼에 이미지 로딩


				UpdateData();Invalidate();RedrawWindow();
		} /*else if(point.x >= getCoord[4].rcNormalPosition.left &&
		  //	point.x <= getCoord[4].rcNormalPosition.right &&
		  //	point.y <= getCoord[4].rcNormalPosition.bottom &&
		  //	point.y >= getCoord[4].rcNormalPosition.top){ // 드래그 놓은 곳이 버튼 5일 때
		  //		if(m_deviceFlag == 0){ // 컴퓨터
		  //			m_tmpBitmap.LoadBitmapW(IDB_BITMAP1);

		  //			m_settingFlag[4] = 0;
		  //		} else if(m_deviceFlag == 1){ // 스마트폰

		  //			m_settingFlag[4] = 1;
		  //			m_tmpBitmap.LoadBitmapW(IDB_BITMAP2);
		  //		} else{ // 둘 다 아니면 그냥 리턴

		  //			m_settingFlag[4] = -1;
		  //			return;
		  //		}
		  //		//	m_CButton_five.SetWindowText(NULL);

		  //		m_CButton_five.SetBitmap(m_tmpBitmap); // 버튼에 이미지 로딩


		  //		UpdateData();Invalidate();RedrawWindow();
		  }*/ else if(point.x >= getCoord[5].rcNormalPosition.left &&
		  point.x <= getCoord[5].rcNormalPosition.right &&
		  point.y <= getCoord[5].rcNormalPosition.bottom &&
		  point.y >= getCoord[5].rcNormalPosition.top){ // 드래그 놓은 곳이 버튼 6일 때
			  if(m_deviceFlag == 0){ // 컴퓨터
				  m_tmpBitmap.LoadBitmapW(IDB_BITMAP1);

				  m_settingFlag[5] = 0;
			  } else if(m_deviceFlag == 1){ // 스마트폰
				  m_tmpBitmap.LoadBitmapW(IDB_BITMAP2);

				  m_settingFlag[5] = 1;
			  } else{ // 둘 다 아니면 그냥 리턴

				  m_settingFlag[5] = -1;
				  return;
			  }
			  m_CButton_six.SetBitmap(m_tmpBitmap); // 버튼에 이미지 로딩

			  UpdateData();Invalidate();RedrawWindow();
		} else if(point.x >= getCoord[6].rcNormalPosition.left &&
			point.x <= getCoord[6].rcNormalPosition.right &&
			point.y <= getCoord[6].rcNormalPosition.bottom &&
			point.y >= getCoord[6].rcNormalPosition.top){ // 드래그 놓은 곳이 버튼 7일 때
				if(m_deviceFlag == 0){ // 컴퓨터
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP1);

					m_settingFlag[6] = 0;
				} else if(m_deviceFlag == 1){ // 스마트폰
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP2);

					m_settingFlag[6] = 1;

				} else{ // 둘 다 아니면 그냥 리턴

					m_settingFlag[6] = -1;
					return;
				}
				m_CButton_seven.SetBitmap(m_tmpBitmap); // 버튼에 이미지 로딩

				UpdateData();Invalidate();RedrawWindow();
		} else if(point.x >= getCoord[7].rcNormalPosition.left &&
			point.x <= getCoord[7].rcNormalPosition.right &&
			point.y <= getCoord[7].rcNormalPosition.bottom &&
			point.y >= getCoord[7].rcNormalPosition.top){ // 드래그 놓은 곳이 버튼 8일 때
				if(m_deviceFlag == 0){ // 컴퓨터
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP1);

					m_settingFlag[7] = 0;
				} else if(m_deviceFlag == 1){ // 스마트폰
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP2);

					m_settingFlag[7] = 1;

				} else{ // 둘 다 아니면 그냥 리턴

					m_settingFlag[7] = -1;
					return;

				}
				m_CButton_eight.SetBitmap(m_tmpBitmap); // 버튼에 이미지 로딩


				UpdateData();
				Invalidate();RedrawWindow();
		} else if(point.x >= getCoord[8].rcNormalPosition.left &&
			point.x <= getCoord[8].rcNormalPosition.right &&
			point.y <= getCoord[8].rcNormalPosition.bottom &&
			point.y >= getCoord[8].rcNormalPosition.top){ // 드래그 놓은 곳이 버튼 9일 때
				if(m_deviceFlag == 0){ // 컴퓨터
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP1);

					m_settingFlag[8] = 0;
				} else if(m_deviceFlag == 1){ // 스마트폰
					m_tmpBitmap.LoadBitmapW(IDB_BITMAP2);

					m_settingFlag[8] = 1;
				} else{ // 둘 다 아니면 그냥 리턴

					m_settingFlag[8] = -1;
					return;
				}

				m_CButton_nine.SetBitmap(m_tmpBitmap); // 버튼에 이미지 로딩


				UpdateData();
				Invalidate();RedrawWindow();
		} else { // 사각형(버튼) 아래가 아니면 그냥 리턴

			return ; 
		}

	}
	CDialogEx::OnLButtonUp(nFlags, point);
}


void ServerConf::OnMouseMove(UINT nFlags, CPoint point)
{
	//	TRACE("---------------------------OnMouseMove---------------------------\n");
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CClientDC dc(this);
	CString str;

	//	str.Format(_T("위치 : %d %d"), point.x, point.y);
	//	AfxMessageBox(str);
	if(m_bDragFlag == true){

		//RedrawWindow();

		//dc.TextOutW(point.x, point.y, str);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}



void ServerConf::OnBnClickedButton1()
{
	TRACE("---------------------------OnBnClickedButton1---------------------------");
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_settingFlag[0] != -1){
		if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			m_CButton_one.SetBitmap(NULL);
			m_CButton_one.SetWindowText(_T("1"));
			m_deviceFlag = -1;
			m_settingFlag[0] = -1;
			RedrawWindow();
		}
	}
}


void ServerConf::OnBnClickedButton2()
{TRACE("---------------------------OnBnClickedButton2---------------------------");
// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
if(m_settingFlag[1] != -1){
	if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
	{
		m_CButton_two.SetBitmap(NULL);
		m_CButton_two.SetWindowText(_T("2"));
		m_deviceFlag = -1;
		m_settingFlag[1] = -1;
		RedrawWindow();
	}
}
}


void ServerConf::OnBnClickedButton3()
{TRACE("---------------------------OnBnClickedButton3---------------------------");
// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
if(m_settingFlag[2] != -1){
	if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
	{
		m_CButton_three.SetBitmap(NULL);
		m_CButton_three.SetWindowText(_T("3"));
		m_deviceFlag = -1;
		m_settingFlag[2] = -1;
		RedrawWindow();
	}
}
}


void ServerConf::OnBnClickedButton4()
{TRACE("---------------------------OnBnClickedButton4---------------------------");
// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
if(m_settingFlag[3] != -1){
	if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
	{
		m_CButton_four.SetBitmap(NULL);
		m_CButton_four.SetWindowText(_T("4"));
		m_deviceFlag = -1;
		m_settingFlag[3] = -1;
		RedrawWindow();
	}
}
}


//void ServerConf::OnBnClickedButton5()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	if(m_settingFlag[4] != -1){
//		if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
//		{
//			m_CButton_five.SetBitmap(NULL);
//			m_CButton_five.SetWindowText(_T("5"));
//			m_deviceFlag = -1;
//			m_settingFlag[4] = -1;
//			RedrawWindow();
//		}
//	}
//}


void ServerConf::OnBnClickedButton6()
{TRACE("---------------------------OnBnClickedButton6---------------------------");
// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
if(m_settingFlag[5] != -1){
	if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
	{
		m_CButton_six.SetBitmap(NULL);
		m_CButton_six.SetWindowText(_T("6"));
		m_deviceFlag = -1;
		m_settingFlag[5] = -1;
		RedrawWindow();
	}
}
}


void ServerConf::OnBnClickedButton7()
{TRACE("---------------------------OnBnClickedButton7---------------------------");
// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
if(m_settingFlag[6] != -1){
	if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
	{
		m_CButton_seven.SetBitmap(NULL);
		m_CButton_seven.SetWindowText(_T("7"));
		m_deviceFlag = -1;
		m_settingFlag[6] = -1;
		RedrawWindow();
	}
}
}


void ServerConf::OnBnClickedButton8()
{TRACE("---------------------------OnBnClickedButton8---------------------------");
// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
if(m_settingFlag[7] != -1){
	if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
	{
		m_CButton_eight.SetBitmap(NULL);
		m_CButton_eight.SetWindowText(_T("8"));
		m_deviceFlag = -1;
		m_settingFlag[7] = -1;
		RedrawWindow();
	}
}
}


void ServerConf::OnBnClickedButton9()
{TRACE("---------------------------OnBnClickedButton9---------------------------");
// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
if(m_settingFlag[8] != -1){
	if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
	{
		m_CButton_nine.SetBitmap(NULL);
		m_CButton_nine.SetWindowText(_T("9"));
		m_deviceFlag = -1;
		m_settingFlag[8] = -1;
		RedrawWindow();
	}
}
}

void ServerConf::closeClient(CMySocket *s)
{
	TRACE("---------------------------closeClient---------------------------");
	if(m_startFlag == TRUE){
		m_startFlag = FALSE;

		AfxMessageBox(_T("Close Success!!"));

		//m_CBtn_Start.EnableWindow(TRUE);
	}
}

void ServerConf::receiveData(CMySocket *s) // 여기서 갑자기 m_settingFlag, nSocket, client_id 의 값이 바뀐다. 왜? 딱히 바뀔만한 상황이 없는데...
{

	TRACE("%d\n", nSocket);
	TRACE("---------------------------receiveData---------------------------\n");
	KPACKET tmp;
	//initFlag();
	s->Receive((LPCSTR *)&tmp, sizeof(KPACKET));

	unpackMessage(tmp, s);

	AfxMessageBox(_T("receive Success!!")); 
}


void ServerConf::initFlag(void)
{
	TRACE("---------------------------initFlag---------------------------");
	nSocket = 0;
	m_applyFlag = FALSE;
	m_bDragFlag = FALSE;
	m_deviceFlag = -1;
	for(int i = 0 ; i < 9 ; i++ ){
		m_settingFlag[i] = -1;
		client_id[i] = 0;
	}
	m_settingFlag[4] = 0;

	m_startFlag = TRUE;
	m_whereisPoint = 5; // 기본값 5 ( 서버에 위치 )
	m_keyBoardHook = FALSE;
	m_mouseHook = FALSE;

	tmp = NULL;
	cThread = NULL;
}




BOOL ServerConf::PreTranslateMessage(MSG* pMsg) // PostMessage를 받아 Parsing하는 함수.......................................................
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}


//void ServerConf::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
//
//	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
//}



KPACKET ServerConf::packMessage(int msgType, int sendDev, int recvDev, int devType, int relativeField, int updownFlag, int pad1, int keyCode, int pad2, int pad3){
	TRACE("---------------------------packMessage---------------------------");
	KPACKET k;
	k.msgType = 0;
	k.sendDev = 0;
	k.recvDev = 0;
	k.deviceType = 0;
	k.relativeField = 0;
	k.updownFlag = 0;;
	k.pad1 = 0;
	k.keyCode = 0;
	k.pad2 = 0;
	k.pad3 = 0;
	switch(msgType){
	case 1: // Keyboard
		k.msgType = msgType;
		k.sendDev = sendDev;
		k.recvDev = recvDev;
		k.deviceType = devType;
		k.relativeField = relativeField;
		k.updownFlag = updownFlag;;
		k.pad1 = pad1;
		k.keyCode = keyCode;
		k.pad2 = pad2;
		k.pad3 = pad3;

	case 2: // Mouse
		MPACKET m;
		m.msgType = msgType;
		m.sendDev = sendDev;
		m.recvDev = recvDev;
		m.deviceType = devType;
		m.relativeField = relativeField;
		m.updownFlag = updownFlag;
		m.leftRight = pad1;
		m.wheelFlag = keyCode;
		m.xCoord = pad2;
		m.yCoord = pad3;

		memcpy(&k, &m, sizeof(KPACKET));


	case 3: // Client
		CPACKET c;
		c.msgType = msgType;
		c.c_id = sendDev;
		c.pad3 = recvDev;
		c.hello = devType;
		c.bye = relativeField;
		c.pad4 = updownFlag;
		c.pad5 = pad1;
		c.pad6 = keyCode;
		c.pad7 = pad2;
		c.pad8 = pad3;

		memcpy(&k, &c, sizeof(KPACKET));

		break;

	case 4: // Server
		DPACKET d;
		d.msgType = msgType;
		d.len = sendDev;

		memcpy(&k, &d, sizeof(KPACKET));
		break;

	default: // packing 실패
		AfxMessageBox(_T("Msg Packing Failed"));
		break;
	}

	return k;


}

void ServerConf::unpackMessage(KPACKET p, CMySocket *s){
	TRACE("---------------------------unpackMessage---------------------------\n");
	switch(p.msgType){
	case 1: // Keyboard
		/*		k.deviceType = p.deviceType;
		k.keyCode = p.keyCode;
		k.msgType = p.msgType;
		k.sendDev = p.sendDev;
		k.recvDev = p.recvDev;
		k.updownFlag = p.updownFlag;
		k.relativeField = p.relativeField;*/

		break;

	case 2: // Mouse

		//m.deviceType = p.deviceType;
		//m.msgType = p.msgType;
		//m.sendDev = p.sendDev;
		//m.recvDev = p.recvDev;
		//m.relativeField = p.relativeField;
		//m.updownFlag = p.updownFlag;
		//m.wheelFlag = p.pad1;
		//m.xCoord = p.keyCode;
		//m.yCoord = p.pad2;

		break;

	case 3: // Client
		CPACKET c;

		c.c_id = p.sendDev;
		c.msgType = p.msgType;
		c.hello = p.deviceType;
		c.bye = p.relativeField;

		if(c.hello == 1){ // 헬로 패킷인 경우 그 클라이언트 id를 알려준다.

			nSocket += 1;

			for(int i = 0 ; i < 9 ; i ++){ // 클라이언트 id와 버튼 연결
				if((m_settingFlag[i] != -1) && (i != 4) && (client_id[i] == 0)){
					if(nSocket > 4){
						client_id[i] = (nSocket + 1); //server(5) 때문에 클라이언트 수보다 하나 더한 값이 클라이언트 id가 된다.
						break;
					} else if(nSocket <= 4){
						client_id[i] = nSocket;
						break;
					}
				}
			}
			TRACE("after nSocket : %d\n", nSocket);
			c.hello = 0;
			keyP = packMessage(3, nSocket, 0, 1, 0, 0, 0, 0, 0, 0);
			TRACE("unpackMessage nSocket : %d\n", nSocket);

			s->Send((LPCSTR)&keyP, sizeof(KPACKET));

			if(nSocket == 1){
				installKeyhook();
				installMousehook();
			}
		}

		if(c.bye == 1){ // bye 패킷인 경우
			nSocket -= 1;

			client_id[c.c_id - 1] = 0;

			if(nSocket == 0){
				initFlag();
				uninstallKeyhook();
				uninstallMousehook();
			}
		}

		//	PostMessage(WM_CLIENT_MESSAGE, 0, 0);

		break;

	case 4: // Server
		DPACKET d;

		d.msgType = p.msgType;
		d.len = p.sendDev;
		//		PostMessage(WM_SERVER_MESSAGE, 0, 0);

		break;

	default: // unpacking 실패
		AfxMessageBox(_T("Unpacking Failed!"));
	}

	return ;
}

BOOL ServerConf::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	TRACE("---------------------------OnCopyData---------------------------\n");
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	HEVENT *hEVENT; 
	MPACKET *mEVENT;
	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYSCREEN);
	HWND dllWnd = FindWindowA(NULL, "MsgWnd");
	TRACE("nWidth : %d, nHeight : %d\n", nWidth, nHeight);

	tmp = &listen.m_sockList; // 쓰레드 리스트 가져옴
	int count = tmp->GetCount();
	POSITION pos = tmp->GetHeadPosition(); // 맨 처음 헤드를 포인팅함
	//TRACE("count : %d\n", count);
	switch(pCopyDataStruct -> dwData){

	case 0: // keyboard
		hEVENT = (tagHEVENT *) pCopyDataStruct->lpData; // hEvent 구조체 연결(후킹된 자료)

		if(hEVENT->lParam >= 0){ // 키가 눌렸을 때

			keyP.deviceType = 1;
			keyP.msgType = 1;
			keyP.keyCode = hEVENT->keyCode;

			//for(int i = 0 ; i < 9 ; i ++){
			//	tmp->GetNext(pos);
			//	if((m_whereisPoint == i) && (i != 5)){ // 
			//		cThread = (CMyThread *)tmp->GetAt(pos);
			//		cThread->m_mySocket->Send((LPCSTR)&keyP, sizeof(KPACKET));
			//	}
			//}
		}
		break;
	case 1: // 마우스 정보를 얻어오면
		mEVENT = (MPACKET *)pCopyDataStruct->lpData; // mEvent 구조체 연결(후킹된 자료)
		if(mEVENT->xCoord <= 2){ // 화면 왼쪽에 붙을 때
			TRACE("m_whereisPoint : %d, client_id[3] : %d, m_settingflag[3] : %d\n", m_whereisPoint, m_settingFlag[3], client_id[3]);

			if(m_whereisPoint == 5 && m_settingFlag[3] != -1 && client_id[3] != 0){ // 4번 버튼이 비지 않았고, 서버 쪽에 포인트가 있으면,
				m_whereisPoint = 4; // 4번 버튼으로 포인트를 옮긴 후 마우스 이동 
				mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, (nWidth - 15)* 65535 / nWidth, mEVENT->yCoord * 65535 / nHeight, 0, 0);

				// 마우스 커서 없애고 키보드, 마우스 이벤트 처리 전부 무시하게 만듬



			} else if(m_whereisPoint == 6){ // 만약 6번에 포인터가 있었다면
				m_whereisPoint = 5; // 서버쪽으로 포인트를 옮긴 후 마우스 이동
				mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, (nWidth - 15)* 65535 / nWidth, mEVENT->yCoord * 65535 / nHeight, 0, 0);

				// 마우스 커서 출력하고 마우스, 키보드 전부 다시 이벤트 받도록 함


			} else
				return true;
			//for(int i = 0 ; i < client_id[3] - 1; i ++){ // 이게... 0부터 시작하는거니깐 id의 값보다 1만큼 작게 for문이 돌아야 정확한 pos가 나온다.
			//	tmp->GetNext(pos);
			//}

			//cThread = (CMyThread *)tmp->GetAt(pos);
			//cThread->m_mySocket->Send((LPCSTR)&keyP, sizeof(KPACKET));
		} 

		if(mEVENT->yCoord<= 2) { // 화면 위족에 붙을 때
			TRACE("m_whereisPoint : %d, client_id[1] : %d, m_settingflag[1] : %d\n", m_whereisPoint, m_settingFlag[1], client_id[1]);

			if(m_whereisPoint == 5 && m_settingFlag[1] != -1 && client_id[1] != 0){ // 2번 버튼이 비지 않았고, 서버 쪽에 포인트가 있으면,
				m_whereisPoint = 2; // 2번 버튼으로 옮긴 후 마우스 이동
				mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, mEVENT->xCoord * 65535 / nWidth, (nHeight - 15) * 65535 / nHeight, 0, 0);

				// 마우스 커서 없애고 키보드, 마우스 이벤트 처리 전부 무시하게 만듬


			} else if(m_whereisPoint == 8){ // 8번에 포인터가 있었다면
				m_whereisPoint = 5; // 서버버튼으로 옮긴 후 마우스 이동
				mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, mEVENT->xCoord * 65535 / nWidth, (nHeight - 15) * 65535 / nHeight, 0, 0);


				// 마우스 커서 출력하고 마우스, 키보드 전부 다시 이벤트 받도록 함



			} else
				return true;

			//for(int i = 0 ; i < client_id[1] - 1; i ++){ // 이게... 0부터 시작하는거니깐 id의 값보다 1만큼 작게 for문이 돌아야 정확한 pos가 나온다.
			//	tmp->GetNext(pos);
			//}
			//cThread = (CMyThread *)tmp->GetAt(pos);
			//cThread->m_mySocket->Send((LPCSTR)&keyP, sizeof(KPACKET));

		} 

		if(mEVENT->xCoord >= nWidth - 2){	 // 화면 오른 쪽에 붙을 때
			TRACE("m_whereisPoint : %d, client_id[5] : %d, m_settingflag[5] : %d\n", m_whereisPoint, m_settingFlag[5], client_id[5]);

			if(m_whereisPoint == 5 && m_settingFlag[5] != -1 && client_id[5] != 0){ // 6번 버튼이 비지 않았고, 서버 쪽에 포인트가 있으면,
				m_whereisPoint = 6;
				mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 15 * 65535 / nWidth, mEVENT->yCoord * 65535 / nHeight, 0, 0);

				// 마우스 커서 없애고 키보드, 마우스 이벤트 처리 전부 무시하게 만듬
				


			} else if(m_whereisPoint == 4){
				m_whereisPoint = 5;
				mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 15 * 65535 / nWidth, mEVENT->yCoord * 65535 / nHeight, 0, 0);

				// 마우스 커서 출력하고 마우스, 키보드 전부 다시 이벤트 받도록 함


			}else
				return true;

			//for(int i = 0 ; i < client_id[5] - 1; i ++){ // 이게... 0부터 시작하는거니깐 id의 값보다 1만큼 작게 for문이 돌아야 정확한 pos가 나온다.
			//	tmp->GetNext(pos);
			//}
			//cThread = (CMyThread *)tmp->GetAt(pos);
			//cThread->m_mySocket->Send((LPCSTR)&keyP, sizeof(KPACKET));
		} 

		if(mEVENT->yCoord >= nHeight - 2){ // 화면 아래쪽에 붙을 때
			TRACE("m_whereisPoint : %d, client_id[7] : %d, m_settingflag[7] : %d\n", m_whereisPoint, m_settingFlag[7], client_id[7]);

			if(m_whereisPoint == 5 && m_settingFlag[7] != -1 && client_id[7] != 0){ // 8번 버튼이 비지 않았고, 서버 쪽에 포인트가 있으면,
				m_whereisPoint = 8;
				mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, mEVENT->xCoord * 65535 / nWidth, 15 * 65535 / nHeight, 0, 0);

				// 마우스 커서 없애고 키보드, 마우스 이벤트 처리 전부 무시하게 만듬


			} else if(m_whereisPoint == 2){
				m_whereisPoint = 5;
				mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, mEVENT->xCoord * 65535 / nWidth, 15 * 65535 / nHeight, 0, 0);


				// 마우스 커서 출력하고 마우스, 키보드 전부 다시 이벤트 받도록 함


			}else
				return true;


			//for(int i = 0 ; i < client_id[7] - 1; i ++){ // 이게... 0부터 시작하는거니깐 id의 값보다 1만큼 작게 for문이 돌아야 정확한 pos가 나온다.
			//	tmp->GetNext(pos);
			//}	
			//cThread = (CMyThread *)tmp->GetAt(pos);
			//cThread->m_mySocket->Send((LPCSTR)&keyP, sizeof(KPACKET));

		}
		break;
	}

	tmp = NULL;
	cThread = NULL;


	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}

void ServerConf::OnBnClickedButton14() // 서버 Stop하는 함수
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	initFlag();
	m_CBtn_Start.EnableWindow(TRUE);
	uninstallKeyhook();
	uninstallMousehook();

}
