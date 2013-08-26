
// testDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "USBIconDlg.h"

// CtestDlg 대화 상자
class CtestDlg : public CDialogEx
{
// 생성입니다.
public:
	CtestDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEST_DIALOG };

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
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedServerConfiguration();
	afx_msg void OnBnClickedClientConfiguration();
	afx_msg void OnBnClickedServerRadio();
	afx_msg void OnBnClickedClientRadio();
	afx_msg void OnBnClickedExitButton();
	CButton CButton_clientconfiguration;
	CButton CButton_ServerConfig;
	bool CRadioButton_Server;
	bool CRadioButton_Client;
	CUSBIconDlg *usbIcon;
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
