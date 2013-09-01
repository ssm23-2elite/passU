// TestKeyHookDlg.h : header file
//

#if !defined(AFX_TESTKEYHOOKDLG_H__BD14DE7A_C4DE_4446_8940_DEC1B42C2E1B__INCLUDED_)
#define AFX_TESTKEYHOOKDLG_H__BD14DE7A_C4DE_4446_8940_DEC1B42C2E1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestKeyHookDlg dialog

class CTestKeyHookDlg : public CDialog
{
public:
	// enum Member
	enum HookMode{JOURNAL, KEYBOARD};

// Construction
public:
	CTestKeyHookDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestKeyHookDlg)
	enum { IDD = IDD_TESTKEYHOOK_DIALOG };
	CEdit	m_ctrlStatus;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestKeyHookDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestKeyHookDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnJournal();
	afx_msg void OnKeyboard();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bJournalRecording;
	BOOL m_bKeyboardHooking;
	int m_nMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTKEYHOOKDLG_H__BD14DE7A_C4DE_4446_8940_DEC1B42C2E1B__INCLUDED_)
