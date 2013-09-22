#pragma once
#include "PassUServerSocket.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "packet.h"

// CClient 대화 상자입니다.

class CClient : public CDialogEx
{
	DECLARE_DYNAMIC(CClient)

public:
	CClient(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CClient();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CLIENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void OnConnectServer(void);
	void OnDisconnect(void);
	CPassUServerSocket m_clientSock;
	CIPAddressCtrl m_IpAddressCtrl;
	afx_msg void OnBnClickedConnect();
	CButton m_cBtn_connect;
	afx_msg void OnBnClickedCancel();
	bool m_connectFlag;
	int client_ID;

	CString m_address;
	BYTE ipFirst, ipSecond, ipThird, ipForth;
	PACKET *p;
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	BOOL ExecuteProcess(CString FilePath, CString Parameter);
	BOOL OSVersionXPOver();
	CButton m_CBtn_Cancel;
};
