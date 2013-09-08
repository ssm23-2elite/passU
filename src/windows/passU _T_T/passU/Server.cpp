// Server.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "Server.h"
#include "afxdialogex.h"
#include "PassUDlg.h"

#define WM_RECEIVEMSG	(WM_USER + 1000)


// 후킹을 위한 함수 포인터
typedef HHOOK (*InstallKeyboardHook)();
typedef HHOOK (*InstallMouseHook)();
//typedef void (*SetWindowHandleToDll(HWND))();
typedef void (*UnInstallKeyboardHook)();
typedef void  (*UnInstallMouseHook)();

InstallKeyboardHook installKeyhook;
InstallMouseHook installMousehook;

UnInstallKeyboardHook uninstallKeyhook;
UnInstallMouseHook uninstallMousehook;

//SetWindowHandleToDll setWindowHandle;

#include "uvcview.h"
#include "vndrlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NUM_STRING_DESC_TO_GET 32

#pragma comment (lib, "Setupapi.lib")

LIST_ENTRY EnumeratedHCListHead =
{
	&EnumeratedHCListHead,
	&EnumeratedHCListHead
};

DEVICE_GUID_LIST gHubList;
DEVICE_GUID_LIST gDeviceList;

PCHAR ConnectionStatuses[] =
{
	"",                   // 0  - NoDeviceConnected
	"",                   // 1  - DeviceConnected
	"FailedEnumeration",  // 2  - DeviceFailedEnumeration
	"GeneralFailure",     // 3  - DeviceGeneralFailure
	"Overcurrent",        // 4  - DeviceCausedOvercurrent
	"NotEnoughPower",     // 5  - DeviceNotEnoughPower
	"NotEnoughBandwidth", // 6  - DeviceNotEnoughBandwidth
	"HubNestedTooDeeply", // 7  - DeviceHubNestedTooDeeply
	"InLegacyHub",        // 8  - DeviceInLegacyHub
	"Enumerating",        // 9  - DeviceEnumerating
	"Reset"               // 10 - DeviceReset
};

ULONG TotalDevicesConnected;

USBSENDDEVICEDESC sendToDeviceDescData;

int GetPassUSBDesc();

// CServer 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServer, CDialogEx)

	CServer::CServer(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServer::IDD, pParent)
	, serverIPAddress(_T(""))
{

}

CServer::~CServer()
{

}

void CServer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IPADDRESS, serverIPAddress);
	DDX_Control(pDX, IDC_BUTTON1, m_cBtn[0]);
	DDX_Control(pDX, IDC_BUTTON2, m_cBtn[1]);
	DDX_Control(pDX, IDC_BUTTON3, m_cBtn[2]);
	DDX_Control(pDX, IDC_BUTTON4, m_cBtn[3]);
	DDX_Control(pDX, IDC_BUTTON5, m_cBtn[4]);
	DDX_Control(pDX, IDC_BUTTON6, m_cBtn[5]);
	DDX_Control(pDX, IDC_BUTTON7, m_cBtn[6]);
	DDX_Control(pDX, IDC_BUTTON8, m_cBtn[7]);
	DDX_Control(pDX, IDC_BUTTON9, m_cBtn[8]);
	DDX_Control(pDX, IDC_LIST1, m_waiting_client);
}


BEGIN_MESSAGE_MAP(CServer, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CServer::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CServer::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CServer::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CServer::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, &CServer::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CServer::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CServer::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CServer::OnBnClickedButton9)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_COPYDATA()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST1, &CServer::OnLvnBegindragList1)
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CServer::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	TRACE("CServer Init\n");

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

	SetDlgItemText(IDC_IPADDRESS, strIpAddress);

	hinstDLL = LoadLibrary(_T("ServerHook.dll"));
	if(!hinstDLL)
		AfxMessageBox(_T("ServerHook.dll 로드 실패!"));

	installKeyhook = (InstallKeyboardHook)GetProcAddress(hinstDLL, "InstallKeyboardHook");
	installMousehook = (InstallMouseHook)GetProcAddress(hinstDLL, "InstallMouseHook");

	uninstallKeyhook = (UnInstallKeyboardHook)GetProcAddress(hinstDLL, "UnInstallKeyboardHook");
	uninstallMousehook = (UnInstallMouseHook)GetProcAddress(hinstDLL, "UnInstallMouseHook");

	dllWnd = ::FindWindow("MsgWnd", NULL);

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	for(int i = 0 ; i < 9 ; i ++){
		ZeroMemory(&btnControl[i], sizeof(btnControl[i]));
		client_nWidth[i] = 0;
		client_nHeight[i] = 0;
	}

	m_keyBoardHook = FALSE;
	m_mouseHook = FALSE;
	m_bDrag = FALSE;

	m_nOldTarget = 0;
	m_nSource = 0;

	m_status = STATUS_EMPTY;
	m_x = 0;
	m_y = 0;

	m_bmp_monitor.LoadBitmapA(IDB_MONITOR);
	m_bmp_phone.LoadBitmapA(IDB_PHONE);
	m_imgList.Create(60, 60, ILC_COLOR24 | ILC_MASK, 1, 1);

	m_imgList.Add(&m_bmp_monitor, RGB(0, 0, 0));
	m_imgList.Add(&m_bmp_phone, RGB(0, 0, 0));

	btnControl[0] = this->GetDlgItem(IDC_BUTTON1);
	btnControl[1] = this->GetDlgItem(IDC_BUTTON2);
	btnControl[2] = this->GetDlgItem(IDC_BUTTON3);
	btnControl[3] = this->GetDlgItem(IDC_BUTTON4);
	btnControl[4] = this->GetDlgItem(IDC_BUTTON5);
	btnControl[5] = this->GetDlgItem(IDC_BUTTON6);
	btnControl[6] = this->GetDlgItem(IDC_BUTTON7);
	btnControl[7] = this->GetDlgItem(IDC_BUTTON8);
	btnControl[8] = this->GetDlgItem(IDC_BUTTON9);

	for(int i = 0 ; i < 9 ; i ++){
		btnControl[i]->GetWindowPlacement(&getCoord[i]);
		btn_Bind[i] = 0;
	}
	m_pDragImage = NULL;

	installKeyhook();

	return TRUE;
}
// CServer 메시지 처리기입니다.




void CServer::OnChangeLocationInfo(int index)
{
}


void CServer::bindWatingClient(int btn_index, int client_index)
{
	// 클라이언트 index를 넣어줘서 버튼하고 binding시키는 것

	btn_Bind[btn_index] = client_index;
	clientInfo[client_index].setPosition(btn_index);

}

void CServer::OnArrivedScreenEdge(int position)
{

}


void CServer::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(btn_Bind[0] != 0){
		if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			m_cBtn[0].SetBitmap(NULL);
			btn_Bind[0] = 0;
			RedrawWindow();
		}
	}
}


void CServer::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(btn_Bind[1] != 0){
		if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			m_cBtn[1].SetBitmap(NULL);
			btn_Bind[1] = 0;
			RedrawWindow();
		}
	}
}


void CServer::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(btn_Bind[2] != 0){
		if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			m_cBtn[2].SetBitmap(NULL);
			btn_Bind[2] = 0;
			RedrawWindow();
		}
	}
}


void CServer::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(btn_Bind[3] != 0){
		if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			m_cBtn[3].SetBitmap(NULL);
			btn_Bind[3] = 0;
			RedrawWindow();
		}
	}
}


void CServer::OnBnClickedButton6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(btn_Bind[5] != 0){
		if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			m_cBtn[5].SetBitmap(NULL);
			btn_Bind[5] = 0;
			RedrawWindow();
		}
	}
}


void CServer::OnBnClickedButton7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(btn_Bind[6] != 0){
		if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			m_cBtn[6].SetBitmap(NULL);
			btn_Bind[6] = 0;
			RedrawWindow();
		}
	}
}


void CServer::OnBnClickedButton8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(btn_Bind[7] != 0){
		if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			m_cBtn[7].SetBitmap(NULL);
			btn_Bind[7] = 0;
			RedrawWindow();
		}
	}
}


void CServer::OnBnClickedButton9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(btn_Bind[8] != 0){
		if(AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION)==IDYES)
		{
			m_cBtn[8].SetBitmap(NULL);
			btn_Bind[8] = 0;
			RedrawWindow();
		}
	}
}


void CServer::OnLButtonUp(UINT nFlags, CPoint point)
{
	CPoint m_tmpPoint;
	CBitmap m_tmpBitmap;

	LVITEM Item; 


	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_bDrag)
	{
		if (NULL == m_pDragImage)
			return ;

		m_waiting_client.SetItemState(m_nOldTarget, 0, LVIS_DROPHILITED);

		//드래그종료   
		m_pDragImage->DragLeave(NULL);
		ReleaseCapture();                
		m_pDragImage->EndDrag();       

		m_bDrag = false;
		delete m_pDragImage;
		m_pDragImage = NULL;       
		// 아이템위치변경
		INT nDest = GetHitIndex(point);       

		TCHAR Name[256 + 1] = {0,}; 

		Item.iItem = m_nSource;   
		Item.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM | LVIF_STATE;   
		Item.stateMask = LVIS_FOCUSED | LVIS_SELECTED | LVIS_STATEIMAGEMASK | LVIS_OVERLAYMASK;    
		Item.pszText = Name;   
		Item.cchTextMax = 256;   
		Item.iSubItem = 0;   
		m_waiting_client.GetItem(&Item);   

		Item.iItem = nDest;   
		//m_waiting_client.InsertItem(&Item);   

		for(int i = 0; i < sizeof(m_cBtn)/sizeof(m_cBtn[0]); i++) {
			if(point.x >= getCoord[i].rcNormalPosition.left &&
				point.x <= getCoord[i].rcNormalPosition.right &&
				point.y <= getCoord[i].rcNormalPosition.bottom &&
				point.y >= getCoord[i].rcNormalPosition.top){ // 드래그 놓은 곳이 버튼 1일 때

					// 처음에 클릭한 그림이 스마트폰인지 컴퓨터인지 걸러내서
					// 컴퓨터이면 버튼에 컴퓨터 삽입, 스마트폰이면 버튼에 스마트폰 삽입
					if(Item.lParam == STATUS_PC){
						m_cBtn[i].SetBitmap(HBITMAP(m_bmp_monitor));
					} else{
						m_cBtn[i].SetBitmap(HBITMAP(m_bmp_phone));
					}

					//TRACE("%s\n", Item.pszText);
					CString tmpStr = Item.pszText;
					char c = tmpStr.GetAt(0);
					int cid = c - '0';

					bindWatingClient(i, cid); // 버튼에 클라이언트를 바인딩 시켜주는 함수

					UpdateData();
					Invalidate();
					RedrawWindow();
					break;
			}
		}
	}   
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CServer::MoveListItem(const INT _nSource, const INT _nDest)
{
	if ((_nSource == _nDest) || (0 > _nDest))
		return ;

	INT nSource = _nSource;   
	INT nColumn = m_waiting_client.GetHeaderCtrl()->GetItemCount();   
	TCHAR Name[256 + 1] = {0,};   

	LVITEM Item;   
	Item.iItem = nSource;   
	Item.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM | LVIF_STATE;   
	Item.stateMask = LVIS_FOCUSED | LVIS_SELECTED | LVIS_STATEIMAGEMASK | LVIS_OVERLAYMASK;    
	Item.pszText = Name;   
	Item.cchTextMax = 256;   
	Item.iSubItem = 0;   
	m_waiting_client.GetItem(&Item);   

	Item.iItem = _nDest;   
	m_waiting_client.InsertItem(&Item);




	if (nSource > _nDest)
		nSource ++;       

	for (INT i= 1; i<nColumn; i++)
	{   
		CString sText = m_waiting_client.GetItemText(nSource, i);       
		m_waiting_client.SetItemText(_nDest, i, sText);       
	}   
	m_waiting_client.DeleteItem(nSource);   
}

void CServer::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnLButtonDown(nFlags, point);
}

