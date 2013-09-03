
// PassUDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "Server.h"
#include "Client.h"
#include "afxwin.h"
#include "PassUClientSocket.h"


// CPassUDlg 대화 상자
class CPassUDlg : public CDialogEx
{
	// 생성입니다.
public:
	CPassUDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_PASSU_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


	// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_Tab;

	CServer m_tab1;
	CClient m_tab2;
	CWnd *m_pwndShow;

	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	CClient m_tab_client;
	CServer m_tab_server;

	typedef struct tagHEVENT{
		int type;
		int keyCode;
		WPARAM data;
		LPARAM lParam;
	}HEVENT;

	CPassUClientSocket *m_pClient;
	CPassUServerSocket *m_pServer;
	CObList m_pSockList;
	BOOL m_SorC;
	void OnStartServer();
	void Accept(void);
	void ReceiveData(CPassUChildSocket * s);
	void CloseChild(CPassUChildSocket *s);
	void CleanUp(void);

	CButton m_CBtn_Start;
	CButton m_CBtn_Stop;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnDestroy();
	void OnConnectStart(void);
	void ClientCleanUp(void);
	void ReceiveClientData(CPassUClientSocket * s);
	void CloseClient(CPassUClientSocket * s);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
};
