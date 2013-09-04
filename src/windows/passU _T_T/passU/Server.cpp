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

	serverIPAddress.Append(strIpAddress);


	hinstDLL = LoadLibrary(_T("ServerHook.dll"));
	if(!hinstDLL)
		AfxMessageBox(_T("ServerHook.dll 로드 실패!"));

	installKeyhook = (InstallKeyboardHook)GetProcAddress(hinstDLL, "InstallKeyboardHook");
	installMousehook = (InstallMouseHook)GetProcAddress(hinstDLL, "InstallMouseHook");

	uninstallKeyhook = (UnInstallKeyboardHook)GetProcAddress(hinstDLL, "UnInstallKeyboardHook");
	uninstallMousehook = (UnInstallMouseHook)GetProcAddress(hinstDLL, "UnInstallMouseHook");


	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	for(int i = 0 ; i < 9 ; i ++){
		ZeroMemory(&btnControl[i], sizeof(btnControl[i]));
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
	m_waiting_client.SetImageList(&m_imgList, LVSIL_NORMAL);

	m_imgList.Add(&m_bmp_monitor, RGB(0, 0, 0));
	m_imgList.Add(&m_bmp_phone, RGB(0, 0, 0));

	/*LVITEM lvitem = {0};

	lvitem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	lvitem.iItem = 0;
	lvitem.pszText = "127.0.0.1";
	lvitem.lParam = (LPARAM)STATUS_PC;
	lvitem.iImage = STATUS_PC - 1;

	m_waiting_client.InsertItem(&lvitem);

	lvitem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	lvitem.iItem = 0;
	lvitem.pszText = "127.0.0.1";
	lvitem.lParam = (LPARAM)STATUS_MOBILE;
	lvitem.iImage = STATUS_MOBILE - 1;

	m_waiting_client.InsertItem(&lvitem);*/

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
	HBITMAP m_hBitmap;

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

	// 후킹 DLL API로부터 받아오는 구조체(포인터)
	HEVENT *hEVENT;
	MPACKET *mEVENT;

	CPassUChildSocket *s = NULL;

	// Main Dialog 포인터를 받아옴
	CPassUDlg * pMainDlg = (CPassUDlg *)::AfxGetMainWnd();
	POSITION pos = pMainDlg->m_pSockList.GetHeadPosition();

	TRACE("OnCopyData\n");


	switch(pCopyDataStruct->dwData){
	case KEYBOARD_DATA: // keyboard
		//hEVENT = (tagHEVENT *) pCopyDataStruct->lpData; // hEvent 구조체 연결(후킹된 자료)

		//if(hEVENT->lParam >= 0){ // 키가 눌렸을 때 
		//	//	TRACE("KEY CODE 도착, keyCode : %d\n", hEVENT->keyCode);

		//	keyP.deviceType = 1;
		//	keyP.msgType = 1;
		//	keyP.keyCode = hEVENT->keyCode;
		//	keyP.updownFlag = hEVENT->updown;

		//	// 1. 연결된 클라이언트 들 중에서 
		//	// 2. 지금 커서가 어디 있는지를 판별하고
		//	// 3. 그 있는 쪽의 버튼에 연결된 클라이언트한테 이 정보를 보냄.

		//	//example : 커서가 4에 있다고 가정
		//	m_status = 4;

		//	s = (CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos);

		//	/*while((btn_Bind[m_status-1] != s->c_id) && (btn_Bind[m_status - 1] != 0)){
		//	pMainDlg->m_pSockList.GetNext(pos);
		//	s = (CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos);
		//	}*/

		//	s->Send((LPCSTR *)&mouseP, sizeof(KPACKET));
		//}
		break;


	case MOUSE_DATA: // mouse
		//mEVENT = (MPACKET *)pCopyDataStruct->lpData; // mEvent 구조체 연결(후킹된 자료)
		////TRACE("MOUSE CODE 도착, x : %d y : %d\n wheelFlag : %d updownFlag : %d leftRight : %d\n", mEVENT->xCoord, mEVENT->yCoord,mEVENT->wheelFlag, mEVENT->updownFlag, mEVENT->leftRight);


		//mouseP.msgType = 2;
		//mouseP.deviceType = mEVENT->deviceType;
		//mouseP.leftRight = mEVENT->leftRight;
		//mouseP.wheelFlag = mEVENT->wheelFlag;
		//mouseP.updownFlag = mEVENT->updownFlag;
		//mouseP.xCoord = mEVENT->xCoord;
		//mouseP.yCoord = mEVENT->yCoord;

		//// 1. 연결된 클라이언트 들 중에서
		//// 2. 지금 커서가 어디 있는지를 판별하고
		//// 3. 그 있는 쪽의 버튼에 연결된 클라이언트한테 이 정보를 보냄.
		////example : 커서가 4에 있다고 가정
		//m_status = 4;
		//s = (CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos);
		///*	while((btn_Bind[m_status-1] != s->c_id) && (btn_Bind[m_status - 1] != 0)){
		//pMainDlg->m_pSockList.GetNext(pos);
		//s = (CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos);
		//}*/

		//s->Send((LPCSTR *)&mouseP, sizeof(MPACKET));
		break;


	case RECEIVE_DATA: // receiveData
		CPACKET* clientP = (CPACKET *) pCopyDataStruct->lpData; // 구조체 연결

		if(clientP->hello == 1){ // hello packet
			// 헬로 패킷이면 클라이언트 정보 설정
			// 버튼에 클라이언트 연결해주고, 첫번째 클라이언트면 후킹 시작하고
			// 클라이언트에 자신의 id 알려주고
			// 리스트 컨트롤에 아이콘 하나 추가
			//	TRACE("client\n");
			for(int i = 0 ; i < 9 ; i ++){
				if(clientInfo[i].clientID == 0){
					// client ID Setting
					clientInfo[i].setID(i + 1);

					clientP->c_id = ((CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos))->c_id = clientInfo[i].getID();
					
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

			if(pMainDlg->m_pSockList.GetCount() == 1){
				installKeyhook();
				installMousehook();
			}

			char buf[1024];
			ZeroMemory(buf, sizeof(buf));
			sprintf_s(buf, "%4d%4d%4d%1d%1d%4d%4d%4d%4d",
				MSG_CLIENT, clientP->c_id, clientP->pad3, clientP->hello,clientP->bye, clientP->ipFirst, clientP->ipSecond, clientP->ipThird, clientP->ipForth);
			((CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos))->Send(buf, strlen(buf));

		} else if(clientP->bye == 1){ // bye packet
			// 굿바이패킷이면
			// 버튼에 클라이언트 해제
			s =  ((CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos));
			while(clientP->c_id == s->c_id){
				((CPassUChildSocket *)pMainDlg->m_pSockList.GetNext(pos));
				s =  ((CPassUChildSocket *)pMainDlg->m_pSockList.GetAt(pos));
			}
			
			uninstallKeyhook();
			uninstallMousehook();
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
