// SimpleUDP.h : main header file for the SIMPLEUDP application
//

#if !defined(AFX_SIMPLEUDP_H__0A80DDA7_1ABE_40A7_8975_48D6992A04D8__INCLUDED_)
#define AFX_SIMPLEUDP_H__0A80DDA7_1ABE_40A7_8975_48D6992A04D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSimpleUDPApp:
// See SimpleUDP.cpp for the implementation of this class
//

class CSimpleUDPApp : public CWinApp
{
public:
	CSimpleUDPApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleUDPApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSimpleUDPApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEUDP_H__0A80DDA7_1ABE_40A7_8975_48D6992A04D8__INCLUDED_)