BOOL CServer::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPassUChildSocket *s = NULL;

	// Main Dialog 포인터를 받아옴
	CPassUDlg * pMainDlg = (CPassUDlg *)::AfxGetMainWnd();
	POSITION pos = pMainDlg->m_pSockList.GetHeadPosition();

	TRACE("OnCopyData\n");


	switch(pCopyDataStruct->dwData){
	case KEYBOARD_DATA: // keyboard
		//hEVENT = (tagHEVENT *) pCopyDataStruct->lpData; // hEvent 구조체 연결(후킹된 자료)

		break;


	case MOUSE_DATA: // mouse
		//mEVENT = (MPACKET *)pCopyDataStruct->lpData; // mEvent 구조체 연결(후킹된 자료)

		break;


	case RECEIVE_DATA: // receiveData
		CPACKET* clientP = (CPACKET *) pCopyDataStruct->lpData; // 구조체 연결

		if(clientP->hello == 1){ // hello packet
			// 헬로 패킷이면 클라이언트 정보 설정
			// 버튼에 클라이언트 연결해주고, 첫번째 클라이언트면 후킹 시작하고
			// 클라이언트에 자신의 id 알려주고
			// 리스트 컨트롤에 아이콘 하나 추가
			//	TRACE("client\n");

			if(pMainDlg->m_pSockList.GetCount() == 1){

				m_waiting_client.SetImageList(&m_imgList, LVSIL_NORMAL);


				installKeyhook();
				installMousehook();
			}

			for(int i = 0 ; i < 9 ; i ++){
				if(clientInfo[i].clientID == 0){
					// client ID Setting
					clientInfo[i].setID(i + 1);

					clientP->c_id = ((CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos))->c_id = clientInfo[i].getID();

					// 클라이언트의 화면 크기 구해서 저장
					client_nWidth[i + 1] = clientP->nWidth;
					client_nHeight[i + 1] = clientP->nHeight;
					
					// Client IP Address Setting
					clientInfo[i].m_address.Format(_T("%d.%d.%d.%d"), clientP->ipFirst, clientP->ipSecond, clientP->ipThird, clientP->ipForth);

					//	TRACE("?\n");
					if(clientP->pad3 == STATUS_PC){ // PC 이면
						// Status 세팅
						clientInfo[i].setStatus(STATUS_PC);

						//	TRACE("Status Setting\n");
						// 정보 저장할 LVITEM 세팅
						LVITEM lvitem;
						CString tmpStr;
						//		TRACE("InstallHook 주석처리함\n");
						tmpStr.Format(_T("%d , IP : %s"), clientP->c_id, clientInfo[i].m_address);

						ZeroMemory(&lvitem, sizeof(lvitem));
						lvitem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
						lvitem.iItem = 0;
						lvitem.pszText = (LPSTR)(LPCSTR)tmpStr;
						lvitem.lParam = (LPARAM)STATUS_PC;
						lvitem.iImage = STATUS_PC - 1;

						m_waiting_client.InsertItem(&lvitem);
						break;
					} else if(clientP->pad3 == STATUS_MOBILE){

						// Status 세팅
						clientInfo[i].setStatus(STATUS_MOBILE);

						//정보 저장할 LVITEM 세팅
						LVITEM lvitem;
						CString tmpStr;
						tmpStr.Format(_T("%d , IP : %s"), clientP->c_id, clientInfo[i].m_address);

						ZeroMemory(&lvitem, sizeof(lvitem));
						lvitem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
						lvitem.iItem = 0;
						lvitem.pszText = (LPSTR)(LPCSTR)tmpStr;
						lvitem.lParam = (LPARAM)STATUS_MOBILE;
						lvitem.iImage = STATUS_MOBILE - 1;
						m_waiting_client.InsertItem(&lvitem);
						break;

					} else{
						AfxMessageBox(_T("status nothing!!"));
						clientInfo[i].setID(0);
						clientInfo[i].setStatus(STATUS_EMPTY);
						return FALSE;
					}
				}
				else
					pMainDlg->m_pSockList.GetNext(pos);
			}

			char buf[1024];
			ZeroMemory(buf, sizeof(buf));
			sprintf_s(buf, "%4d%4d%4d%1d%1d%4d%4d%4d%4d%5d%5d",
				MSG_CLIENT, clientP->c_id, clientP->pad3, clientP->hello,clientP->bye, clientP->ipFirst, clientP->ipSecond, clientP->ipThird, clientP->ipForth,
				0, 0);
			((CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos))->Send(buf, sizeof(CPACKET));
			
			/*if(clientP->pad3 == STATUS_PC) {
				GetPassUSBDesc();
				
				DPACKET packet;
				packet.msgType = MSG_DATA;
				packet.len = sizeof(USBSENDDEVICEDESC);
				memcpy(&packet.usbdesc, &sendToDeviceDescData, sizeof(USBSENDDEVICEDESC));

				((CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos))->Send((char*)&packet, sizeof(DPACKET));
			}*/
		} else if(clientP->bye == 1){ // bye packet
			// 굿바이패킷이면
			// 버튼에 클라이언트 해제
			s =  ((CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos));
			while(clientP->c_id == s->c_id){
				((CPassUChildSocket *)pMainDlg->m_pSockList.GetNext(pos));
				s =  ((CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos));
			}

			if(pMainDlg->m_pSockList.GetCount() == 0){
				uninstallKeyhook();
				uninstallMousehook();
			}
		}
		break;
	}
	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}


void CServer::OnLvnBegindragList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if (0 >= m_waiting_client.GetSelectedCount())
		return ;

	m_nSource = pNMLV->iItem;

	//드래그이미지리스트생성
	POINT ptImg;
	m_pDragImage = m_waiting_client.CreateDragImage(m_nSource, &ptImg);
	if (NULL == m_pDragImage)
		return ;

	//핫스팟
	CPoint ptSpot;
	ptSpot.x = pNMLV->ptAction.x - ptImg.x;
	ptSpot.y = pNMLV->ptAction.y - ptImg.y;
	m_pDragImage->BeginDrag(0, ptSpot);

	//현재마우스커서위치에드래그이미지그림
	ptImg.x = pNMLV->ptAction.x;
	ptImg.y = pNMLV->ptAction.y;
	ClientToScreen(&ptImg);
	m_pDragImage->DragEnter(NULL, ptImg);

	m_nOldTarget = m_nSource;
	m_bDrag = true;
	SetCapture();
}


