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
    \date       05/2006
*/
//===========================================================================

#include "stdafx.h"
#include "implicit_functions.h"
#include "implicit_functionsDlg.h"
#include "implicit_functions_globals.h"

#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int right_aligned_elements[] = {
  IDC_STATIC1, IDC_CAMZOOM_SLIDER, IDC_TOGGLEHAPTICS_BUTTON
};
#define NUM_RIGHT_ALIGNED_ELEMENTS 3

static int bottom_aligned_elements[] = {
  IDC_STATIC
};
#define NUM_BOTTOM_ALIGNED_ELEMENTS 1

// Positions relative to the parent window left and _bottom_
CRect initial_ralign_positions[NUM_RIGHT_ALIGNED_ELEMENTS];
CRect initial_balign_positions[NUM_BOTTOM_ALIGNED_ELEMENTS];
CRect initial_dlg_rect;
CRect initial_dlg_window_rect;
CRect initial_gl_area_rect;
CRect initial_gl_wnd_rect;


Cimplicit_functionsDlg::Cimplicit_functionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cimplicit_functionsDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(Cimplicit_functionsDlg)
	//}}AFX_DATA_INIT
  initialized = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  
}

void Cimplicit_functionsDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cimplicit_functionsDlg)
	DDX_Control(pDX, IDC_CAMZOOM_SLIDER, m_camera_zoom_slider);
	DDX_Control(pDX, IDC_STIFFNESS, m_stiffness);
	DDX_Control(pDX, IDC_STATIC_FRICTION, m_static_friction);
	DDX_Control(pDX, IDC_DYNAMIC_FRICTION, m_dynamic_friction);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(Cimplicit_functionsDlg, CDialog)
	//{{AFX_MSG_MAP(Cimplicit_functionsDlg)
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
  ON_WM_HSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
  ON_BN_CLICKED(IDC_TOGGLEHAPTICS_BUTTON, OnToggleHapticsButton)
	//}}AFX_MSG_MAP
  ON_WM_SIZING()
  ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL Cimplicit_functionsDlg::OnInitDialog() {

  m_left_scrolling_gl_area = 0;
  m_right_scrolling_gl_area = 0;
  initialized = 0;

	CDialog::OnInitDialog();

  GetWindowRect(&initial_dlg_window_rect);
  GetClientRect(&initial_dlg_rect);

  int i;
  for(i=0; i<NUM_RIGHT_ALIGNED_ELEMENTS; i++) {
    CWnd* c = this->GetDlgItem(right_aligned_elements[i]);
    RECT r;
    c->GetWindowRect(&r);
    ScreenToClient(&r);
    initial_ralign_positions[i] = r;
  }

  for(i=0; i<NUM_BOTTOM_ALIGNED_ELEMENTS; i++) {
    CWnd* c = this->GetDlgItem(bottom_aligned_elements[i]);
    RECT r;
    c->GetWindowRect(&r);
    ScreenToClient(&r);
    initial_balign_positions[i] = r;
  }

  SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	
  CWnd* pWnd = GetDlgItem(IDC_GL_AREA);
  m_gl_area_hwnd = pWnd->m_hWnd;

  RECT r;
  pWnd->GetWindowRect(&r);
  ScreenToClient(&r);
  initial_gl_area_rect = r;

  // Map from 0 to 100 chai units
  m_camera_zoom_slider.SetRange(0,180,1);
  m_camera_zoom_slider.SetPos(45);

	m_stiffness.SetRange(0,100,1);
  m_stiffness.SetPos(30);

	m_static_friction.SetRange(0,100,1);
  m_static_friction.SetPos(60);

	m_dynamic_friction.SetRange(0,100,1);
  m_dynamic_friction.SetPos(40);
  
  initialized = 1;

  return TRUE;
}


HCURSOR Cimplicit_functionsDlg::OnQueryDragIcon() {
	return (HCURSOR) m_hIcon;
}


void Cimplicit_functionsDlg::OnClose() {
	
  g_main_app->uninitialize();

  FreeConsole();
  exit(0);

	CDialog::OnClose();
}


void Cimplicit_functionsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	
  if ((void*)(pScrollBar) == (void*)(&m_camera_zoom_slider)) {

    UpdateData(TRUE);
    int position = m_camera_zoom_slider.GetPos();
    g_main_app->zoom(position);
  }
	g_main_app->setCoefficients(m_stiffness.GetPos(), m_static_friction.GetPos(), m_dynamic_friction.GetPos());

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void Cimplicit_functionsDlg::OnMouseMove(UINT nFlags, CPoint point) {

	CDialog::OnMouseMove(nFlags, point);

  CWnd* pWnd = GetDlgItem(IDC_GL_AREA);
  RECT r;
  pWnd->GetWindowRect(&r);
  ScreenToClient(&r);

  // convert to viewport coordinates
  point.x -= r.left;
  point.y -= r.top;  

  if (m_left_scrolling_gl_area) {
    CPoint delta = point - last_left_scroll_point;    
    g_main_app->scroll(delta,1);
    last_left_scroll_point = point;
  }

  if (m_right_scrolling_gl_area) {
    CPoint delta = point - last_right_scroll_point;
    g_main_app->scroll(delta,0);
    last_right_scroll_point = point;
  }
}


