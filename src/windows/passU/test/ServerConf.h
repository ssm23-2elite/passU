#pragma once

#include <afxsock.h>
#include "afxwin.h"

// ServerConf 대화 상자입니다.

class ServerConf : public CDialogEx
{
	DECLARE_DYNAMIC(ServerConf)

public:
	ServerConf(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ServerConf();
	bool m_bDragFlag; // 드래그 판별 플래그
	bool m_applyFlag; // apply 눌렀나 판별하는 플래그
	CPoint m_ptltemText; // 특정 아이템 텍스트의 좌표
	//CTrackWnd m_wndTrack;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CButton m_CButton_one;
	CButton m_CButton_two;
	CButton m_CButton_three;
	CButton m_CButton_four;
	CButton m_CButton_five;
	CButton m_CButton_six;
	CButton m_CButton_seven;
	CButton m_CButton_eight;
	CButton m_CButton_nine;
	afx_msg void OnBnClickedServerButton1();
	afx_msg void OnBnClickedServerButton2();
	afx_msg void OnBnClickedServerButton3();
	afx_msg void OnBnClickedServerButton4();
	afx_msg void OnBnClickedServerButton5();
	afx_msg void OnBnClickedServerButton6();
	afx_msg void OnBnClickedServerButton7();
	afx_msg void OnBnClickedServerButton8();
	afx_msg void OnBnClickedServerButton9();
	CString m_serverPortEdit;
	afx_msg void OnBnClickedPortApply();
	CButton m_CButton_portApply;
	afx_msg void OnBnClickedPortCancel();
	CButton m_CButton_portCancel;
	afx_msg void OnStnClickedPiccom();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CEdit m_portEditControl;
	CString serverIPAddress;
};
