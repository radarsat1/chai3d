//===========================================================================
/*
    This file is part of the CHAI 3D visualization and haptics libraries.
    Copyright (C) 2003-2009 by CHAI 3D. All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License("GPL") version 2
    as published by the Free Software Foundation.

    For using the CHAI 3D libraries with software that can not be combined
    with the GNU GPL, and for taking advantage of the additional benefits
    of our support services, please contact CHAI 3D about acquiring a
    Professional Edition License.

    \author:    <http://www.chai3d.org>
    \author:    Federico Barbagli
    \version    #CHAI_VERSION#
*/
//===========================================================================

// 06_board_tester.h : main header file for the 06_BOARD_TESTER application
//

#if !defined(AFX_06_BOARD_TESTER_H__EA460884_D8FD_4554_B57F_9E27F67B7C03__INCLUDED_)
#define AFX_06_BOARD_TESTER_H__EA460884_D8FD_4554_B57F_9E27F67B7C03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy06_board_testerApp:
// See 06_board_tester.cpp for the implementation of this class
//

class CMy06_board_testerApp : public CWinApp
{
public:
	CMy06_board_testerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy06_board_testerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMy06_board_testerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_06_BOARD_TESTER_H__EA460884_D8FD_4554_B57F_9E27F67B7C03__INCLUDED_)
