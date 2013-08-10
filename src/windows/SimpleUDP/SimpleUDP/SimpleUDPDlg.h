// SimpleUDPDlg.h : header file
//

#if !defined(AFX_SIMPLEUDPDLG_H__FEE90C08_9664_44F4_A9C4_6329A435F730__INCLUDED_)
#define AFX_SIMPLEUDPDLG_H__FEE90C08_9664_44F4_A9C4_6329A435F730__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSimpleUDPDlg dialog

class CSimpleUDPDlg : public CDialog
{
// Construction
public:
	CSimpleUDPDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSimpleUDPDlg)
	enum { IDD = IDD_SERVER };
	CEdit	m_edit;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleUDPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSimpleUDPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEUDPDLG_H__FEE90C08_9664_44F4_A9C4_6329A435F730__INCLUDED_)
