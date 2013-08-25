// TestKeyHook.h : main header file for the TESTKEYHOOK application
//

#if !defined(AFX_TESTKEYHOOK_H__6F0BC24A_ED23_4603_9844_7D13E232D9E2__INCLUDED_)
#define AFX_TESTKEYHOOK_H__6F0BC24A_ED23_4603_9844_7D13E232D9E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestKeyHookApp:
// See TestKeyHook.cpp for the implementation of this class
//

class CTestKeyHookApp : public CWinApp
{
public:
	CTestKeyHookApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestKeyHookApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestKeyHookApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTKEYHOOK_H__6F0BC24A_ED23_4603_9844_7D13E232D9E2__INCLUDED_)
