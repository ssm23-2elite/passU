#include "afxwin.h"
#include "packet.h"
#if !defined(AFX_CLIENTSEND_H__A5AE0E84_00E5_4EFE_AEE6_87F0DE55BDC7__INCLUDED_)
#define AFX_CLIENTSEND_H__A5AE0E84_00E5_4EFE_AEE6_87F0DE55BDC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSend.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClientSend dialog

class CClientSend : public CDialog
{
// Construction
public:
	CClientSend(CWnd* pParent = NULL);   // standard constructor
	void OnMouseEvent(CPoint point);
	BOOL isConnected;
	MPACKET mpacket;
	KPACKET kpacket;
	LONG prev_x, prev_y;

// Dialog Data
	//{{AFX_DATA(CClientSend)
	enum { IDD = IDD_CLIENT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSend)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConnect();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	CButton m_connect;
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSEND_H__A5AE0E84_00E5_4EFE_AEE6_87F0DE55BDC7__INCLUDED_)