void CServer::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_bDrag)
	{
		if (NULL == m_pDragImage)
			return ;

		// 드래그이미지이동
		CPoint ptCursor(point);
		ClientToScreen(&ptCursor);
		m_pDragImage->DragMove(ptCursor);

		// 커서위치의리스트아이템찾아서선택표시를해준다.
		INT nTarget = GetHitIndex(point);
		if (-1 != nTarget)
		{
			if (m_nOldTarget != nTarget)
			{
				m_pDragImage->DragLeave(NULL);
				m_waiting_client.SetItemState(nTarget, 
					LVIS_DROPHILITED, LVIS_DROPHILITED);

				m_waiting_client.SetItemState(m_nOldTarget, 0, 
					LVIS_DROPHILITED);
				m_nOldTarget = nTarget;

				m_pDragImage->DragEnter(NULL, ptCursor);
				m_waiting_client.UpdateWindow();
			}
		}
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

int CServer::GetHitIndex(CPoint point) // 커서 위치의 리스트아이템 인덱스를 찾는다.
{

	CRect rcList;
	m_waiting_client.GetWindowRect(&rcList);
	ScreenToClient(reinterpret_cast<LPPOINT>(&rcList));

	LVHITTESTINFO HitInfo;
	HitInfo.pt.x = point.x - rcList.left;
	HitInfo.pt.y = point.y - rcList.top;

	return m_waiting_client.HitTest(&HitInfo);
}

PACKET CServer::packMessage(int msgType, int sendDev, int recvDev, int deviceType, int relativeField, int updownFlag, int pad1, int keyCode, int pad2, int pad3, int pad4)
{
	PACKET tmp;

	tmp.msgType = msgType;
	tmp.sendDev = sendDev;
	tmp.recvDev = recvDev;
	tmp.deviceType = deviceType;
	tmp.relativeField = relativeField;
	tmp.updownFlag = updownFlag;
	tmp.pad1 = pad1;
	tmp.keyCode = keyCode;
	tmp.pad2 = pad2;
	tmp.pad3 = pad3;
	tmp.pad4 = pad4;

	return tmp;
}

void CServer::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_bmp_monitor.Detach();
	m_bmp_phone.Detach();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

int GetPassUSBDesc()
{
	HANDLE                           hHCDev = NULL;
	HDEVINFO                         deviceInfo = NULL;
	SP_DEVINFO_DATA                  deviceInfoData;
	SP_DEVICE_INTERFACE_DATA         deviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetailData = NULL;
	ULONG                            index = 0;
	ULONG                            requiredLength = 0;
	BOOL                             success;

	ZeroMemory(&sendToDeviceDescData, sizeof(USBSENDDEVICEDESC));
	gHubList.DeviceInfo = INVALID_HANDLE_VALUE;
	InitializeListHead(&gHubList.ListHead);
	gDeviceList.DeviceInfo = INVALID_HANDLE_VALUE;
	InitializeListHead(&gDeviceList.ListHead);

	TotalDevicesConnected = 0;
	TotalHubs = 0;

	// GUID 기반 인터페이스를 사용하여 호스트 컨트롤러를 반복
	// Handle 반환
	//
	deviceInfo = SetupDiGetClassDevs((LPGUID)&GUID_CLASS_USB_HOST_CONTROLLER,
		NULL,
		NULL,
		(DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

	deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (index=0;
		SetupDiEnumDeviceInfo(deviceInfo,
		index,
		&deviceInfoData); // 디바이스 정보 요소를 명시하는 구조체 반환
	index++)
	{
		deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		success = SetupDiEnumDeviceInterfaces(deviceInfo,
			0,
			(LPGUID)&GUID_CLASS_USB_HOST_CONTROLLER,
			index,
			&deviceInterfaceData); // 디바이스 interfaces 를 열거
		if (!success)
		{
			OOPS();
			break;
		}

		success = SetupDiGetDeviceInterfaceDetail(deviceInfo,
			&deviceInterfaceData,
			NULL,
			0,
			&requiredLength,
			NULL); // 디바이스 interface의 정보를 읽어온다. 크기와 함께

		if (!success && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			OOPS();
			break;
		}

		deviceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)ALLOC(requiredLength);
		if (deviceDetailData == NULL)
		{
			OOPS();
			break;
		}

		deviceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		success = SetupDiGetDeviceInterfaceDetail(deviceInfo,
			&deviceInterfaceData,
			deviceDetailData,
			requiredLength,
			&requiredLength,
			NULL); // 디바이스의 경로를 가져온다.

		if (!success)
		{
			OOPS();
			break;
		}

		hHCDev = CreateFile(deviceDetailData->DevicePath,
			GENERIC_WRITE, // 읽기? 쓰기?
			FILE_SHARE_WRITE, // 동시에 쓰기가능
			NULL,
			OPEN_EXISTING, // 있으면 열어
			0,
			NULL); // 해당 Path에 있는 디바이스의 Handle을 얻는다.

		// Handle 얻기를 성공한 경우 그 디바이스의 RootHub의 내용을 가져온다.
		//
		if (hHCDev != INVALID_HANDLE_VALUE)
		{

			EnumerateHostController(hHCDev,
				deviceDetailData->DevicePath,
				deviceInfo,
				&deviceInfoData);


			CloseHandle(hHCDev);
		}

		FREE(deviceDetailData);
	}

	SetupDiDestroyDeviceInfoList(deviceInfo); // deviceInfo 메모리 free

	return 0;
}

VOID
	EnumerateHostController (
	HANDLE                   hHCDev,    _Inout_ PCHAR            leafName,
	_In_    HANDLE           deviceInfo,
	_In_    PSP_DEVINFO_DATA deviceInfoData
	)
{
	PCHAR                   driverKeyName = NULL;
	PCHAR                   rootHubName = NULL;
	PLIST_ENTRY             listEntry = NULL;
	PUSBHOSTCONTROLLERINFO  hcInfo = NULL;
	PUSBHOSTCONTROLLERINFO  hcInfoInList = NULL;
	DWORD                   dwSuccess;
	BOOL                    success = FALSE;
	ULONG                   deviceAndFunction = 0;
	PUSB_DEVICE_PNP_STRINGS DevProps = NULL;


	// 메모리 할당
	//
	hcInfo = (PUSBHOSTCONTROLLERINFO)ALLOC(sizeof(USBHOSTCONTROLLERINFO));

	// 메모리 할당 실패
	//
	if (NULL == hcInfo)
		return;

	hcInfo->DeviceInfoType = HostControllerInfo; // enum -> 0

	// host controller로 부터 해당 드라이버 key 이름을 가져온다.
	//
	driverKeyName = GetHCDDriverKeyName(hHCDev);

	if (NULL == driverKeyName)
	{
		OOPS();
		FREE(hcInfo);
		return;
	}

	// 이미 존재할 경우 다시 열거 하지 않는다.
	//
	listEntry = EnumeratedHCListHead.Flink;

	while (listEntry != &EnumeratedHCListHead)
	{
		hcInfoInList = CONTAINING_RECORD(listEntry,
			USBHOSTCONTROLLERINFO,
			ListEntry);

		if (strcmp(driverKeyName, hcInfoInList->DriverKey) == 0)
		{
			// 이미 리스트에 존재 즉, 열거 안함
			//
			FREE(driverKeyName);
			FREE(hcInfo);
			return;
		}

		listEntry = listEntry->Flink;
	}

	// host controller device 속성들을 얻음
	{
		size_t cbDriverName = 0;
		HRESULT hr = S_OK;

		hr = StringCbLength(driverKeyName, MAX_DRIVER_KEY_NAME, &cbDriverName);
		if (SUCCEEDED(hr))
		{
			DevProps = DriverNameToDeviceProperties(driverKeyName, cbDriverName);
		}
	}

	// 드라이버 키를 얻음 
	//
	hcInfo->DriverKey = driverKeyName;

	if (DevProps)
	{
		ULONG   ven, dev, subsys, rev;
		ven = dev = subsys = rev = 0;

		// DeviceId를 얻음
		//
		if (sscanf_s(DevProps->DeviceId,
			"PCI\\VEN_%x&DEV_%x&SUBSYS_%x&REV_%x",
			&ven, &dev, &subsys, &rev) != 4)
		{
			OOPS();
		}

		// 기타 정보들
		//
		hcInfo->VendorID = ven;
		hcInfo->DeviceID = dev;
		hcInfo->SubSysID = subsys;
		hcInfo->Revision = rev;
		hcInfo->UsbDeviceProperties = DevProps;
	}
	else
	{
		OOPS();
	}

	if (DevProps != NULL && DevProps->DeviceDesc != NULL)
	{
		leafName = DevProps->DeviceDesc;
	}
	else
	{
		OOPS();
	}

	// USB Host Controller의 power map을 얻는다.
	//
	dwSuccess = GetHostControllerPowerMap(hHCDev, hcInfo);

	if (ERROR_SUCCESS != dwSuccess)
	{
		OOPS();
	}


	// 버스, 디바이스 그리고 함수를 얻는다.
	//
	hcInfo->BusDeviceFunctionValid = FALSE;

	// 버스 얻는 부분
	//
	success = SetupDiGetDeviceRegistryProperty(deviceInfo,
		deviceInfoData,
		SPDRP_BUSNUMBER,
		NULL,
		(PBYTE)&hcInfo->BusNumber,
		sizeof(hcInfo->BusNumber),
		NULL);

	if (success)
	{
		// 디바이스의 주소를 얻는 부분
		//
		success = SetupDiGetDeviceRegistryProperty(deviceInfo,
			deviceInfoData,
			SPDRP_ADDRESS,
			NULL,
			(PBYTE)&deviceAndFunction,
			sizeof(deviceAndFunction),
			NULL);
	}

	if (success)
	{
		// 함수를 얻는 부분
		hcInfo->BusDevice = deviceAndFunction >> 16;
		hcInfo->BusFunction = deviceAndFunction & 0xffff;
		hcInfo->BusDeviceFunctionValid = TRUE;
	}

	// USB Host Controller info를 얻는 부분
	dwSuccess = GetHostControllerInfo(hHCDev, hcInfo);

	if (ERROR_SUCCESS != dwSuccess)
	{
		OOPS();
	}

	// 열거된 host controller들을 host controller 리스트에 추가
	//
	InsertTailList(&EnumeratedHCListHead,
		&hcInfo->ListEntry);

	// Root Hub의 이름을 얻는 부분
	// host controller와 root hub를 열거하기 위함
	//
	rootHubName = GetRootHubName(hHCDev);

	if (rootHubName != NULL)
	{
		size_t cbHubName = 0;
		HRESULT hr = S_OK;

		hr = StringCbLength(rootHubName, MAX_DRIVER_KEY_NAME, &cbHubName);
		if (SUCCEEDED(hr))
		{
			EnumerateHub(rootHubName,
				cbHubName,
				NULL,       // ConnectionInfo
				NULL,       // ConnectionInfoV2
				NULL,       // PortConnectorProps
				NULL,       // ConfigDesc
				NULL,       // BosDesc
				NULL,       // StringDescs
				NULL);      // We do not pass DevProps for RootHub
		}
	}
	else
	{
		// Failure obtaining root hub name.

		OOPS();
	}

	return;
}

PCHAR GetHCDDriverKeyName (
	HANDLE  HCD
	)
{
	BOOL                    success = 0;
	ULONG                   nBytes = 0;
	USB_HCD_DRIVERKEY_NAME  driverKeyName = {0};
	PUSB_HCD_DRIVERKEY_NAME driverKeyNameW = NULL;
	PCHAR                   driverKeyNameA = NULL;

	ZeroMemory(&driverKeyName, sizeof(driverKeyName));

	// Get the length of the name of the driver key of the HCD
	//
	success = DeviceIoControl(HCD,
		IOCTL_GET_HCD_DRIVERKEY_NAME, // 해당 함수 이름
		&driverKeyName, // inputbuffer의 주소
		sizeof(driverKeyName), // inputbuffer 크기
		&driverKeyName, // outputbuffer의 주소
		sizeof(driverKeyName), // outputbuffer 크기
		&nBytes,
		NULL); // 제어 코드를 직접적으로 해당 디바이스에 전송

	if (!success)
	{
		OOPS();
		goto GetHCDDriverKeyNameError;
	}

	nBytes = driverKeyName.ActualLength;
	if (nBytes <= sizeof(driverKeyName))
	{
		OOPS();
		goto GetHCDDriverKeyNameError;
	}

	// 메모리 할당
	driverKeyNameW = (PUSB_HCD_DRIVERKEY_NAME)ALLOC(nBytes + 1);
	if (driverKeyNameW == NULL)
	{
		OOPS();
		goto GetHCDDriverKeyNameError;
	}

	success = DeviceIoControl(HCD,
		IOCTL_GET_HCD_DRIVERKEY_NAME,
		driverKeyNameW,
		nBytes,
		driverKeyNameW,
		nBytes,
		&nBytes,
		NULL);
	if (!success)
	{
		OOPS();
		goto GetHCDDriverKeyNameError;
	}

	// Convert the driver key name
	//
	driverKeyNameA = WideStrToMultiStr(driverKeyNameW->DriverKeyName, nBytes);

	// All done, free the uncoverted driver key name and return the
	// converted driver key name
	//
	FREE(driverKeyNameW);

	return driverKeyNameA;

GetHCDDriverKeyNameError:
	// There was an error, free anything that was allocated
	//
	if (driverKeyNameW != NULL)
	{
		FREE(driverKeyNameW);
		driverKeyNameW = NULL;
	}

	return NULL;
}

PCHAR WideStrToMultiStr ( 
	_In_reads_bytes_(cbWideStr) PWCHAR WideStr, 
	_In_ size_t                   cbWideStr
	)
{
	ULONG  nBytes = 0;
	PCHAR  MultiStr = NULL;
	PWCHAR pWideStr = NULL;

	// Use local string to guarantee zero termination
	pWideStr = (PWCHAR) ALLOC((DWORD) cbWideStr + 1);
	if (NULL == pWideStr)
	{
		return NULL;
	}
	memset(pWideStr, 0, cbWideStr + 1);
	memcpy(pWideStr, WideStr, cbWideStr);

	// Get the length of the converted string
	//
	nBytes = WideCharToMultiByte(
		CP_ACP,
		WC_NO_BEST_FIT_CHARS,
		pWideStr,
		-1,
		NULL,
		0,
		NULL,
		NULL);

	if (nBytes == 0)
	{
		FREE(pWideStr);
		return NULL;
	}

	// Allocate space to hold the converted string
	//
	MultiStr = (PCHAR)ALLOC(nBytes);
	if (MultiStr == NULL)
	{
		FREE(pWideStr);
		return NULL;
	}

	// Convert the string
	//
	nBytes = WideCharToMultiByte(
		CP_ACP,
		WC_NO_BEST_FIT_CHARS,
		pWideStr,
		-1,
		MultiStr,
		nBytes,
		NULL,
		NULL);

	if (nBytes == 0)
	{
		FREE(MultiStr);
		FREE(pWideStr);
		return NULL;
	}

	FREE(pWideStr);
	return MultiStr;
}

/*****************************************************************************

DriverNameToDeviceProperties()

DriverName과 일치하는 DevNode 의 장치 속성들을 return.
맞는 DevNode가 없으면 NULL return.

호출자는 FREE()를 이용하여 반환된 구조체를 반드시 free.

*****************************************************************************/
PUSB_DEVICE_PNP_STRINGS 
	DriverNameToDeviceProperties( 
	_In_reads_bytes_(cbDriverName) PCHAR  DriverName, 
	_In_ size_t cbDriverName
	)
{
	HDEVINFO        deviceInfo = INVALID_HANDLE_VALUE;
	SP_DEVINFO_DATA deviceInfoData = {0};
	ULONG           len;
	BOOL            status;
	PUSB_DEVICE_PNP_STRINGS DevProps = NULL;
	DWORD           lastError;

	// 메모리 할당
	DevProps = (PUSB_DEVICE_PNP_STRINGS) ALLOC(sizeof(USB_DEVICE_PNP_STRINGS));

	if(NULL == DevProps)
	{
		status = FALSE;
		goto Done;
	}

	// device의 instance를 얻음
	status = DriverNameToDeviceInst(DriverName, cbDriverName, &deviceInfo, &deviceInfoData);
	if (status == FALSE)
	{
		goto Done;
	}

	len = 0;
	status = SetupDiGetDeviceInstanceId(deviceInfo,
		&deviceInfoData,
		NULL,
		0,
		&len);
	lastError = GetLastError();


	if (status != FALSE && lastError != ERROR_INSUFFICIENT_BUFFER)
	{
		status = FALSE;
		goto Done;
	}

	//
	// An extra byte is required for the terminating character
	//

	len++;
	DevProps->DeviceId = (PCHAR)ALLOC(len);

	if (DevProps->DeviceId == NULL)
	{
		status = FALSE;
		goto Done;
	}

	status = SetupDiGetDeviceInstanceId(deviceInfo,
		&deviceInfoData,
		DevProps->DeviceId,
		len,
		&len);
	if (status == FALSE)
	{
		goto Done;
	}

	status = GetDeviceProperty(deviceInfo,
		&deviceInfoData,
		SPDRP_DEVICEDESC,
		&DevProps->DeviceDesc);

	if (status == FALSE)
	{
		goto Done;
	}


	//    
	// We don't fail if the following registry query fails as these fields are additional information only
	//

	GetDeviceProperty(deviceInfo,
		&deviceInfoData,
		SPDRP_HARDWAREID,
		&DevProps->HwId);

	GetDeviceProperty(deviceInfo,
		&deviceInfoData,
		SPDRP_SERVICE,
		&DevProps->Service);

	GetDeviceProperty(deviceInfo,
		&deviceInfoData,
		SPDRP_CLASS,
		&DevProps->DeviceClass);
Done:

	if (deviceInfo != INVALID_HANDLE_VALUE)
	{
		SetupDiDestroyDeviceInfoList(deviceInfo);
	}

	if (status == FALSE)
	{
		if (DevProps != NULL)
		{
			FreeDeviceProperties(&DevProps);
		}
	}
	return DevProps;
}

/*****************************************************************************

DriverNameToDeviceInst()

DriverName과 일치하는 DevNode의 Device instance를 찾는다.
DevNode와 일치하는 것이 없으면 FALSE를 있다면 True를 return.

*****************************************************************************/
BOOL 
	DriverNameToDeviceInst(
	_In_reads_bytes_(cbDriverName) PCHAR DriverName,
	_In_ size_t cbDriverName,
	_Out_ HDEVINFO *pDevInfo,
	_Out_writes_bytes_(sizeof(SP_DEVINFO_DATA)) PSP_DEVINFO_DATA pDevInfoData
	)
{
	HDEVINFO         deviceInfo = INVALID_HANDLE_VALUE;
	BOOL             status = TRUE;
	ULONG            deviceIndex;
	SP_DEVINFO_DATA  deviceInfoData;
	BOOL             bResult = FALSE;
	PCHAR            pDriverName = NULL;
	PSTR             buf = NULL;
	BOOL             done = FALSE;

	if (pDevInfo == NULL)
	{
		return FALSE;
	}

	if (pDevInfoData == NULL)
	{
		return FALSE;
	}

	memset(pDevInfoData, 0, sizeof(SP_DEVINFO_DATA));

	*pDevInfo = INVALID_HANDLE_VALUE;

	// zero termination 을 보장하기 위해 지역 문자열 변수 사용
	pDriverName = (PCHAR) ALLOC((DWORD) cbDriverName + 1);
	if (NULL == pDriverName)
	{
		status = FALSE;
		goto Done;
	}
	StringCbCopyN(pDriverName, cbDriverName + 1, DriverName, cbDriverName);

	// 만약 주어진 DriverName과 일치하는 어떤 장치가 있다면 
	// 모든 현재 연결된 보이는 장치들을 설명한다.
	//
	deviceInfo = SetupDiGetClassDevs(NULL,
		NULL,
		NULL,
		DIGCF_ALLCLASSES | DIGCF_PRESENT);

	if (deviceInfo == INVALID_HANDLE_VALUE)
	{
		status = FALSE;
		goto Done;
	}

	deviceIndex = 0;
	deviceInfoData.cbSize = sizeof(deviceInfoData);

	while (done == FALSE)
	{
		//
		// 다음 장치의 devinst를 얻음
		//

		status = SetupDiEnumDeviceInfo(deviceInfo,
			deviceIndex,
			&deviceInfoData);

		deviceIndex++;

		if (!status)
		{
			//
			// 이것은 에러 거나 모든 장치들이 처리되고 있음을 안내 하는 것일 수 있다.
			// 또는 원하는 장치를 찾지 못했다는 것일 수 잇다.
			//

			done = TRUE;
			break;
		}

		//
		// DriverName 값을 얻음.
		//

		bResult = GetDeviceProperty(deviceInfo,
			&deviceInfoData,
			SPDRP_DRIVER,
			&buf);

		// DriverName 과 값이 일치하는 경우 DeviceInstance 를  return.
		//
		if (bResult == TRUE && buf != NULL && _stricmp(pDriverName, buf) == 0)
		{
			done = TRUE;
			*pDevInfo = deviceInfo;
			CopyMemory(pDevInfoData, &deviceInfoData, sizeof(deviceInfoData));
			FREE(buf);
			break;
		}

		if(buf != NULL)
		{
			FREE(buf);
			buf = NULL;
		}
	}

Done:

	if (bResult == FALSE)
	{
		if (deviceInfo != INVALID_HANDLE_VALUE)
		{
			SetupDiDestroyDeviceInfoList(deviceInfo);
		}
	}

	if (pDriverName != NULL)
	{
		FREE(pDriverName);
	}

	return status;
}

/*****************************************************************************

FreeDeviceProperties()

device properties 구조체를 free.

*****************************************************************************/
VOID FreeDeviceProperties(_In_ PUSB_DEVICE_PNP_STRINGS *ppDevProps)
{
	if(ppDevProps == NULL)
	{
		return;
	}

	if(*ppDevProps == NULL)
	{
		return;
	}

	if ((*ppDevProps)->DeviceId != NULL)
	{
		FREE((*ppDevProps)->DeviceId);
	}

	if ((*ppDevProps)->DeviceDesc != NULL)
	{
		FREE((*ppDevProps)->DeviceDesc);
	}

	//
	// 다음 오는 것은 필요치 않다. but left in case
	// 하지만 공간이 할당된 남은 포이터 필드들이 
	// 남는 경우 나중에 실패가 뜰것이다. 
	//

	if ((*ppDevProps)->HwId != NULL)
	{
		FREE((*ppDevProps)->HwId);
	}

	if ((*ppDevProps)->Service != NULL)
	{
		FREE((*ppDevProps)->Service);
	}

	if ((*ppDevProps)->DeviceClass != NULL)
	{
		FREE((*ppDevProps)->DeviceClass);
	}

	if ((*ppDevProps)->PowerState != NULL)
	{
		FREE((*ppDevProps)->PowerState);
	}

	FREE(*ppDevProps);
	*ppDevProps = NULL;
}

//*****************************************************************************
//
// GetHostControllerPowerMap()
//
// HANDLE hHCDev
//      - USB Host Controller 를 다루기 위한 handle.
//
// PUSBHOSTCONTROLLERINFO hcInfo
//      - Power Map Info를 받기 위한 데이터 구조체
//
// return DWORD dwError
//      - return ERROR_SUCCESS 또는 last error
//
//*****************************************************************************

DWORD 
	GetHostControllerPowerMap(
	HANDLE hHCDev, 
	PUSBHOSTCONTROLLERINFO hcInfo)
{
	USBUSER_POWER_INFO_REQUEST UsbPowerInfoRequest;
	PUSB_POWER_INFO            pUPI = &UsbPowerInfoRequest.PowerInformation ;
	DWORD                      dwError = 0;
	DWORD                      dwBytes = 0;
	BOOL                       bSuccess = FALSE;
	int                        nIndex = 0;
	int                        nPowerState = WdmUsbPowerSystemWorking;

	for ( ; nPowerState <= WdmUsbPowerSystemShutdown; nIndex++, nPowerState++) 
	{
		// 0으로 초기화
		memset(&UsbPowerInfoRequest, 0, sizeof(UsbPowerInfoRequest));

		// set the header and request sizes
		UsbPowerInfoRequest.Header.UsbUserRequest = USBUSER_GET_POWER_STATE_MAP;
		UsbPowerInfoRequest.Header.RequestBufferLength = sizeof(UsbPowerInfoRequest);
		UsbPowerInfoRequest.PowerInformation.SystemState = (WDMUSB_POWER_STATE)nPowerState;

		//
		// 현재 hub를 위한 USB_POWER_INFO 구조체를 위한 USBHUB를 알아낸다.
		//
		bSuccess = DeviceIoControl(hHCDev,
			IOCTL_USB_USER_REQUEST,
			&UsbPowerInfoRequest,
			sizeof(UsbPowerInfoRequest),
			&UsbPowerInfoRequest,
			sizeof(UsbPowerInfoRequest),
			&dwBytes,
			NULL);

		if (!bSuccess)
		{
			dwError = GetLastError();
			OOPS();
		}
		else
		{
			// 컴퓨터의 USB Host Controller 의 info structure 를 data에 copy
			memcpy( &(hcInfo->USBPowerInfo[nIndex]), pUPI, sizeof(USB_POWER_INFO));
		}
	}

	return dwError;
}

//*****************************************************************************
//
// GetHostControllerInfo()
//
// HANDLE hHCDev
//      - USB Host Controller 를 다루기 위한 handle.
//
// PUSBHOSTCONTROLLERINFO hcInfo
//      - Power Map Info를 받기 위한 데이터 구조체
//
// return DWORD dwError
//      - return ERROR_SUCCESS 또는 last error
//
//*****************************************************************************

DWORD 
	GetHostControllerInfo(
	HANDLE hHCDev, 
	PUSBHOSTCONTROLLERINFO hcInfo)
{
	USBUSER_CONTROLLER_INFO_0 UsbControllerInfo;
	DWORD                      dwError = 0;
	DWORD                      dwBytes = 0;
	BOOL                       bSuccess = FALSE;

	memset(&UsbControllerInfo, 0, sizeof(UsbControllerInfo));

	// header 그리고 요청하는 크기들을 결정
	UsbControllerInfo.Header.UsbUserRequest = USBUSER_GET_CONTROLLER_INFO_0;
	UsbControllerInfo.Header.RequestBufferLength = sizeof(UsbControllerInfo);

	//
	// USB_CONTROLLER_INFO_0 structure 를 얻음
	//
	bSuccess = DeviceIoControl(hHCDev,
		IOCTL_USB_USER_REQUEST,
		&UsbControllerInfo,
		sizeof(UsbControllerInfo),
		&UsbControllerInfo,
		sizeof(UsbControllerInfo),
		&dwBytes,
		NULL);

	if (!bSuccess)
	{
		dwError = GetLastError();
		OOPS();
	}
	else
	{
		hcInfo->ControllerInfo = (PUSB_CONTROLLER_INFO_0) ALLOC(sizeof(USB_CONTROLLER_INFO_0));
		if(NULL == hcInfo->ControllerInfo)
		{
			dwError = GetLastError();
			OOPS();
		}
		else
		{
			// 컴퓨터의 USB Host Controller 의 info structure 를 data에 copy
			memcpy(hcInfo->ControllerInfo, &UsbControllerInfo.Info0, sizeof(USB_CONTROLLER_INFO_0));
		}
	}
	return dwError;
}

//*****************************************************************************
//
// GetRootHubName()
//
//*****************************************************************************

PCHAR GetRootHubName (
	HANDLE HostController
	)
{
	BOOL                success = 0;
	ULONG               nBytes = 0;
	USB_ROOT_HUB_NAME   rootHubName;
	PUSB_ROOT_HUB_NAME  rootHubNameW = NULL;
	PCHAR               rootHubNameA = NULL;

	// Host Controller 가 붙어 있는 Root Hub의 이름의 길이를 얻음
	//
	success = DeviceIoControl(HostController,
		IOCTL_USB_GET_ROOT_HUB_NAME,
		0,
		0,
		&rootHubName,
		sizeof(rootHubName),
		&nBytes,
		NULL);

	if (!success)
	{
		OOPS();
		goto GetRootHubNameError;
	}

	// Root Hub name를 위한 공간 할당 
	//
	nBytes = rootHubName.ActualLength;

	rootHubNameW = (PUSB_ROOT_HUB_NAME)ALLOC(nBytes);
	if (rootHubNameW == NULL)
	{
		OOPS();
		goto GetRootHubNameError;
	}

	// Host Controller가 붙어 있는 Root Hub의 이름을 얻음
	//
	success = DeviceIoControl(HostController,
		IOCTL_USB_GET_ROOT_HUB_NAME,
		NULL,
		0,
		rootHubNameW,
		nBytes,
		&nBytes,
		NULL);
	if (!success)
	{
		OOPS();
		goto GetRootHubNameError;
	}

	// Root Hub의 이름을 변환
	//
	rootHubNameA = WideStrToMultiStr(rootHubNameW->RootHubName, nBytes);

	// 모든 것이 끝나고, 변환된 Root Hub의 이름을 반환하고 변환되지 않은 것은 free.
	//
	FREE(rootHubNameW);

	return rootHubNameA;

GetRootHubNameError:
	// There was an error, free anything that was allocated
	// 
	if (rootHubNameW != NULL)
	{
		FREE(rootHubNameW);
		rootHubNameW = NULL;
	}
	return NULL;
}

//*****************************************************************************
//
// EnumerateHub()
//
// HubName - 현재 Hub의 이름. 이 포인터는 유지 되어야 하므로 호출자가 이 메모리를
// free 하거나 재사용 할 수 없다.
//
// ConnectionInfo - NULL if this is a root hub, else this is the connection
// info for an external hub.  This pointer is kept so the caller can neither
// free nor reuse this memory.
//
// ConfigDesc - NULL if this is a root hub, else this is the Configuration
// Descriptor for an external hub.  This pointer is kept so the caller can
// neither free nor reuse this memory.
//
// StringDescs - NULL if this is a root hub.
//
// DevProps - Device properties of the hub
//
//*****************************************************************************

VOID
	EnumerateHub (
	_In_reads_(cbHubName) PCHAR                     HubName,
	_In_ size_t                                     cbHubName,
	_In_opt_ PUSB_NODE_CONNECTION_INFORMATION_EX    ConnectionInfo,
	_In_opt_ PUSB_NODE_CONNECTION_INFORMATION_EX_V2 ConnectionInfoV2,
	_In_opt_ PUSB_PORT_CONNECTOR_PROPERTIES         PortConnectorProps,
	_In_opt_ PUSB_DESCRIPTOR_REQUEST                ConfigDesc,
	_In_opt_ PUSB_DESCRIPTOR_REQUEST                BosDesc,
	_In_opt_ PSTRING_DESCRIPTOR_NODE                StringDescs,
	_In_opt_ PUSB_DEVICE_PNP_STRINGS                DevProps
	)
{
	// Initialize locals to not allocated state so the error cleanup routine
	// only tries to cleanup things that were successfully allocated.
	//
	PUSB_NODE_INFORMATION    hubInfo = NULL;
	PUSB_HUB_INFORMATION_EX  hubInfoEx = NULL;
	PUSB_HUB_CAPABILITIES_EX hubCapabilityEx = NULL;
	HANDLE                  hHubDevice = INVALID_HANDLE_VALUE;
	PVOID                   info = NULL;
	PCHAR                   deviceName = NULL;
	ULONG                   nBytes = 0;
	BOOL                    success = 0;
	DWORD                   dwSizeOfLeafName = 0;
	CHAR                    leafName[512] = {0}; 
	HRESULT                 hr = S_OK;
	size_t                  cchHeader = 0;
	size_t                  cchFullHubName = 0;

	// Allocate some space for a USBDEVICEINFO structure to hold the
	// hub info, hub name, and connection info pointers.  GPTR zero
	// initializes the structure for us.
	//
	info = ALLOC(sizeof(USBEXTERNALHUBINFO));
	if (info == NULL)
	{
		OOPS();
		goto EnumerateHubError;
	}

	// Allocate some space for a USB_NODE_INFORMATION structure for this Hub
	//
	hubInfo = (PUSB_NODE_INFORMATION)ALLOC(sizeof(USB_NODE_INFORMATION));
	if (hubInfo == NULL)
	{
		OOPS();
		goto EnumerateHubError;
	}

	hubInfoEx = (PUSB_HUB_INFORMATION_EX)ALLOC(sizeof(USB_HUB_INFORMATION_EX));
	if (hubInfoEx == NULL)
	{
		OOPS();
		goto EnumerateHubError;
	}

	hubCapabilityEx = (PUSB_HUB_CAPABILITIES_EX)ALLOC(sizeof(USB_HUB_CAPABILITIES_EX));
	if(hubCapabilityEx == NULL)
	{
		OOPS();
		goto EnumerateHubError;
	}

	// Keep copies of the Hub Name, Connection Info, and Configuration
	// Descriptor pointers
	//
	((PUSBROOTHUBINFO)info)->HubInfo   = hubInfo;
	((PUSBROOTHUBINFO)info)->HubName   = HubName;

	if (ConnectionInfo != NULL)
	{
		((PUSBEXTERNALHUBINFO)info)->DeviceInfoType = ExternalHubInfo;
		((PUSBEXTERNALHUBINFO)info)->ConnectionInfo = ConnectionInfo;
		((PUSBEXTERNALHUBINFO)info)->ConfigDesc = ConfigDesc;
		((PUSBEXTERNALHUBINFO)info)->StringDescs = StringDescs;
		((PUSBEXTERNALHUBINFO)info)->PortConnectorProps = PortConnectorProps;
		((PUSBEXTERNALHUBINFO)info)->HubInfoEx = hubInfoEx;
		((PUSBEXTERNALHUBINFO)info)->HubCapabilityEx = hubCapabilityEx;
		((PUSBEXTERNALHUBINFO)info)->BosDesc = BosDesc;
		((PUSBEXTERNALHUBINFO)info)->ConnectionInfoV2 = ConnectionInfoV2;
		((PUSBEXTERNALHUBINFO)info)->UsbDeviceProperties = DevProps;
	}
	else
	{
		((PUSBROOTHUBINFO)info)->DeviceInfoType = RootHubInfo;
		((PUSBROOTHUBINFO)info)->HubInfoEx = hubInfoEx;
		((PUSBROOTHUBINFO)info)->HubCapabilityEx = hubCapabilityEx;
		((PUSBROOTHUBINFO)info)->PortConnectorProps = PortConnectorProps;
		((PUSBROOTHUBINFO)info)->UsbDeviceProperties = DevProps;
	}

	// Allocate a temp buffer for the full hub device name.
	//
	hr = StringCbLength("\\\\.\\", MAX_DEVICE_PROP, &cchHeader);
	if (FAILED(hr))
	{
		goto EnumerateHubError;
	}
	cchFullHubName = cchHeader + cbHubName + 1;
	deviceName = (PCHAR)ALLOC((DWORD) cchFullHubName);
	if (deviceName == NULL)
	{
		OOPS();
		goto EnumerateHubError;
	}

	// Create the full hub device name
	//
	hr = StringCchCopyN(deviceName, cchFullHubName, "\\\\.\\", cchHeader);
	if (FAILED(hr))
	{
		goto EnumerateHubError;
	}
	hr = StringCchCatN(deviceName, cchFullHubName, HubName, cbHubName);
	if (FAILED(hr))
	{
		goto EnumerateHubError;
	}

	// Try to hub the open device
	//
	hHubDevice = CreateFile(deviceName,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	// Done with temp buffer for full hub device name
	//
	FREE(deviceName);

	if (hHubDevice == INVALID_HANDLE_VALUE)
	{
		OOPS();
		goto EnumerateHubError;
	}

	//
	// Now query USBHUB for the USB_NODE_INFORMATION structure for this hub.
	// This will tell us the number of downstream ports to enumerate, among
	// other things.
	//
	success = DeviceIoControl(hHubDevice,
		IOCTL_USB_GET_NODE_INFORMATION,
		hubInfo,
		sizeof(USB_NODE_INFORMATION),
		hubInfo,
		sizeof(USB_NODE_INFORMATION),
		&nBytes,
		NULL);

	if (!success)
	{
		OOPS();
		goto EnumerateHubError;
	}

	success = DeviceIoControl(hHubDevice,
		IOCTL_USB_GET_HUB_INFORMATION_EX,
		hubInfoEx,
		sizeof(USB_HUB_INFORMATION_EX),
		hubInfoEx,
		sizeof(USB_HUB_INFORMATION_EX),
		&nBytes,
		NULL);

	//
	// Fail gracefully for downlevel OS's from Win8
	//
	if (!success || nBytes < sizeof(USB_HUB_INFORMATION_EX))
	{
		FREE(hubInfoEx);
		hubInfoEx = NULL;
		if (ConnectionInfo != NULL)
		{
			((PUSBEXTERNALHUBINFO)info)->HubInfoEx = NULL;
		}
		else
		{
			((PUSBROOTHUBINFO)info)->HubInfoEx = NULL;
		}
	}

	//
	// Obtain Hub Capabilities
	//
	success = DeviceIoControl(hHubDevice,
		IOCTL_USB_GET_HUB_CAPABILITIES_EX,
		hubCapabilityEx,
		sizeof(USB_HUB_CAPABILITIES_EX),
		hubCapabilityEx,
		sizeof(USB_HUB_CAPABILITIES_EX),
		&nBytes,
		NULL);

	//
	// Fail gracefully
	//
	if (!success || nBytes < sizeof(USB_HUB_CAPABILITIES_EX))
	{
		FREE(hubCapabilityEx);
		hubCapabilityEx = NULL;
		if (ConnectionInfo != NULL)
		{
			((PUSBEXTERNALHUBINFO)info)->HubCapabilityEx = NULL;
		}
		else
		{
			((PUSBROOTHUBINFO)info)->HubCapabilityEx = NULL;
		}
	}

	// Build the leaf name from the port number and the device description
	//
	dwSizeOfLeafName = sizeof(leafName);
	if (ConnectionInfo)
	{
		StringCchPrintf(leafName, dwSizeOfLeafName, "[Port%d] ", ConnectionInfo->ConnectionIndex);
		StringCchCat(leafName, 
			dwSizeOfLeafName, 
			ConnectionStatuses[ConnectionInfo->ConnectionStatus]);
		StringCchCatN(leafName, 
			dwSizeOfLeafName, 
			" :  ",
			sizeof(" :  "));
	}

	if (DevProps)
	{
		size_t cbDeviceDesc = 0;
		hr = StringCbLength(DevProps->DeviceDesc, MAX_DRIVER_KEY_NAME, &cbDeviceDesc);
		if(SUCCEEDED(hr))
		{
			StringCchCatN(leafName, 
				dwSizeOfLeafName, 
				DevProps->DeviceDesc,
				cbDeviceDesc);
		}
	}
	else
	{
		if(ConnectionInfo != NULL)
		{
			// External hub
			StringCchCatN(leafName, 
				dwSizeOfLeafName, 
				HubName,
				cbHubName);
		}
		else
		{
			// Root hub
			StringCchCatN(leafName, 
				dwSizeOfLeafName, 
				"RootHub",
				sizeof("RootHub")); 
		}
	}
	/*
	// Now add an item to the TreeView with the PUSBDEVICEINFO pointer info
	// as the LPARAM reference value containing everything we know about the
	// hub.
	//

	if (hItem == NULL)
	{
	OOPS();
	goto EnumerateHubError;
	}
	*/
	// Now recursively enumerate the ports of this hub.
	//
	EnumerateHubPorts(
		hHubDevice,
		hubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts
		);


	CloseHandle(hHubDevice);
	return;

EnumerateHubError:
	//
	// Clean up any stuff that got allocated
	//

	if (hHubDevice != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hHubDevice);
		hHubDevice = INVALID_HANDLE_VALUE;
	}

	if (hubInfo)
	{
		FREE(hubInfo);
	}

	if (hubInfoEx)
	{
		FREE(hubInfoEx);
	}

	if (info)
	{
		FREE(info);
	}

	if (HubName)
	{
		FREE(HubName);
	}

	if (ConnectionInfo)
	{
		FREE(ConnectionInfo);
	}

	if (ConfigDesc)
	{
		FREE(ConfigDesc);
	}

	if (BosDesc)
	{
		FREE(BosDesc);
	}

	if (StringDescs != NULL)
	{
		PSTRING_DESCRIPTOR_NODE Next;

		do {

			Next = StringDescs->Next;
			FREE(StringDescs);
			StringDescs = Next;

		} while (StringDescs != NULL);
	}
}

//*****************************************************************************
//
// EnumerateHubPorts()
//
// hTreeParent - Handle of the TreeView item under which the hub port should
// be added.
//
// hHubDevice - Handle of the hub device to enumerate.
//
// NumPorts - Number of ports on the hub.
//
//*****************************************************************************

VOID
	EnumerateHubPorts (
	HANDLE      hHubDevice,
	ULONG       NumPorts
	)
{
	ULONG       index = 0;
	BOOL        success = 0;
	HRESULT     hr = S_OK;
	PCHAR       driverKeyName = NULL;
	PUSB_DEVICE_PNP_STRINGS DevProps;
	DWORD       dwSizeOfLeafName = 0;
	CHAR        leafName[512];
	int         icon = 0;

	PUSB_NODE_CONNECTION_INFORMATION_EX    connectionInfoEx;
	PUSB_PORT_CONNECTOR_PROPERTIES         pPortConnectorProps;
	USB_PORT_CONNECTOR_PROPERTIES          portConnectorProps;
	PUSB_DESCRIPTOR_REQUEST                configDesc;
	PUSB_DESCRIPTOR_REQUEST                bosDesc;
	PSTRING_DESCRIPTOR_NODE                stringDescs;
	PUSBDEVICEINFO                         info;
	PUSB_NODE_CONNECTION_INFORMATION_EX_V2 connectionInfoExV2;
	PDEVICE_INFO_NODE                      pNode;

	// Loop over all ports of the hub.
	//
	// Port indices are 1 based, not 0 based.
	//
	for (index = 1; index <= NumPorts; index++)
	{
		ULONG nBytesEx;
		ULONG nBytes = 0;

		connectionInfoEx = NULL;
		pPortConnectorProps = NULL;
		ZeroMemory(&portConnectorProps, sizeof(portConnectorProps));
		configDesc = NULL;
		bosDesc = NULL;
		stringDescs = NULL;
		info = NULL;
		connectionInfoExV2 = NULL;
		pNode = NULL;
		DevProps = NULL;
		ZeroMemory(leafName, sizeof(leafName));

		//
		// Allocate space to hold the connection info for this port.
		// For now, allocate it big enough to hold info for 30 pipes.
		//
		// Endpoint numbers are 0-15.  Endpoint number 0 is the standard
		// control endpoint which is not explicitly listed in the Configuration
		// Descriptor.  There can be an IN endpoint and an OUT endpoint at
		// endpoint numbers 1-15 so there can be a maximum of 30 endpoints
		// per device configuration.
		//
		// Should probably size this dynamically at some point.
		//

		nBytesEx = sizeof(USB_NODE_CONNECTION_INFORMATION_EX) +
			(sizeof(USB_PIPE_INFO) * 30);

		connectionInfoEx = (PUSB_NODE_CONNECTION_INFORMATION_EX)ALLOC(nBytesEx);

		if (connectionInfoEx == NULL)
		{
			OOPS();
			break;
		}

		connectionInfoExV2 = (PUSB_NODE_CONNECTION_INFORMATION_EX_V2) 
			ALLOC(sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2));

		if (connectionInfoExV2 == NULL)
		{
			OOPS();
			FREE(connectionInfoEx);
			break;
		}

		//
		// Now query USBHUB for the structures
		// for this port.  This will tell us if a device is attached to this
		// port, among other things.
		// The fault tolerate code is executed first.
		//

		portConnectorProps.ConnectionIndex = index;

		success = DeviceIoControl(hHubDevice,
			IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES,
			&portConnectorProps,
			sizeof(USB_PORT_CONNECTOR_PROPERTIES),
			&portConnectorProps,
			sizeof(USB_PORT_CONNECTOR_PROPERTIES),
			&nBytes,
			NULL);

		if (success && nBytes == sizeof(USB_PORT_CONNECTOR_PROPERTIES)) 
		{
			pPortConnectorProps = (PUSB_PORT_CONNECTOR_PROPERTIES)
				ALLOC(portConnectorProps.ActualLength);

			if (pPortConnectorProps != NULL)
			{
				pPortConnectorProps->ConnectionIndex = index;

				success = DeviceIoControl(hHubDevice,
					IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES,
					pPortConnectorProps,
					portConnectorProps.ActualLength,
					pPortConnectorProps,
					portConnectorProps.ActualLength,
					&nBytes,
					NULL);

				if (!success || nBytes < portConnectorProps.ActualLength)
				{
					FREE(pPortConnectorProps);
					pPortConnectorProps = NULL;
				}
			}
		}

		connectionInfoExV2->ConnectionIndex = index;
		connectionInfoExV2->Length = sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2);
		connectionInfoExV2->SupportedUsbProtocols.Usb300 = 1;

		success = DeviceIoControl(hHubDevice,
			IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX_V2,
			connectionInfoExV2,
			sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2),
			connectionInfoExV2,
			sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2),
			&nBytes,
			NULL);

		if (!success || nBytes < sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2)) 
		{
			FREE(connectionInfoExV2);
			connectionInfoExV2 = NULL;
		}

		connectionInfoEx->ConnectionIndex = index;

		success = DeviceIoControl(hHubDevice,
			IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX,
			connectionInfoEx,
			nBytesEx,
			connectionInfoEx,
			nBytesEx,
			&nBytesEx,
			NULL);

		if (success)
		{
			//
			// Since the USB_NODE_CONNECTION_INFORMATION_EX is used to display
			// the device speed, but the hub driver doesn't support indication
			// of superspeed, we overwrite the value if the super speed
			// data structures are available and indicate the device is operating
			// at SuperSpeed.
			// 

			if (connectionInfoEx->Speed == UsbHighSpeed 
				&& connectionInfoExV2 != NULL 
				&& connectionInfoExV2->Flags.DeviceIsOperatingAtSuperSpeedOrHigher)
			{
				connectionInfoEx->Speed = UsbSuperSpeed;
			}
		} 
		else 
		{
			PUSB_NODE_CONNECTION_INFORMATION    connectionInfo = NULL;

			// Try using IOCTL_USB_GET_NODE_CONNECTION_INFORMATION
			// instead of IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX
			//

			nBytes = sizeof(USB_NODE_CONNECTION_INFORMATION) +
				sizeof(USB_PIPE_INFO) * 30;

			connectionInfo = (PUSB_NODE_CONNECTION_INFORMATION)ALLOC(nBytes);

			if (connectionInfo == NULL) 
			{
				OOPS();

				FREE(connectionInfoEx);
				if (pPortConnectorProps != NULL)
				{
					FREE(pPortConnectorProps);
				}
				if (connectionInfoExV2 != NULL)
				{
					FREE(connectionInfoExV2);
				}
				continue;                
			}

			connectionInfo->ConnectionIndex = index;

			success = DeviceIoControl(hHubDevice,
				IOCTL_USB_GET_NODE_CONNECTION_INFORMATION,
				connectionInfo,
				nBytes,
				connectionInfo,
				nBytes,
				&nBytes,
				NULL);

			if (!success)
			{
				OOPS();

				FREE(connectionInfo);
				FREE(connectionInfoEx);
				if (pPortConnectorProps != NULL)
				{
					FREE(pPortConnectorProps);
				}
				if (connectionInfoExV2 != NULL)
				{
					FREE(connectionInfoExV2);
				}
				continue;
			}

			// Copy IOCTL_USB_GET_NODE_CONNECTION_INFORMATION into
			// IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX structure.
			//
			connectionInfoEx->ConnectionIndex = connectionInfo->ConnectionIndex;
			connectionInfoEx->DeviceDescriptor = connectionInfo->DeviceDescriptor;
			connectionInfoEx->CurrentConfigurationValue = connectionInfo->CurrentConfigurationValue;
			connectionInfoEx->Speed = connectionInfo->LowSpeed ? UsbLowSpeed : UsbFullSpeed;
			connectionInfoEx->DeviceIsHub = connectionInfo->DeviceIsHub;
			connectionInfoEx->DeviceAddress = connectionInfo->DeviceAddress;
			connectionInfoEx->NumberOfOpenPipes = connectionInfo->NumberOfOpenPipes;
			connectionInfoEx->ConnectionStatus = connectionInfo->ConnectionStatus;

			memcpy(&connectionInfoEx->PipeList[0],
				&connectionInfo->PipeList[0],
				sizeof(USB_PIPE_INFO) * 30);

			FREE(connectionInfo);
		}

		// Update the count of connected devices
		//
		if (connectionInfoEx->ConnectionStatus == DeviceConnected)
		{
			TotalDevicesConnected++;
		}

		if (connectionInfoEx->DeviceIsHub)
		{
			TotalHubs++;
		}

		// If there is a device connected, get the Device Description
		//
		if (connectionInfoEx->ConnectionStatus != NoDeviceConnected)
		{
			driverKeyName = GetDriverKeyName(hHubDevice, index);

			if (driverKeyName)
			{
				size_t cbDriverName = 0;

				hr = StringCbLength(driverKeyName, MAX_DRIVER_KEY_NAME, &cbDriverName);
				if (SUCCEEDED(hr))
				{
					DevProps = DriverNameToDeviceProperties(driverKeyName, cbDriverName);
					pNode = FindMatchingDeviceNodeForDriverName(driverKeyName, connectionInfoEx->DeviceIsHub);
				}
				FREE(driverKeyName);
			}

		}

		// If there is a device connected to the port, try to retrieve the
		// Configuration Descriptor from the device.
		//
		if (connectionInfoEx->ConnectionStatus == DeviceConnected)
		{
			configDesc = GetConfigDescriptor(hHubDevice,
				index,
				0);
		}
		else
		{
			configDesc = NULL;
		}

		if (configDesc != NULL &&
			connectionInfoEx->DeviceDescriptor.bcdUSB >= 0x0210)
		{
			bosDesc = GetBOSDescriptor(hHubDevice,
				index);
		}
		else
		{
			bosDesc = NULL;
		}

		if (configDesc != NULL &&
			AreThereStringDescriptors(&connectionInfoEx->DeviceDescriptor,
			(PUSB_CONFIGURATION_DESCRIPTOR)(configDesc+1)))
		{
			stringDescs = GetAllStringDescriptors (
				hHubDevice,
				index,
				&connectionInfoEx->DeviceDescriptor,
				(PUSB_CONFIGURATION_DESCRIPTOR)(configDesc+1));
		}
		else
		{
			stringDescs = NULL;
		}

		// If the device connected to the port is an external hub, get the
		// name of the external hub and recursively enumerate it.
		//
		if (connectionInfoEx->DeviceIsHub)
		{
			PCHAR extHubName;
			size_t cbHubName = 0;

			extHubName = GetExternalHubName(hHubDevice, index);
			if (extHubName != NULL)
			{
				hr = StringCbLength(extHubName, MAX_DRIVER_KEY_NAME, &cbHubName);
				if (SUCCEEDED(hr))
				{
					EnumerateHub(//hPortItem,
						extHubName,
						cbHubName,
						connectionInfoEx,
						connectionInfoExV2,
						pPortConnectorProps,
						configDesc,
						bosDesc,
						stringDescs,
						DevProps);
				}
			}
		}
		else
		{
			// Allocate some space for a USBDEVICEINFO structure to hold the
			// hub info, hub name, and connection info pointers.  GPTR zero
			// initializes the structure for us.
			//
			info = (PUSBDEVICEINFO) ALLOC(sizeof(USBDEVICEINFO));

			if (info == NULL)
			{
				OOPS();
				if (configDesc != NULL)
				{
					FREE(configDesc);
				}
				if (bosDesc != NULL)
				{
					FREE(bosDesc);
				}
				FREE(connectionInfoEx);

				if (pPortConnectorProps != NULL)
				{
					FREE(pPortConnectorProps);
				}
				if (connectionInfoExV2 != NULL)
				{
					FREE(connectionInfoExV2);
				}
				break;
			}

			info->DeviceInfoType = DeviceInfo;
			info->ConnectionInfo = connectionInfoEx;
			info->PortConnectorProps = pPortConnectorProps;
			info->ConfigDesc = configDesc;
			info->StringDescs = stringDescs;
			info->BosDesc = bosDesc;
			info->ConnectionInfoV2 = connectionInfoExV2;
			info->UsbDeviceProperties = DevProps;
			info->DeviceInfoNode = pNode;

			//결과값

			
			PUSB_DESCRIPTOR_REQUEST ConfigReqDesc = ((PUSBEXTERNALHUBINFO)info)->ConfigDesc;
			PUSB_COMMON_DESCRIPTOR  commonDesc = (PUSB_COMMON_DESCRIPTOR)(ConfigReqDesc + 1);
			PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)commonDesc;
			PUSB_INTERFACE_DESCRIPTOR   InterfaceDesc;

			if(ConfigReqDesc != NULL)
			{memcpy(&sendToDeviceDescData.DeviceDescriptor, &info->ConnectionInfo->DeviceDescriptor, sizeof(sendToDeviceDescData.DeviceDescriptor) );
			memcpy(&sendToDeviceDescData.ConfigDesc, ConfigDesc, ConfigDesc->bLength );

			commonDesc = (PUSB_COMMON_DESCRIPTOR)((PUCHAR)commonDesc + commonDesc->bLength);
			InterfaceDesc = (PUSB_INTERFACE_DESCRIPTOR)commonDesc;

			memcpy(&sendToDeviceDescData.InterfaceDesc, InterfaceDesc, sizeof(sendToDeviceDescData.InterfaceDesc) );

			memcpy(&sendToDeviceDescData.EndpointDescriptor[0], &((PUSB_PIPE_INFO)info->ConnectionInfo->PipeList)[0], sizeof(sendToDeviceDescData.EndpointDescriptor[0]) );
			memcpy(&sendToDeviceDescData.EndpointDescriptor[1], &((PUSB_PIPE_INFO)info->ConnectionInfo->PipeList)[1], sizeof(sendToDeviceDescData.EndpointDescriptor[0]) );

			strcpy_s(sendToDeviceDescData.DeviceId, info->UsbDeviceProperties->DeviceId);
			strcpy_s(sendToDeviceDescData.DeviceDesc, info->UsbDeviceProperties->DeviceDesc);
			strcpy_s(sendToDeviceDescData.HwId, info->UsbDeviceProperties->HwId);
			strcpy_s(sendToDeviceDescData.Service, info->UsbDeviceProperties->Service);
			strcpy_s(sendToDeviceDescData.DeviceClass, info->UsbDeviceProperties->DeviceClass);
			}

			// Add error description if ConnectionStatus is other than NoDeviceConnected / DeviceConnected
			StringCchCat(leafName, 

				sizeof(leafName), 
				ConnectionStatuses[connectionInfoEx->ConnectionStatus]);

			if (DevProps)
			{
				size_t cchDeviceDesc = 0;

				hr = StringCbLength(DevProps->DeviceDesc, MAX_DEVICE_PROP, &cchDeviceDesc);
				if (FAILED(hr))
				{
					OOPS();
				}
				dwSizeOfLeafName = sizeof(leafName);
				StringCchCatN(leafName, 
					dwSizeOfLeafName - 1, 
					" :  ",
					sizeof(" :  "));
				StringCchCatN(leafName, 
					dwSizeOfLeafName - 1, 
					DevProps->DeviceDesc,
					cchDeviceDesc );
			}

			if (connectionInfoEx->ConnectionStatus == NoDeviceConnected)
			{
				if (connectionInfoExV2 != NULL &&
					connectionInfoExV2->SupportedUsbProtocols.Usb300 == 1)
				{
					icon = NoSsDeviceIcon;
				}
				else
				{
					icon = NoDeviceIcon;
				}
			}
			else if (connectionInfoEx->CurrentConfigurationValue)
			{
				if (connectionInfoEx->Speed == UsbSuperSpeed)
				{
					icon = GoodSsDeviceIcon;
				}
				else
				{
					icon = GoodDeviceIcon;
				}
			}
			else
			{
				icon = BadDeviceIcon;
			}
		}
	} // for
}

