//===========================================================================
/*
    This file is part of the CHAI 3D visualization and haptics libraries.
    Copyright (C) 2003-2004 by CHAI 3D. All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License("GPL") version 2
    as published by the Free Software Foundation.

    For using the CHAI 3D libraries with software that can not be combined
    with the GNU GPL, and for taking advantage of the additional benefits
    of our support services, please contact CHAI 3D about acquiring a
    Professional Edition License.

    \author:    <http://www.chai3d.org>
    \author:    Chris Sewell
    \version    1.0
    \date       03/2005
*/
//===========================================================================


#if !defined(AFX_ACTIVE_XCTL_H__FDB105C0_4840_4D78_82CA_556C33490973__INCLUDED_)
#define AFX_ACTIVE_XCTL_H__FDB105C0_4840_4D78_82CA_556C33490973__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _MSVC // compiler complains if this is not defined

#include <gl/gl.h>
#include "CWorld.h"
#include "CViewport.h"
#include "CGenericObject.h"
#include "CLight.h"
#include "CPhantom3dofPointer.h"
#include "CMeta3dofPointer.h"
#include "CPrecisionTimer.h"

// Active_xCtl.h : Declaration of the CActive_xCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CActive_xCtrl : See Active_xCtl.cpp for implementation.

class CActive_xCtrl : public COleControl
{
	DECLARE_DYNCREATE(CActive_xCtrl)

// Constructor
public:
	CActive_xCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActive_xCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CActive_xCtrl();

	DECLARE_OLECREATE_EX(CActive_xCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CActive_xCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CActive_xCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CActive_xCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CActive_xCtrl)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CActive_xCtrl)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CActive_xCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CActive_xCtrl)
	//}}AFX_DISP_ID
	};

	// The interface to the haptic device
	cGeneric3dofPointer *tool;
	
	// The high-precision timer that's used (optionally) to run
	// the haptic loop
	//cPrecisionTimer timer;
	
	// A flag that indicates whether haptics/graphics are currently enabled
	int haptics_enabled;
	int graphics_enabled;
	
	// A flag that indicates whether the haptics thread is currently running
	// This flag does not get set when the haptic callback is driven from a 
	// multimedia timer 
	int haptics_thread_running;
	
	#define TOGGLE_HAPTICS_TOGGLE  -1
	#define TOGGLE_HAPTICS_DISABLE  0
	#define TOGGLE_HAPTICS_ENABLE   1

	// If the parameter is -1, haptics are toggled on/off
	// If it's 0 haptics are turned off
	// If it's 1 haptics are turned on
	void toggle_haptics(int enable = TOGGLE_HAPTICS_TOGGLE);
	
	// An object of some kind, to be rendered in the scene
	cMesh* object;
	cViewport* viewport;
	cCamera* camera;

	// virtual world
	cWorld* world;

	// Booleans indicating whether we're currently scrolling with
	// the left and/or right mouse buttons
	int m_left_scrolling_gl_area;
	int m_right_scrolling_gl_area;

	// The last point scrolled through by each mouse button,
	// in _viewport_ coordinates (i.e. (0,0) is the top-left
	// corner of the viewport area)
	CPoint last_left_scroll_point;
	CPoint last_right_scroll_point;
	cVector3d original_point;
  
	// The currently selected object (or zero when there's no selection)
	cGenericObject* selected_object;
	cVector3d obj_haptic_force;

	// Handles mouse-scroll events (moves or rotates the selected object)
	void scroll(CPoint p, int left_button = 1);

	// Handles mouse clicks (marks the front-most clicked object as 'selected')
	void select(CPoint p);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIVE_XCTL_H__FDB105C0_4840_4D78_82CA_556C33490973__INCLUDED)
