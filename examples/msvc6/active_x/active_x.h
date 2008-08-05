#if !defined(AFX_ACTIVE_X_H__8DAB8271_D94F_4096_A735_D480F2602AF0__INCLUDED_)
#define AFX_ACTIVE_X_H__8DAB8271_D94F_4096_A735_D480F2602AF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// active_x.h : main header file for ACTIVE_X.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CActive_xApp : See active_x.cpp for implementation.

class CActive_xApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIVE_X_H__8DAB8271_D94F_4096_A735_D480F2602AF0__INCLUDED)
