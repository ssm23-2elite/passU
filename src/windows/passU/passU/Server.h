#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "PassUServerSocket.h"
#include "PassUServerListen.h"
#include "PassULoactionInfo.h"
#include "packet.h"

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
	
	CPassULoactionInfo m_location_info[9];
	CPassUServerListen listen; // Listen, Accep하는 클래스 변수

	BOOL m_keyBoardHook;
	BOOL m_mouseHook;

	CListCtrl m_wating_client;
	CPassUServerThread *cThread;

	CObList * sockList;

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

	CImageList m_imgList;

	void OnStartServer();
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
	void ReceiveData(CPassUServerSocket * s);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	PACKET packMessage(int msgType, int sendDev, int recvDev, int deviceType, int relativeField, int updownFlag, int pad1, int keyCode, int pad2, int pad3);
};
