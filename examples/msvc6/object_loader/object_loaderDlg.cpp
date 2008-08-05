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
    \author:    Dan Morris
    \version    1.0
    \date       06/2004
*/
//===========================================================================

#include "stdafx.h"
#include "object_loader.h"
#include "object_loaderDlg.h"
#include "object_loader_globals.h"

#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Default haptic parameters for new objects
#define DEFAULT_STIFFNESS 50.0f
#define DEFAULT_STATIC_FRICTION 0.3f
#define DEFAULT_DYNAMIC_FRICTION 0.3f

#define DEFAULT_STEREO_SEPARATION 0.5f
#define DEFAULT_STEREO_FOCUS 4.0f

// This many slider scale units equals one friction unit
#define FRICTION_SLIDER_SCALE 100.0

#define SEPARATION_SLIDER_SCALE 50.0
#define FOCUS_SLIDER_SCALE 10.0

Cobject_loaderDlg::Cobject_loaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cobject_loaderDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(Cobject_loaderDlg)
	m_material_check = TRUE;
	m_showbox_check = FALSE;
	m_showframe_check = FALSE;
	m_shownormals_check = FALSE;
	m_usecolors_check = TRUE;
	m_usetexture_check = TRUE;
	m_usewireframe_check = FALSE;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  
}

