#pragma once
#include "afxwin.h"
#include "afxcmn.h"


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

	BOOL m_keyBoardHook;
	BOOL m_mouseHook;
	CButton m_cBtn_one;
	CButton m_cBtn_two;
	CButton m_cBtn_three;
	CButton m_cBtn_four;
	CButton m_cBtn_server;
	CButton m_cBtn_six;
	CButton m_cBtn_seven;
	CButton m_cBtn_eight;
	CButton m_cBtn_nine;
	CListCtrl m_cListCtrl;
};