//*****************************************************************************
//
// GetDriverKeyName()
//
//*****************************************************************************

PCHAR GetDriverKeyName (
	HANDLE  Hub,
	ULONG   ConnectionIndex
	)
{
	BOOL                                success = 0;
	ULONG                               nBytes = 0;
	USB_NODE_CONNECTION_DRIVERKEY_NAME  driverKeyName;
	PUSB_NODE_CONNECTION_DRIVERKEY_NAME driverKeyNameW = NULL;
	PCHAR                               driverKeyNameA = NULL;

	// Get the length of the name of the driver key of the device attached to
	// the specified port.
	//
	driverKeyName.ConnectionIndex = ConnectionIndex;

	success = DeviceIoControl(Hub,
		IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,
		&driverKeyName,
		sizeof(driverKeyName),
		&driverKeyName,
		sizeof(driverKeyName),
		&nBytes,
		NULL);

	if (!success)
	{
		OOPS();
		goto GetDriverKeyNameError;
	}

	// Allocate space to hold the driver key name
	//
	nBytes = driverKeyName.ActualLength;

	if (nBytes <= sizeof(driverKeyName))
	{
		OOPS();
		goto GetDriverKeyNameError;
	}

	driverKeyNameW = (PUSB_NODE_CONNECTION_DRIVERKEY_NAME)ALLOC(nBytes);
	if (driverKeyNameW == NULL)
	{
		OOPS();
		goto GetDriverKeyNameError;
	}

	// Get the name of the driver key of the device attached to
	// the specified port.
	//
	driverKeyNameW->ConnectionIndex = ConnectionIndex;

	success = DeviceIoControl(Hub,
		IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,
		driverKeyNameW,
		nBytes,
		driverKeyNameW,
		nBytes,
		&nBytes,
		NULL);

	if (!success)
	{
		OOPS();
		goto GetDriverKeyNameError;
	}

	// Driver Key name 의 type을 변환 ( WCHAR -> PCHAR )
	//
	driverKeyNameA = WideStrToMultiStr(driverKeyNameW->DriverKeyName, nBytes);

	// 메모리 해제
	//
	FREE(driverKeyNameW);

	return driverKeyNameA;


GetDriverKeyNameError:
	// There was an error, free anything that was allocated
	//
	if (driverKeyNameW != NULL)
	{
		FREE(driverKeyNameW);
		driverKeyNameW = NULL;
	}

	return NULL;
}

