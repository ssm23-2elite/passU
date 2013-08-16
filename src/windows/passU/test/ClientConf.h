#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// ClientConf 대화 상자입니다.

class ClientConf : public CDialogEx
{
	DECLARE_DYNAMIC(ClientConf)

public:
	ClientConf(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ClientConf();
	CString m_address;
	CIPAddressCtrl m_ip;
	CString m_PortNum;
	CSocket clientSock;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnIpnFieldchangedIpaddress(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();

};
