#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "PassUServerSocket.h"
#include "PassUChildSocket.h"
#include "packet.h"
#include "ClientInfo.h"

// CServer 대화 상자입니다.

class CServer : public CDialogEx
{
	DECLARE_DYNAMIC(CServer)

public:
	CServer(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CServer();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString serverIPAddress;
	HINSTANCE hinstDLL; // DLL 로딩
	HHOOK hHook; // HHOOK

	CButton m_cBtn[9];
	
	CClientInfo clientInfo[9]; // 각각의 index는 client ID


	BOOL m_keyBoardHook;
	BOOL m_mouseHook;
	BOOL m_bDrag;          

	int m_nOldTarget;   
	int m_nSource;      
	// for listctrl

	CImageList m_imgList[9];
	CListCtrl m_waiting_client;
	CImageList *m_pDragImage;
	CObList m_pSockList;

	//LBUTTONDOWN 했을 때 무슨 Device인지 플래그
	BOOL m_deviceFlag; // -1 : Nothing // 0 : Computer // 1 : Android
	// LBUtton UP 했을 때 버튼에 그림 그려주기
	WINDOWPLACEMENT getCoord[9];
	CWnd *btnControl[9];

	LVITEM lvitem[9]; // CimageList에 등록할 정보를 배열로 관리, index + 1 = ClientID

	static int STATUS_IDLE;
	static int STATUS_ON_FOR_SERVER;
	static int STATUS_ON_FOR_CLIENT;
	static int STATUS_OFF;

	int m_status;
	int m_x;
	int m_y;

	PACKET *p;
	KPACKET *k;
	MPACKET *m;
	CPACKET *c;

	// bmp load
	CBitmap m_bmp_monitor;
	CBitmap m_bmp_phone;
	
	void OnChangeLocationInfo(int index);
	void bindWatingClient(int client_index);
	void OnDisconnectedClient(int client_index);
	void OnArrivedScreenEdge(int position);
	CButton m_CBtn_ServerStart;
	afx_msg void OnBnClickedServerStart();
	afx_msg void OnBnClickedServerStop();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg LRESULT OnABC( WPARAM wParam, LPARAM lParam) ;
	PACKET packMessage(int msgType, int sendDev, int recvDev, int deviceType, int relativeField, int updownFlag, int pad1, int keyCode, int pad2, int pad3, int pad4);
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnBegindragList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	int GetHitIndex(CPoint);
	void MoveListItem(const INT _nSource, const INT _nDest);

};