PDEVICE_INFO_NODE
	FindMatchingDeviceNodeForDriverName(
	_In_ PSTR   DriverKeyName,
	_In_ BOOLEAN IsHub
	)
{
	PDEVICE_INFO_NODE pNode  = NULL;
	PDEVICE_GUID_LIST pList  = NULL;
	PLIST_ENTRY       pEntry = NULL;

	pList = IsHub ? &gHubList : &gDeviceList;

	pEntry = pList->ListHead.Flink;

	while (pEntry != &pList->ListHead)
	{
		pNode = CONTAINING_RECORD(pEntry,
			DEVICE_INFO_NODE,
			ListEntry);
		if (_stricmp(DriverKeyName, pNode->DeviceDriverName) == 0)
		{
			return pNode;
		}

		pEntry = pEntry->Flink;
	}

	return NULL;
}

//*****************************************************************************
//
// GetConfigDescriptor()
//
// hHubDevice - Handle of the hub device containing the port from which the
// Configuration Descriptor will be requested.
//
// ConnectionIndex - Identifies the port on the hub to which a device is
// attached from which the Configuration Descriptor will be requested.
//
// DescriptorIndex - Configuration Descriptor index, zero based.
//
//*****************************************************************************

PUSB_DESCRIPTOR_REQUEST
	GetConfigDescriptor (
	HANDLE  hHubDevice,
	ULONG   ConnectionIndex,
	UCHAR   DescriptorIndex
	)
{
	BOOL    success = 0;
	ULONG   nBytes = 0;
	ULONG   nBytesReturned = 0;

	UCHAR   configDescReqBuf[sizeof(USB_DESCRIPTOR_REQUEST) +
		sizeof(USB_CONFIGURATION_DESCRIPTOR)];

	PUSB_DESCRIPTOR_REQUEST         configDescReq = NULL;
	PUSB_CONFIGURATION_DESCRIPTOR   configDesc = NULL;


	// Request the Configuration Descriptor the first time using our
	// local buffer, which is just big enough for the Cofiguration
	// Descriptor itself.
	//
	nBytes = sizeof(configDescReqBuf);

	configDescReq = (PUSB_DESCRIPTOR_REQUEST)configDescReqBuf;
	configDesc = (PUSB_CONFIGURATION_DESCRIPTOR)(configDescReq+1);

	// Zero fill the entire request structure
	//
	memset(configDescReq, 0, nBytes);

	// Indicate the port from which the descriptor will be requested
	//
	configDescReq->ConnectionIndex = ConnectionIndex;

	//
	// USBHUB uses URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE to process this
	// IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION request.
	//
	// USBD will automatically initialize these fields:
	//     bmRequest = 0x80
	//     bRequest  = 0x06
	//
	// We must inititialize these fields:
	//     wValue    = Descriptor Type (high) and Descriptor Index (low byte)
	//     wIndex    = Zero (or Language ID for String Descriptors)
	//     wLength   = Length of descriptor buffer
	//
	configDescReq->SetupPacket.wValue = (USB_CONFIGURATION_DESCRIPTOR_TYPE << 8)
		| DescriptorIndex;

	configDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

	// Now issue the get descriptor request.
	//
	success = DeviceIoControl(hHubDevice,
		IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
		configDescReq,
		nBytes,
		configDescReq,
		nBytes,
		&nBytesReturned,
		NULL);

	if (!success)
	{
		OOPS();
		return NULL;
	}

	if (nBytes != nBytesReturned)
	{
		OOPS();
		return NULL;
	}

	if (configDesc->wTotalLength < sizeof(USB_CONFIGURATION_DESCRIPTOR))
	{
		OOPS();
		return NULL;
	}

	// Now request the entire Configuration Descriptor using a dynamically
	// allocated buffer which is sized big enough to hold the entire descriptor
	//
	nBytes = sizeof(USB_DESCRIPTOR_REQUEST) + configDesc->wTotalLength;

	configDescReq = (PUSB_DESCRIPTOR_REQUEST)ALLOC(nBytes);

	if (configDescReq == NULL)
	{
		OOPS();
		return NULL;
	}

	configDesc = (PUSB_CONFIGURATION_DESCRIPTOR)(configDescReq+1);

	// Indicate the port from which the descriptor will be requested
	//
	configDescReq->ConnectionIndex = ConnectionIndex;

	//
	// USBHUB uses URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE to process this
	// IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION request.
	//
	// USBD will automatically initialize these fields:
	//     bmRequest = 0x80
	//     bRequest  = 0x06
	//
	// We must inititialize these fields:
	//     wValue    = Descriptor Type (high) and Descriptor Index (low byte)
	//     wIndex    = Zero (or Language ID for String Descriptors)
	//     wLength   = Length of descriptor buffer
	//
	configDescReq->SetupPacket.wValue = (USB_CONFIGURATION_DESCRIPTOR_TYPE << 8)
		| DescriptorIndex;

	configDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

	// Now issue the get descriptor request.
	//

	success = DeviceIoControl(hHubDevice,
		IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
		configDescReq,
		nBytes,
		configDescReq,
		nBytes,
		&nBytesReturned,
		NULL);

	if (!success)
	{
		OOPS();
		FREE(configDescReq);
		return NULL;
	}

	if (nBytes != nBytesReturned)
	{
		OOPS();
		FREE(configDescReq);
		return NULL;
	}

	if (configDesc->wTotalLength != (nBytes - sizeof(USB_DESCRIPTOR_REQUEST)))
	{
		OOPS();
		FREE(configDescReq);
		return NULL;
	}

	return configDescReq;
}