void Cobject_loaderDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cobject_loaderDlg)
	DDX_Control(pDX, IDC_STEREOFOCUS_SLIDER, m_stereo_focus_slider);
	DDX_Control(pDX, IDC_STEREOSEP_SLIDER, m_stereo_separation_slider);
	DDX_Control(pDX, IDC_STEREOSEP_TEXT, m_stereo_separation_text);
	DDX_Control(pDX, IDC_STEREOFOCUS_TEXT, m_stereo_focus_text);
	DDX_Control(pDX, IDC_STIFFNESS_TEXT, m_stiffness_text);
	DDX_Control(pDX, IDC_STATICFRICTION_TEXT, m_static_friction_text);
	DDX_Control(pDX, IDC_DYNAMIC_FRICTION_RADIUS_TEXT, m_dynamic_friction_text);
	DDX_Control(pDX, IDC_STATICFRICTION_SLIDER, m_static_friction_slider);
	DDX_Control(pDX, IDC_STIFFNESS_SLIDER, m_stiffness_slider);
	DDX_Control(pDX, IDC_DYNAMIC_FRICTION_SLIDER, m_dynamic_friction_slider);
	DDX_Control(pDX, IDC_CAMZOOM_SLIDER, m_camera_zoom_slider);
	DDX_Check(pDX, IDC_CHECK_MATERIAL, m_material_check);
	DDX_Check(pDX, IDC_CHECK_SHOWBOX, m_showbox_check);
	DDX_Check(pDX, IDC_CHECK_SHOWFRAME, m_showframe_check);
	DDX_Check(pDX, IDC_CHECK_SHOWNORMALS, m_shownormals_check);
	DDX_Check(pDX, IDC_CHECK_USECOLORS, m_usecolors_check);
	DDX_Check(pDX, IDC_CHECK_USETEXTURE, m_usetexture_check);
	DDX_Check(pDX, IDC_CHECK_WIREFRAME, m_usewireframe_check);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(Cobject_loaderDlg, CDialog)
	//{{AFX_MSG_MAP(Cobject_loaderDlg)
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_CHECK_MATERIAL, OnCheck)
	ON_BN_CLICKED(IDC_LOAD_MODEL_BUTTON, OnLoadModelButton)
	ON_BN_CLICKED(IDC_LOAD_TEXTURE_BUTTON, OnLoadTextureButton)
	ON_WM_HSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_TOGGLEHAPTICS_BUTTON, OnToggleHapticsButton)
	ON_BN_CLICKED(IDC_TOGGLE_STEREO_BUTTON, OnToggleStereoButton)
	ON_WM_PAINT()
  ON_BN_CLICKED(IDC_CHECK_SHOWBOX, OnCheck)
  ON_BN_CLICKED(IDC_CHECK_SHOWFRAME, OnCheck)
  ON_BN_CLICKED(IDC_CHECK_SHOWNORMALS, OnCheck)
  ON_BN_CLICKED(IDC_CHECK_USECOLORS, OnCheck)
  ON_BN_CLICKED(IDC_CHECK_USETEXTURE, OnCheck)
  ON_BN_CLICKED(IDC_CHECK_WIREFRAME, OnCheck)
	ON_BN_CLICKED(IDC_ANIMATION_BUTTON, OnAnimationButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL Cobject_loaderDlg::OnInitDialog() {

  m_gl_wnd = 0;
  m_left_scrolling_gl_area = 0;
  m_right_scrolling_gl_area = 0;

	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	

  CWnd* pWnd = GetDlgItem(IDC_GL_AREA);  
  
  RECT child;
  pWnd->GetClientRect(&child);
  
  // Create our opengl window
  m_gl_wnd = new CWnd();
  m_gl_wnd->Create(NULL,
						       NULL,
						       WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE,
						       child,
						       pWnd,
						       0);
  m_gl_area_hwnd = m_gl_wnd->m_hWnd;
  
  // Map field of view from 0 to 180 chai units
  m_camera_zoom_slider.SetRange(0,180,1);
  m_camera_zoom_slider.SetPos(45);
  
  // Set up haptic property sliders
  m_static_friction_slider.SetRange(0,100,1);
  m_static_friction = DEFAULT_STATIC_FRICTION;
  m_static_friction_slider.SetPos(m_static_friction*FRICTION_SLIDER_SCALE);

  m_dynamic_friction_slider.SetRange(0,100,1);
  m_dynamic_friction = DEFAULT_DYNAMIC_FRICTION;
  m_dynamic_friction_slider.SetPos(m_dynamic_friction*FRICTION_SLIDER_SCALE);

  m_stiffness_slider.SetRange(0,100,1);
  m_stiffness = DEFAULT_STIFFNESS;
  m_stiffness_slider.SetPos(m_stiffness);

  m_stereo_separation_slider.SetRange(0,100,1);
  m_separation = DEFAULT_STEREO_SEPARATION;
  m_stereo_separation_slider.SetPos(m_separation*SEPARATION_SLIDER_SCALE);
  
  m_stereo_focus_slider.SetRange(0,100,1);
  m_focus = DEFAULT_STEREO_FOCUS;
  m_stereo_focus_slider.SetPos(m_focus*FOCUS_SLIDER_SCALE);

  update_slider_text();

  UpdateData(FALSE);
  
  return TRUE;
}


void Cobject_loaderDlg::update_slider_text() {

  char buf[100];

  sprintf(buf,"Stiffness: %3.3f",m_stiffness);
  m_stiffness_text.SetWindowText(buf);
  
  sprintf(buf,"Dynamic friction radius: %3.3f",m_dynamic_friction);
  m_dynamic_friction_text.SetWindowText(buf);

  sprintf(buf,"Static friction radius: %3.3f",m_static_friction);
  m_static_friction_text.SetWindowText(buf);

  sprintf(buf,"Stereo separation: %3.3f",m_separation);
  m_stereo_separation_text.SetWindowText(buf);

  sprintf(buf,"Stereo focal length: %3.3f",m_focus);
  m_stereo_focus_text.SetWindowText(buf);

  UpdateData(FALSE);

}


HCURSOR Cobject_loaderDlg::OnQueryDragIcon() {
	return (HCURSOR) m_hIcon;
}


void Cobject_loaderDlg::OnClose() {
	
  g_main_app->uninitialize();

  FreeConsole();
  exit(0);

	CDialog::OnClose();
}


void Cobject_loaderDlg::OnCheck() {
	UpdateData(TRUE);
  g_main_app->update_options_from_gui();
}

void Cobject_loaderDlg::OnLoadModelButton() {

  char filename[_MAX_PATH];

  int result = FileBrowse(filename, _MAX_PATH, 0, 0,
    "model files (*.obj, *.3ds)|*.obj;*.3ds|All Files (*.*)|*.*||",
    "Choose a model...");

  if (result < 0) {
    _cprintf("File browse canceled...\n");
    return;
  }

  g_main_app->LoadModel(filename);
}

void Cobject_loaderDlg::OnLoadTextureButton() {	

    char filename[_MAX_PATH];

  int result = FileBrowse(filename, _MAX_PATH, 0, 0,
    "image files (*.bmp, *.tga)|*.bmp;*.tga|All Files (*.*)|*.*||",
    "Choose an image file...");

  if (result < 0) {
    _cprintf("File browse canceled...\n");
    return;
  }

  g_main_app->LoadTexture(filename);

}


void Cobject_loaderDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	
  UpdateData(TRUE);
    
  if ((void*)(pScrollBar) == (void*)(&m_camera_zoom_slider)) {

    int position = m_camera_zoom_slider.GetPos();
    g_main_app->zoom(position);
    
  }
  
  // Update our local records of haptic properties and send them off
  // to the main application...
  m_stiffness = (float)(m_stiffness_slider.GetPos());
  m_static_friction = ((float)m_static_friction_slider.GetPos()) / FRICTION_SLIDER_SCALE;
  m_dynamic_friction = ((float)m_dynamic_friction_slider.GetPos()) / FRICTION_SLIDER_SCALE;
  m_focus = ((float)m_stereo_focus_slider.GetPos()) / FOCUS_SLIDER_SCALE;
  m_separation = ((float)m_stereo_separation_slider.GetPos()) / SEPARATION_SLIDER_SCALE;
   
  update_slider_text();

  g_main_app->update_options_from_gui();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void Cobject_loaderDlg::OnMouseMove(UINT nFlags, CPoint point) {

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

void Cobject_loaderDlg::OnLButtonDown(UINT nFlags, CPoint point) {

  CWnd* pWnd = m_gl_wnd; // GetDlgItem(IDC_GL_AREA);
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

void Cobject_loaderDlg::OnLButtonUp(UINT nFlags, CPoint point) {
	
  CDialog::OnLButtonUp(nFlags, point);  
  m_left_scrolling_gl_area = 0;

}



void Cobject_loaderDlg::OnRButtonDown(UINT nFlags, CPoint point) {

  CWnd* pWnd = m_gl_wnd; // GetDlgItem(IDC_GL_AREA);
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


void Cobject_loaderDlg::OnRButtonUp(UINT nFlags, CPoint point) {
	
  CDialog::OnRButtonUp(nFlags, point);
  m_right_scrolling_gl_area = 0;

}


void Cobject_loaderDlg::OnToggleHapticsButton() {
	
  g_main_app->toggle_haptics();

  CButton* button = (CButton*)(GetDlgItem(IDC_TOGGLEHAPTICS_BUTTON));
  button->SetWindowText(g_main_app->haptics_enabled?"Disable haptics":"Enable haptics");
	
}

void Cobject_loaderDlg::OnToggleStereoButton() {
	
  if (m_gl_wnd) {
    m_gl_wnd->DestroyWindow();
    delete m_gl_wnd;
  }
  
  RECT child;
  CWnd* pWnd = GetDlgItem(IDC_GL_AREA);  
  pWnd->GetClientRect(&child);
  
  m_gl_wnd = new CWnd();
  m_gl_wnd->Create(NULL,
						       NULL,
						       WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE,
						       child,
						       pWnd,
						       0);
  m_gl_area_hwnd = m_gl_wnd->m_hWnd;
  
  int currently_enabled = g_main_app->viewport->getStereoOn();
  g_main_app->reinitialize_viewport(1 - currently_enabled);
	
}


BOOL Cobject_loaderDlg::PreTranslateMessage(MSG* pMsg) {
	
  //_cprintf("message\n");

  // Handle mouse messages explicitly...
  if (pMsg->message == WM_LBUTTONDOWN ||
      pMsg->message == WM_RBUTTONDOWN ||
      pMsg->message == WM_LBUTTONUP   ||
      pMsg->message == WM_RBUTTONUP   ||
      pMsg->message == WM_MOUSEMOVE) {

    //_cprintf("mouse msg\n");

    int x = LOWORD(pMsg->lParam);
    int y = HIWORD(pMsg->lParam);
    CPoint p(x,y);

    if (pMsg->message == WM_LBUTTONDOWN)      OnLButtonDown(pMsg->wParam,p);
    else if (pMsg->message == WM_RBUTTONDOWN) OnRButtonDown(pMsg->wParam,p);
    else if (pMsg->message == WM_LBUTTONUP)   OnLButtonUp(pMsg->wParam,p);
    else if (pMsg->message == WM_RBUTTONUP)   OnRButtonUp(pMsg->wParam,p);
    else if (pMsg->message == WM_MOUSEMOVE)   OnMouseMove(pMsg->wParam,p);    
    
  }

	return CDialog::PreTranslateMessage(pMsg);
}

void Cobject_loaderDlg::OnAnimationButton() {

  g_main_app->toggle_animation();

  CButton* button = (CButton*)(GetDlgItem(IDC_ANIMATION_BUTTON));
  button->SetWindowText(g_main_app->moving_object?"Stop Animation":"Animate Object");

}