void Cimplicit_functionsDlg::OnLButtonDown(UINT nFlags, CPoint point) {

  CDialog::OnLButtonDown(nFlags, point);

  ::SetCapture(m_hWnd);

  CWnd* pWnd = GetDlgItem(IDC_GL_AREA);
  RECT r;
  pWnd->GetWindowRect(&r);
  ScreenToClient(&r);

  if (PtInRect(&r,point)) {

    // convert to viewport coordinates
    point.x -= r.left;
    point.y -= r.top;      
  
    m_left_scrolling_gl_area = 1;
    last_left_scroll_point = point;
    g_main_app->select(point);
  }	
  
}


void Cimplicit_functionsDlg::OnLButtonUp(UINT nFlags, CPoint point) {
	
  ::ReleaseCapture();

  CDialog::OnLButtonUp(nFlags, point);  
  m_left_scrolling_gl_area = 0;

}


void Cimplicit_functionsDlg::OnRButtonDown(UINT nFlags, CPoint point) {

  CDialog::OnRButtonDown(nFlags, point);

  ::SetCapture(m_hWnd);

  CWnd* pWnd = GetDlgItem(IDC_GL_AREA);
  RECT r;
  pWnd->GetWindowRect(&r);
  ScreenToClient(&r);
  
  if (PtInRect(&r,point)) {

    // convert to viewport coordinates
    point.x -= r.left;
    point.y -= r.top;    

    m_right_scrolling_gl_area = 1;
    last_right_scroll_point = point;
    g_main_app->select(point);
  }	
  
}


void Cimplicit_functionsDlg::OnRButtonUp(UINT nFlags, CPoint point) {
	
  ::ReleaseCapture();

  CDialog::OnRButtonUp(nFlags, point);
  m_right_scrolling_gl_area = 0;

}


void Cimplicit_functionsDlg::OnToggleHapticsButton() {
	
  g_main_app->toggle_haptics();

  CButton* button = (CButton*)(GetDlgItem(IDC_TOGGLEHAPTICS_BUTTON));
  button->SetWindowText(g_main_app->haptics_enabled?"Disable haptics":"Enable haptics");
	
}


void Cimplicit_functionsDlg::OnSizing(UINT fwSide, LPRECT pRect)
{

  int neww = pRect->right - pRect->left;
  int initw = initial_dlg_window_rect.right - initial_dlg_window_rect.left;

  if (neww < initw) {
    if (fwSide == WMSZ_RIGHT || fwSide == WMSZ_TOPRIGHT || fwSide == WMSZ_BOTTOMRIGHT)
      pRect->right += initw - neww;
    else
      pRect->left -= initw - neww;
  }

  int newh = pRect->bottom - pRect->top;
  int inith = initial_dlg_window_rect.bottom - initial_dlg_window_rect.top;

  if (newh < inith) {
    if (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT || fwSide == WMSZ_BOTTOMRIGHT)
      pRect->bottom += inith - newh;
    else
      pRect->top -= inith - newh;
  }

  // Repaint
  Invalidate();
  UpdateWindow();
}


void Cimplicit_functionsDlg::OnSize(UINT nType, int cx, int cy)
{
  CDialog::OnSize(nType, cx, cy);

  if (initialized==0) return;

  RECT dlgr;
  GetClientRect(&dlgr);

  int i;

  for(i=0; i<NUM_RIGHT_ALIGNED_ELEMENTS; i++) {
    CWnd* c = GetDlgItem(right_aligned_elements[i]);
    int roffset = initial_dlg_rect.right - 
      initial_ralign_positions[i].left;

    RECT r;
    r.left = dlgr.right - roffset;
    r.top = initial_ralign_positions[i].top;
    // width and height
    r.right = initial_ralign_positions[i].right -
      initial_ralign_positions[i].left;
    r.bottom =       initial_ralign_positions[i].bottom -
      initial_ralign_positions[i].top;
    c->MoveWindow(r.left,r.top,r.right,r.bottom);
  }

  for(i=0; i<NUM_BOTTOM_ALIGNED_ELEMENTS; i++) {
    CWnd* c = GetDlgItem(bottom_aligned_elements[i]);
    int boffset = initial_dlg_rect.bottom - 
      initial_balign_positions[i].top;

    RECT r;
    r.left = initial_balign_positions[i].left;
    r.top = dlgr.bottom - boffset;

    // width and height
    r.right = initial_balign_positions[i].right -
      initial_balign_positions[i].left;
    r.bottom =       initial_balign_positions[i].bottom -
      initial_balign_positions[i].top;
    c->MoveWindow(r.left,r.top,r.right,r.bottom);
  }


  // Resize GL area
  RECT r = initial_gl_area_rect;
  r.bottom += dlgr.bottom - initial_dlg_rect.bottom;
  r.right += dlgr.right - initial_dlg_rect.right;

  CWnd* c = GetDlgItem(IDC_GL_AREA);
  c->MoveWindow(r.left,r.top,r.right - r.left, r.bottom - r.top, false);

  r = initial_gl_wnd_rect;
  r.bottom += dlgr.bottom - initial_dlg_rect.bottom;
  r.right += dlgr.right - initial_dlg_rect.right;

  //m_gl_wnd->MoveWindow(r.left,r.top,r.right - r.left, r.bottom - r.top, false);

  g_main_app->viewport->update(true);

  // Repaint
  Invalidate();
  UpdateWindow();
  RedrawWindow(0,0,RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}