//*****************************************************************************
//
// GetBOSDescriptor()
//
// hHubDevice - Handle of the hub device containing the port from which the
// Configuration Descriptor will be requested.
//
// ConnectionIndex - Identifies the port on the hub to which a device is
// attached from which the BOS Descriptor will be requested.
//
//*****************************************************************************

PUSB_DESCRIPTOR_REQUEST
	GetBOSDescriptor (
	HANDLE  hHubDevice,
	ULONG   ConnectionIndex
	)
{
	BOOL    success = 0;
	ULONG   nBytes = 0;
	ULONG   nBytesReturned = 0;

	UCHAR   bosDescReqBuf[sizeof(USB_DESCRIPTOR_REQUEST) +
		sizeof(USB_BOS_DESCRIPTOR)];

	PUSB_DESCRIPTOR_REQUEST bosDescReq = NULL;
	PUSB_BOS_DESCRIPTOR     bosDesc = NULL;


	// Request the BOS Descriptor the first time using our
	// local buffer, which is just big enough for the BOS
	// Descriptor itself.
	//
	nBytes = sizeof(bosDescReqBuf);

	bosDescReq = (PUSB_DESCRIPTOR_REQUEST)bosDescReqBuf;
	bosDesc = (PUSB_BOS_DESCRIPTOR)(bosDescReq+1);

	// Zero fill the entire request structure
	//
	memset(bosDescReq, 0, nBytes);

	// Indicate the port from which the descriptor will be requested
	//
	bosDescReq->ConnectionIndex = ConnectionIndex;

	//
	// USBHUB uses URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE to process this
	// IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION request.
	//
	// USBD will automatically initialize these fields:
	//     bmRequest = 0x80
	//     bRequest  = 0x06
	//
	// We must inititialize these fields:
	//     wValue    = Descriptor Type (high) and Descriptor Index (low byte)
	//     wIndex    = Zero (or Language ID for String Descriptors)
	//     wLength   = Length of descriptor buffer
	//
	bosDescReq->SetupPacket.wValue = (USB_BOS_DESCRIPTOR_TYPE << 8);

	bosDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

	// Now issue the get descriptor request.
	//
	success = DeviceIoControl(hHubDevice,
		IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
		bosDescReq,
		nBytes,
		bosDescReq,
		nBytes,
		&nBytesReturned,
		NULL);

	if (!success)
	{
		OOPS();
		return NULL;
	}

	if (nBytes != nBytesReturned)
	{
		OOPS();
		return NULL;
	}

	if (bosDesc->wTotalLength < sizeof(USB_BOS_DESCRIPTOR))
	{
		OOPS();
		return NULL;
	}

	// Now request the entire BOS Descriptor using a dynamically
	// allocated buffer which is sized big enough to hold the entire descriptor
	//
	nBytes = sizeof(USB_DESCRIPTOR_REQUEST) + bosDesc->wTotalLength;

	bosDescReq = (PUSB_DESCRIPTOR_REQUEST)ALLOC(nBytes);

	if (bosDescReq == NULL)
	{
		OOPS();
		return NULL;
	}

	bosDesc = (PUSB_BOS_DESCRIPTOR)(bosDescReq+1);

	// Indicate the port from which the descriptor will be requested
	//
	bosDescReq->ConnectionIndex = ConnectionIndex;

	//
	// USBHUB uses URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE to process this
	// IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION request.
	//
	// USBD will automatically initialize these fields:
	//     bmRequest = 0x80
	//     bRequest  = 0x06
	//
	// We must inititialize these fields:
	//     wValue    = Descriptor Type (high) and Descriptor Index (low byte)
	//     wIndex    = Zero (or Language ID for String Descriptors)
	//     wLength   = Length of descriptor buffer
	//
	bosDescReq->SetupPacket.wValue = (USB_BOS_DESCRIPTOR_TYPE << 8);

	bosDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

	// Now issue the get descriptor request.
	//

	success = DeviceIoControl(hHubDevice,
		IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
		bosDescReq,
		nBytes,
		bosDescReq,
		nBytes,
		&nBytesReturned,
		NULL);

	if (!success)
	{
		OOPS();
		FREE(bosDescReq);
		return NULL;
	}

	if (nBytes != nBytesReturned)
	{
		OOPS();
		FREE(bosDescReq);
		return NULL;
	}

	if (bosDesc->wTotalLength != (nBytes - sizeof(USB_DESCRIPTOR_REQUEST)))
	{
		OOPS();
		FREE(bosDescReq);
		return NULL;
	}

	return bosDescReq;
}

//*****************************************************************************
//
// AreThereStringDescriptors()
//
// DeviceDesc - Device Descriptor for which String Descriptors should be
// checked.
//
// ConfigDesc - Configuration Descriptor (also containing Interface Descriptor)
// for which String Descriptors should be checked.
//
//*****************************************************************************

BOOL
	AreThereStringDescriptors (
	PUSB_DEVICE_DESCRIPTOR          DeviceDesc,
	PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc
	)
{
	PUCHAR                  descEnd = NULL;
	PUSB_COMMON_DESCRIPTOR  commonDesc = NULL;

	//
	// Check Device Descriptor strings
	//

	if (DeviceDesc->iManufacturer ||
		DeviceDesc->iProduct      ||
		DeviceDesc->iSerialNumber
		)
	{
		return TRUE;
	}


	//
	// Check the Configuration and Interface Descriptor strings
	//

	descEnd = (PUCHAR)ConfigDesc + ConfigDesc->wTotalLength;

	commonDesc = (PUSB_COMMON_DESCRIPTOR)ConfigDesc;

	while ((PUCHAR)commonDesc + sizeof(USB_COMMON_DESCRIPTOR) < descEnd &&
		(PUCHAR)commonDesc + commonDesc->bLength <= descEnd)
	{
		switch (commonDesc->bDescriptorType)
		{
		case USB_CONFIGURATION_DESCRIPTOR_TYPE:
		case USB_OTHER_SPEED_CONFIGURATION_DESCRIPTOR_TYPE:
			if (commonDesc->bLength != sizeof(USB_CONFIGURATION_DESCRIPTOR))
			{
				OOPS();
				break;
			}
			if (((PUSB_CONFIGURATION_DESCRIPTOR)commonDesc)->iConfiguration)
			{
				return TRUE;
			}
			commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
			continue;

		case USB_INTERFACE_DESCRIPTOR_TYPE:
			if (commonDesc->bLength != sizeof(USB_INTERFACE_DESCRIPTOR) &&
				commonDesc->bLength != sizeof(USB_INTERFACE_DESCRIPTOR2))
			{
				OOPS();
				break;
			}
			if (((PUSB_INTERFACE_DESCRIPTOR)commonDesc)->iInterface)
			{
				return TRUE;
			}
			commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
			continue;

		default:
			commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
			continue;
		}
		break;
	}

	return FALSE;
}

//*****************************************************************************
//
// GetAllStringDescriptors()
//
// hHubDevice - Handle of the hub device containing the port from which the
// String Descriptors will be requested.
//
// ConnectionIndex - Identifies the port on the hub to which a device is
// attached from which the String Descriptors will be requested.
//
// DeviceDesc - Device Descriptor for which String Descriptors should be
// requested.
//
// ConfigDesc - Configuration Descriptor (also containing Interface Descriptor)
// for which String Descriptors should be requested.
//
//*****************************************************************************

PSTRING_DESCRIPTOR_NODE
	GetAllStringDescriptors (
	HANDLE                          hHubDevice,
	ULONG                           ConnectionIndex,
	PUSB_DEVICE_DESCRIPTOR          DeviceDesc,
	PUSB_CONFIGURATION_DESCRIPTOR   ConfigDesc
	)
{
	PSTRING_DESCRIPTOR_NODE supportedLanguagesString = NULL;
	ULONG                   numLanguageIDs = 0;
	USHORT                  *languageIDs = NULL;

	PUCHAR                  descEnd = NULL;
	PUSB_COMMON_DESCRIPTOR  commonDesc = NULL;
	UCHAR                   uIndex = 1;
	UCHAR                   bInterfaceClass = 0;
	BOOL                    getMoreStrings = FALSE;
	HRESULT                 hr = S_OK;

	//
	// Get the array of supported Language IDs, which is returned
	// in String Descriptor 0
	//
	supportedLanguagesString = GetStringDescriptor(hHubDevice,
		ConnectionIndex,
		0,
		0);

	if (supportedLanguagesString == NULL)
	{
		return NULL;
	}

	numLanguageIDs = (supportedLanguagesString->StringDescriptor->bLength - 2) / 2;

	languageIDs = (USHORT *)(&supportedLanguagesString->StringDescriptor->bString[0]);

	//
	// Get the Device Descriptor strings
	//

	if (DeviceDesc->iManufacturer)
	{
		GetStringDescriptors(hHubDevice,
			ConnectionIndex,
			DeviceDesc->iManufacturer,
			numLanguageIDs,
			languageIDs,
			supportedLanguagesString);
	}

	if (DeviceDesc->iProduct)
	{
		GetStringDescriptors(hHubDevice,
			ConnectionIndex,
			DeviceDesc->iProduct,
			numLanguageIDs,
			languageIDs,
			supportedLanguagesString);
	}

	if (DeviceDesc->iSerialNumber)
	{
		GetStringDescriptors(hHubDevice,
			ConnectionIndex,
			DeviceDesc->iSerialNumber,
			numLanguageIDs,
			languageIDs,
			supportedLanguagesString);
	}

	//
	// Get the Configuration and Interface Descriptor strings
	//

	descEnd = (PUCHAR)ConfigDesc + ConfigDesc->wTotalLength;

	commonDesc = (PUSB_COMMON_DESCRIPTOR)ConfigDesc;

	while ((PUCHAR)commonDesc + sizeof(USB_COMMON_DESCRIPTOR) < descEnd &&
		(PUCHAR)commonDesc + commonDesc->bLength <= descEnd)
	{
		switch (commonDesc->bDescriptorType)
		{
		case USB_CONFIGURATION_DESCRIPTOR_TYPE:
			if (commonDesc->bLength != sizeof(USB_CONFIGURATION_DESCRIPTOR))
			{
				OOPS();
				break;
			}
			if (((PUSB_CONFIGURATION_DESCRIPTOR)commonDesc)->iConfiguration)
			{
				GetStringDescriptors(hHubDevice,
					ConnectionIndex,
					((PUSB_CONFIGURATION_DESCRIPTOR)commonDesc)->iConfiguration,
					numLanguageIDs,
					languageIDs,
					supportedLanguagesString);
			}
			commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
			continue;

		case USB_IAD_DESCRIPTOR_TYPE:
			if (commonDesc->bLength < sizeof(USB_IAD_DESCRIPTOR))
			{
				OOPS();
				break;
			}
			if (((PUSB_IAD_DESCRIPTOR)commonDesc)->iFunction)
			{
				GetStringDescriptors(hHubDevice,
					ConnectionIndex,
					((PUSB_IAD_DESCRIPTOR)commonDesc)->iFunction,
					numLanguageIDs,
					languageIDs,
					supportedLanguagesString);
			}
			commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
			continue;

		case USB_INTERFACE_DESCRIPTOR_TYPE:
			if (commonDesc->bLength != sizeof(USB_INTERFACE_DESCRIPTOR) &&
				commonDesc->bLength != sizeof(USB_INTERFACE_DESCRIPTOR2))
			{
				OOPS();
				break;
			}
			if (((PUSB_INTERFACE_DESCRIPTOR)commonDesc)->iInterface)
			{
				GetStringDescriptors(hHubDevice,
					ConnectionIndex,
					((PUSB_INTERFACE_DESCRIPTOR)commonDesc)->iInterface,
					numLanguageIDs,
					languageIDs,
					supportedLanguagesString);
			}

			//
			// We need to display more string descriptors for the following
			// interface classes
			//
			bInterfaceClass = ((PUSB_INTERFACE_DESCRIPTOR)commonDesc)->bInterfaceClass;
			if (bInterfaceClass == USB_DEVICE_CLASS_VIDEO)
			{
				getMoreStrings = TRUE;
			}
			commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
			continue;

		default:
			commonDesc = (PUSB_COMMON_DESCRIPTOR) ((PUCHAR) commonDesc + commonDesc->bLength);
			continue;
		}
		break;
	}

	if (getMoreStrings)
	{
		//
		// We might need to display strings later that are referenced only in
		// class-specific descriptors. Get String Descriptors 1 through 32 (an
		// arbitrary upper limit for Strings needed due to "bad devices"
		// returning an infinite repeat of Strings 0 through 4) until one is not
		// found.
		//
		// There are also "bad devices" that have issues even querying 1-32, but
		// historically USBView made this query, so the query should be safe for
		// video devices.
		//
		for (uIndex = 1; SUCCEEDED(hr) && (uIndex < NUM_STRING_DESC_TO_GET); uIndex++) 
		{
			hr = GetStringDescriptors(hHubDevice,
				ConnectionIndex,
				uIndex,
				numLanguageIDs,
				languageIDs,
				supportedLanguagesString);
		}
	}

	return supportedLanguagesString;
}

//*****************************************************************************
//
// GetExternalHubName()
//
//*****************************************************************************

PCHAR GetExternalHubName (
	HANDLE  Hub,
	ULONG   ConnectionIndex
	)
{
	BOOL                        success = 0;
	ULONG                       nBytes = 0;
	USB_NODE_CONNECTION_NAME    extHubName;
	PUSB_NODE_CONNECTION_NAME   extHubNameW = NULL;
	PCHAR                       extHubNameA = NULL;

	// Get the length of the name of the external hub attached to the
	// specified port.
	//
	extHubName.ConnectionIndex = ConnectionIndex;

	success = DeviceIoControl(Hub,
		IOCTL_USB_GET_NODE_CONNECTION_NAME,
		&extHubName,
		sizeof(extHubName),
		&extHubName,
		sizeof(extHubName),
		&nBytes,
		NULL);

	if (!success)
	{
		OOPS();
		goto GetExternalHubNameError;
	}

	// Allocate space to hold the external hub name
	//
	nBytes = extHubName.ActualLength;

	if (nBytes <= sizeof(extHubName))
	{
		OOPS();
		goto GetExternalHubNameError;
	}

	extHubNameW = (PUSB_NODE_CONNECTION_NAME)ALLOC(nBytes);

	if (extHubNameW == NULL)
	{
		OOPS();
		goto GetExternalHubNameError;
	}

	// Get the name of the external hub attached to the specified port
	//
	extHubNameW->ConnectionIndex = ConnectionIndex;

	success = DeviceIoControl(Hub,
		IOCTL_USB_GET_NODE_CONNECTION_NAME,
		extHubNameW,
		nBytes,
		extHubNameW,
		nBytes,
		&nBytes,
		NULL);

	if (!success)
	{
		OOPS();
		goto GetExternalHubNameError;
	}

	// Convert the External Hub name
	//
	extHubNameA = WideStrToMultiStr(extHubNameW->NodeName, nBytes);

	// All done, free the uncoverted external hub name and return the
	// converted external hub name
	//
	FREE(extHubNameW);

	return extHubNameA;


GetExternalHubNameError:
	// There was an error, free anything that was allocated
	//
	if (extHubNameW != NULL)
	{
		FREE(extHubNameW);
		extHubNameW = NULL;
	}

	return NULL;
}

//*****************************************************************************
//
// GetStringDescriptor()
//
// hHubDevice - Handle of the hub device containing the port from which the
// String Descriptor will be requested.
//
// ConnectionIndex - Identifies the port on the hub to which a device is
// attached from which the String Descriptor will be requested.
//
// DescriptorIndex - String Descriptor index.
//
// LanguageID - Language in which the string should be requested.
//
//*****************************************************************************

PSTRING_DESCRIPTOR_NODE
	GetStringDescriptor (
	HANDLE  hHubDevice,
	ULONG   ConnectionIndex,
	UCHAR   DescriptorIndex,
	USHORT  LanguageID
	)
{
	BOOL    success = 0;
	ULONG   nBytes = 0;
	ULONG   nBytesReturned = 0;

	UCHAR   stringDescReqBuf[sizeof(USB_DESCRIPTOR_REQUEST) +
		MAXIMUM_USB_STRING_LENGTH];

	PUSB_DESCRIPTOR_REQUEST stringDescReq = NULL;
	PUSB_STRING_DESCRIPTOR  stringDesc = NULL;
	PSTRING_DESCRIPTOR_NODE stringDescNode = NULL;

	nBytes = sizeof(stringDescReqBuf);

	stringDescReq = (PUSB_DESCRIPTOR_REQUEST)stringDescReqBuf;
	stringDesc = (PUSB_STRING_DESCRIPTOR)(stringDescReq+1);

	// Zero fill the entire request structure
	//
	memset(stringDescReq, 0, nBytes);

	// Indicate the port from which the descriptor will be requested
	//
	stringDescReq->ConnectionIndex = ConnectionIndex;

	//
	// USBHUB uses URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE to process this
	// IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION request.
	//
	// USBD will automatically initialize these fields:
	//     bmRequest = 0x80
	//     bRequest  = 0x06
	//
	// We must inititialize these fields:
	//     wValue    = Descriptor Type (high) and Descriptor Index (low byte)
	//     wIndex    = Zero (or Language ID for String Descriptors)
	//     wLength   = Length of descriptor buffer
	//
	stringDescReq->SetupPacket.wValue = (USB_STRING_DESCRIPTOR_TYPE << 8)
		| DescriptorIndex;

	stringDescReq->SetupPacket.wIndex = LanguageID;

	stringDescReq->SetupPacket.wLength = (USHORT)(nBytes - sizeof(USB_DESCRIPTOR_REQUEST));

	// Now issue the get descriptor request.
	//
	success = DeviceIoControl(hHubDevice,
		IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
		stringDescReq,
		nBytes,
		stringDescReq,
		nBytes,
		&nBytesReturned,
		NULL);

	//
	// Do some sanity checks on the return from the get descriptor request.
	//

	if (!success)
	{
		OOPS();
		return NULL;
	}

	if (nBytesReturned < 2)
	{
		OOPS();
		return NULL;
	}

	if (stringDesc->bDescriptorType != USB_STRING_DESCRIPTOR_TYPE)
	{
		OOPS();
		return NULL;
	}

	if (stringDesc->bLength != nBytesReturned - sizeof(USB_DESCRIPTOR_REQUEST))
	{
		OOPS();
		return NULL;
	}

	if (stringDesc->bLength % 2 != 0)
	{
		OOPS();
		return NULL;
	}

	//
	// Looks good, allocate some (zero filled) space for the string descriptor
	// node and copy the string descriptor to it.
	//

	stringDescNode = (PSTRING_DESCRIPTOR_NODE)ALLOC(sizeof(STRING_DESCRIPTOR_NODE) +
		stringDesc->bLength);

	if (stringDescNode == NULL)
	{
		OOPS();
		return NULL;
	}

	stringDescNode->DescriptorIndex = DescriptorIndex;
	stringDescNode->LanguageID = LanguageID;

	memcpy(stringDescNode->StringDescriptor,
		stringDesc,
		stringDesc->bLength);

	return stringDescNode;
}

//*****************************************************************************
//
// GetStringDescriptors()
//
// hHubDevice - Handle of the hub device containing the port from which the
// String Descriptor will be requested.GetStringDescriptors
//
// ConnectionIndex - Identifies the port on the hub to which a device is
// attached from which the String Descriptor will be requested.
//
// DescriptorIndex - String Descriptor index.
//
// NumLanguageIDs -  Number of languages in which the string should be
// requested.
//
// LanguageIDs - Languages in which the string should be requested.
//
// StringDescNodeHead - First node in linked list of device's string descriptors
//
// Return Value: HRESULT indicating whether the string is on the list
//
//*****************************************************************************

HRESULT
	GetStringDescriptors (
	_In_ HANDLE                         hHubDevice,
	_In_ ULONG                          ConnectionIndex,
	_In_ UCHAR                          DescriptorIndex,
	_In_ ULONG                          NumLanguageIDs,
	_In_reads_(NumLanguageIDs) USHORT  *LanguageIDs,
	_In_ PSTRING_DESCRIPTOR_NODE        StringDescNodeHead
	)
{
	PSTRING_DESCRIPTOR_NODE tail = NULL;
	PSTRING_DESCRIPTOR_NODE trailing = NULL;
	ULONG i = 0;

	//
	// Go to the end of the linked list, searching for the requested index to
	// see if we've already retrieved it
	//
	for (tail = StringDescNodeHead; tail != NULL; tail = tail->Next)
	{
		if (tail->DescriptorIndex == DescriptorIndex)
		{
			return S_OK;
		}

		trailing = tail;
	}

	tail = trailing;

	//
	// Get the next String Descriptor. If this is NULL, then we're done (return)
	// Otherwise, loop through all Language IDs
	//
	for (i = 0; (tail != NULL) && (i < NumLanguageIDs); i++)
	{
		tail->Next = GetStringDescriptor(hHubDevice,
			ConnectionIndex,
			DescriptorIndex,
			LanguageIDs[i]);

		tail = tail->Next;
	}

	if (tail == NULL)
	{
		return E_FAIL;
	} else {
		return S_OK;
	}
}

_Success_(return == TRUE)
	BOOL
	GetDeviceProperty(
	_In_    HDEVINFO         DeviceInfoSet,
	_In_    PSP_DEVINFO_DATA DeviceInfoData,
	_In_    DWORD            Property,
	_Outptr_  LPTSTR        *ppBuffer
	)
{
	BOOL bResult;
	DWORD requiredLength = 0;
	DWORD lastError;

	if (ppBuffer == NULL)
	{
		return FALSE;
	}

	*ppBuffer = NULL;

	bResult = SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
		DeviceInfoData,
		Property ,
		NULL,
		NULL,
		0,
		&requiredLength); // Plug and Play 디바이스 속성을 검색하여 requiredLength를 읽어온다.
	lastError = GetLastError();

	if ((requiredLength == 0) || (bResult != FALSE && lastError != ERROR_INSUFFICIENT_BUFFER))
	{
		return FALSE;
	}

	*ppBuffer = (LPTSTR)ALLOC(requiredLength);

	if (*ppBuffer == NULL)
	{
		return FALSE;
	}

	bResult = SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
		DeviceInfoData,
		Property ,
		NULL,
		(PBYTE) *ppBuffer,
		requiredLength,
		&requiredLength); // 다시 한번 호출해서 해당 디바이스의 속성의 실제 내용을 읽어온다.
	if(bResult == FALSE)
	{
		FREE(*ppBuffer);
		*ppBuffer = NULL;
		return FALSE;
	}

	return TRUE;
}