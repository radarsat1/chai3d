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

// Active_xCtl.cpp : Implementation of the CActive_xCtrl ActiveX Control class.

#include "stdafx.h"
#include "active_x.h"
#include "Active_xCtl.h"
#include "Active_xPpg.h"

#define USE_PHANTOM_DIRECT_IO 1


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef M_PI
#define M_PI 3.1415926535898
#endif


IMPLEMENT_DYNCREATE(CActive_xCtrl, COleControl)

void createCube(cMesh *mesh, float edge);


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CActive_xCtrl, COleControl)
	//{{AFX_MSG_MAP(CActive_xCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CActive_xCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CActive_xCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CActive_xCtrl, COleControl)
	//{{AFX_EVENT_MAP(CActive_xCtrl)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CActive_xCtrl, 1)
	PROPPAGEID(CActive_xPropPage::guid)
END_PROPPAGEIDS(CActive_xCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CActive_xCtrl, "ACTIVEX.ActivexCtrl.1",
	0x783c52b1, 0xa05c, 0x46da, 0xaf, 0x6a, 0x9, 0xa1, 0xa0, 0xa0, 0xdc, 0xed)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CActive_xCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DActive_x =
		{ 0xf878b5cb, 0x7b78, 0x47c3, { 0x9a, 0x56, 0xec, 0x9, 0xea, 0x16, 0x1b, 0x65 } };
const IID BASED_CODE IID_DActive_xEvents =
		{ 0x1d96f83b, 0x6d55, 0x4323, { 0x8a, 0x18, 0x32, 0xe7, 0xde, 0x71, 0x29, 0xa } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwActive_xOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CActive_xCtrl, IDS_ACTIVE_X, _dwActive_xOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CActive_xCtrl::CActive_xCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CActive_xCtrl

BOOL CActive_xCtrl::CActive_xCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_ACTIVE_X,
			IDB_ACTIVE_X,
			afxRegApartmentThreading,
			_dwActive_xOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////


// The application's hWnd is not initialized when the constructor is called,
// so wait until it is initialized so that it points to the ActiveX control.
bool hWndPresent = false;
void RenderIteration(void* param) 
{
	CActive_xCtrl* app = (CActive_xCtrl*)(param);

	if (app->m_hWnd == NULL) return;

	// application's hWnd is initialized
	if (!hWndPresent) 
	{
		if (app->viewport != NULL) delete app->viewport;
		app->viewport = new cViewport(app->m_hWnd, app->camera, false);
		hWndPresent = true;
	}

	if (app->viewport != NULL) app->viewport->render();
}


DWORD RenderLoop(void* param)
{
	CActive_xCtrl* app = (CActive_xCtrl*)(param);

	app->viewport = new cViewport(app->m_hWnd, app->camera, false);
	
	while (app->graphics_enabled)
	{
		RenderIteration(param);
		Sleep(50);  // sleep to let haptic thread run faster
	}

	return 0;
}


// A single iteration through the loop...
// Compute the forces and update the graphics
void HapticIteration(void* param) 
{
  CActive_xCtrl* app = (CActive_xCtrl*)(param);
  if (app->tool == NULL) return;

  app->tool->updatePose();
  app->tool->computeForces();
  app->tool->applyForces();
}


// Haptic thread function
DWORD HapticLoop(void* param) 
{
	CActive_xCtrl* app = (CActive_xCtrl*)(param);
	while(app->haptics_enabled) 
	{
		HapticIteration(param);
	}
  app->haptics_thread_running = 0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
/*
  Enable or disable haptics; called when the user double-clicks
  on the image to activate/inactivate haptics.  
  The "enable" parameter is one of :

  #define TOGGLE_HAPTICS_TOGGLE  -1
  #define TOGGLE_HAPTICS_DISABLE  0
  #define TOGGLE_HAPTICS_ENABLE   1
*/

void CActive_xCtrl::toggle_haptics(int enable) 
{

	if (enable == TOGGLE_HAPTICS_TOGGLE) 
	{
		if (haptics_enabled) toggle_haptics(TOGGLE_HAPTICS_DISABLE);
	    else toggle_haptics(TOGGLE_HAPTICS_ENABLE);
	}	
	else if (enable == TOGGLE_HAPTICS_ENABLE) 
	{
  
		if (haptics_enabled) return; // nothing to do here

		haptics_enabled = 1;

		// Create a phantom tool with its graphical representation
 
		// Use device zero, and use either the gstEffect or direct 
		// i/o communication mode, depending on the USE_PHANTOM_DIRECT_IO
		// constant
		if (tool == NULL) 
		{
			// make a tool
			tool = new cMeta3dofPointer(world, 0, USE_PHANTOM_DIRECT_IO);
	
			world->addChild(tool);
 

			// set up a nice-looking workspace for the phantom so 
			// it fits nicely with our shape
			tool->setWorkspace(2.0,2.0,2.0);
    
			// Rotate the tool so its axes align with our opengl-like axes
			tool->rotate(cVector3d(0,0,1),-90.0*M_PI/180.0);
			tool->rotate(cVector3d(1,0,0),-90.0*M_PI/180.0);
			tool->setRadius(0.05);

			// Original point stored for zoom
			original_point = cVector3d(
									   tool->m_deviceLocalPos.x,
									   tool->m_deviceLocalPos.y,
									   tool->m_deviceLocalPos.z
									  );
		}

		// Now phantom is ready to be enabled
		   
		// set up the device
		tool->initialize();

		// Open communication with the device
		tool->start();      

		// tell the tool to show his coordinate frame so you
	  // can see tool rotation
		tool->visualizeFrames(true);

		// I need to call this so the tool can update its internal
		// transformations before performing collision detection, etc.
		tool->computeGlobalPositions();


		// Enable forces
		tool->setForcesON();

		// Tell the proxy algorithm associated with this tool to enable its
		// "dynamic mode", which allows interaction with moving objects
    
    // The dynamic proxy is in a pretty beta state, so we turn it off for now...
		// tool->getProxy()->enableDynamicProxy(1);

		// start haptic thread
		haptics_thread_running = 1;


		DWORD thread_id;
		::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(HapticLoop), this, 0, &thread_id);

		// Boost thread and process priority
		::SetThreadPriority(&thread_id, THREAD_PRIORITY_ABOVE_NORMAL);

	} // enabling

	else if (enable == TOGGLE_HAPTICS_DISABLE) 
	{

		// Don't do anything if haptics are already off
		if (haptics_enabled == 0) return;

		// tell the haptic thread to quit
		haptics_enabled = 0;

		// wait for the haptic thread to quit
		while(haptics_thread_running) Sleep(1);

		// Stop the haptic device...
		if (tool != NULL)
		{
			tool->setForcesOFF();
			tool->stop();
		}

		// Set all rotate/move/zoom variables to 0
		selected_object = 0;
		m_left_scrolling_gl_area = 0;
		m_right_scrolling_gl_area = 0;
		last_left_scroll_point = 0;
		last_right_scroll_point = 0;
  } // disabling
}


/////////////////////////////////////////////////////////////////////////////
// CActive_xCtrl::CActive_xCtrl - Constructor

CActive_xCtrl::CActive_xCtrl()
{
	InitializeIIDs(&IID_DActive_x, &IID_DActive_xEvents);

	// TODO: Initialize your control's instance data here.
	// Initialize Instance Data
	haptics_enabled = 0;
	haptics_thread_running = 0;
	tool = 0;
	viewport = NULL;

	selected_object = 0;
	m_left_scrolling_gl_area = 0;
	m_right_scrolling_gl_area = 0;
	last_left_scroll_point = 0;
	last_right_scroll_point = 0;

	graphics_enabled = 1;

	obj_haptic_force.set(0.0, 0.0, 0.0);


	// WORLD
	world = new cWorld();


	// CAMERA 
	// Create a camera
	camera = new cCamera(world);
	world->addChild(camera);

	// Set camera position and orientation
	int result = camera->set(cVector3d(0,0,4),   // position of camera
		         cVector3d(0.0, 0.0, 0.0),   // camera looking at origin
			     cVector3d(0.0, 1.0, 0.0));  // orientation of camera (standing up)
	

 	// LIGHT
	// Enable one light
	cLight *light = new cLight(world);
	camera->addChild(light);
	light->setEnabled(true);
	light->setPos(cVector3d(240, 200, 200)); 
	light->setDir(cVector3d(-1.4, -1.0, -1.0));
	

	// Create a mesh
	object = new cMesh(world);
	world->addChild(object);

	object = new cMesh(world);
    world->addChild(object);

    double size = 1.0;

    // Create a nice little cube
    createCube(object,(float)size);

    // Move the object over some so the Phantom will not initially be
    // inside the object.
    object->translate(1.5 ,0.0 ,-1.2);

    // Rotate the cube to make it look nice
    object->rotate(cVector3d(0,1,0),30.0 * 3.14159 / 180.0);
    object->rotate(cVector3d(1,0,0),30.0 * 3.14159 / 180.0);

    // Initialize the object's collision-detection state
    object->computeGlobalPositions();
    object->computeBoundaryBox(true);

    // Note that I don't bother to build a fancy collision detector
    // for this cube, since the brute force collision detector -
    // which is the default - works fine for using the haptic proxy
    // on a simple shape with only a few triangles.
  
    // Give a color to each vertex
    for (unsigned int i=0; i<object->getNumVertices(); i++) {
    
      cVertex* nextVertex = object->getVertex(i);

      cColorb color;
      color.set(
        GLuint(0xff*(size + nextVertex->getPos().x ) / (2.0 * size)),
        GLuint(0xff*(size + nextVertex->getPos().y ) / (2.0 * size)),
        GLuint(0xff* nextVertex->getPos().z / 2*size)
        );

      nextVertex->setColor(color);
	}

    // Set object settings.  The parameters tell the object
    // to apply this alpha level to his textures and to his
    // children (of course he has neither right now).
    // object->setTransparencyLevel(0.5, true, true);

    // Give him some material properties...
    cMaterial material;

    // This sets the _haptic_ stiffness of the material
    material.setStiffness(50.0);

    material.m_ambient.set( 0.4f, 0.2f, 0.2f, 1.0f );
    material.m_diffuse.set( 0.8f, 0.6f, 0.6f, 1.0f );
    material.m_specular.set( 0.9f, 0.9f, 0.9f, 1.0f );
    material.setShininess(100);
    object->m_material = material;

    world->computeGlobalPositions(false);
    world->setBackgroundColor(0.9f, 0.9f, 0.9f);
	
	DWORD thread_id;
	::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(RenderLoop), this, 0, &thread_id);
}



/////////////////////////////////////////////////////////////////////////////
// CActive_xCtrl::~CActive_xCtrl - Destructor

CActive_xCtrl::~CActive_xCtrl()
{
	// TODO: Cleanup your control's instance data here.
	graphics_enabled = 0;
	Sleep(50);  // Wait for the thread to exit the loop
	
	toggle_haptics(TOGGLE_HAPTICS_DISABLE);
	if (world != NULL) delete world;
	if (viewport != NULL) delete viewport;
}


/////////////////////////////////////////////////////////////////////////////
// CActive_xCtrl::OnDraw - Drawing function

void CActive_xCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	
}


/////////////////////////////////////////////////////////////////////////////
// CActive_xCtrl::DoPropExchange - Persistence support

void CActive_xCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CActive_xCtrl::OnResetState - Reset control to default state

void CActive_xCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CActive_xCtrl message handlers
void CActive_xCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{	
	COleControl::OnMouseMove(nFlags, point);

	if (m_left_scrolling_gl_area) 
	{
		CPoint delta = point - last_left_scroll_point;    
		scroll(delta,1);
		last_left_scroll_point = point;
	}

	if (m_right_scrolling_gl_area) 
	{
		CPoint delta = point - last_right_scroll_point;
		scroll(delta,0);
		last_right_scroll_point = point;
	}
}


void CActive_xCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (haptics_enabled) 
	{
		world->setBackgroundColor(0.9f, 0.9f, 0.9f);
		toggle_haptics(TOGGLE_HAPTICS_DISABLE);
	}
	else 
	{
		world->setBackgroundColor(0.8f, 0.8f, 0.8f);
		toggle_haptics(TOGGLE_HAPTICS_ENABLE);
	}
	
	COleControl::OnLButtonDblClk(nFlags, point);
}


void CActive_xCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	COleControl::OnLButtonDown(nFlags, point);
	m_left_scrolling_gl_area = 1;
	last_left_scroll_point = point;
	select(point);
}


void CActive_xCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{	
	COleControl::OnLButtonUp(nFlags, point);
	m_left_scrolling_gl_area = 0;
}


void CActive_xCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	COleControl::OnRButtonDown(nFlags, point);
	m_right_scrolling_gl_area = 1;
	last_right_scroll_point = point;
	select(point);
}


void CActive_xCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{	
	COleControl::OnRButtonUp(nFlags, point);
	m_right_scrolling_gl_area = 0;
}


// Called when the user moves the mouse in the main window
// while a button is held down
void CActive_xCtrl::scroll(CPoint p, int left_button) 
{
	// If the user hasn't clicked on any objects, or if haptics is
	// enabled, we don't have to move or rotate anyone.
	if (selected_object == 0) return;

	cGenericObject* object_to_move = selected_object;

	// If the left button is being held down, rotate the
	// selected object
	if (left_button) 
	{
		cVector3d axis1(-1,0,0);
		object_to_move->rotate(axis1,-1.0*(float)p.y / 50.0);
  
		cVector3d axis2(0,1,0);
		object_to_move->rotate(axis2,(float)p.x / 50.0);
	}

	// If the right button is being held down, move the
	// selected object
	else 
	{
		object_to_move->translate((float)p.x / 100.0, 0, 0);
		object_to_move->translate(0, -1.0*(float)p.y / 100.0, 0);
	}

	// Let the object re-compute his global position data
	object->computeGlobalPositions();
	object->computeBoundaryBox(true);
}


void CActive_xCtrl::select(CPoint p) 
{
	selected_object = object;
}


// A global function for sticking a cube in the specified mesh
// 
// Manually creates the 12 triangles (two per face) required to
// model a cube
void createCube(cMesh *mesh, float edge) 
{

    // I define the cube's "radius" to be half the edge size
    float radius = edge / 2.0f;
    int n;
    int cur_index = 0;
    int start_index = 0;

    // +x face
    mesh->newVertex( radius,  radius, -radius);
    mesh->newVertex( radius,  radius,  radius);
    mesh->newVertex( radius, -radius, -radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    mesh->newVertex( radius, -radius, -radius);
    mesh->newVertex( radius,  radius,  radius);
    mesh->newVertex( radius, -radius,  radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    for(n=start_index; n<cur_index; n++) 
	{
      cVertex* curVertex = mesh->getVertex(n);
      curVertex->setNormal(1,0,0);
	}
  
    start_index += 6;

    // -x face
    mesh->newVertex(-radius,  radius,  radius);
    mesh->newVertex(-radius,  radius, -radius);
    mesh->newVertex(-radius, -radius, -radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    mesh->newVertex(-radius,  radius,  radius);
    mesh->newVertex(-radius, -radius, -radius);
    mesh->newVertex(-radius, -radius,  radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    for(n=start_index; n<cur_index; n++) {
      cVertex* curVertex = mesh->getVertex(n);
      curVertex->setNormal(-1,0,0);
	}
  
    start_index += 6;

    // +y face
    mesh->newVertex(radius,  radius,  radius);
    mesh->newVertex(radius,  radius, -radius);
    mesh->newVertex(-radius, radius, -radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    mesh->newVertex(radius,  radius,  radius);
    mesh->newVertex(-radius, radius, -radius);
    mesh->newVertex(-radius, radius,  radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    for(n=start_index; n<cur_index; n++) 
	{
      cVertex* curVertex = mesh->getVertex(n);
      curVertex->setNormal(0,1,0);
	}
  
    start_index += 6;

    // -y face
    mesh->newVertex(radius,  -radius,  radius);
    mesh->newVertex(-radius, -radius, -radius);
    mesh->newVertex(radius,  -radius, -radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    mesh->newVertex(-radius, -radius, -radius);
    mesh->newVertex(radius,  -radius,  radius);
    mesh->newVertex(-radius, -radius,  radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    for(n=start_index; n<cur_index; n++) 
	{
      cVertex* curVertex = mesh->getVertex(n);
      curVertex->setNormal(0,-1,0);
	}
  
    start_index += 6;
  
    // +z face
    mesh->newVertex(-radius, -radius, radius);
    mesh->newVertex(radius,  -radius, radius);
    mesh->newVertex(radius,  radius,  radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    mesh->newVertex(-radius, -radius, radius);
    mesh->newVertex( radius,  radius, radius);
    mesh->newVertex(-radius,  radius, radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    for(n=start_index; n<cur_index; n++) 
	{
      cVertex* curVertex = mesh->getVertex(n);
      curVertex->setNormal(0,0,1);
	}
  
    start_index += 6;
  
    // -z face
    mesh->newVertex(-radius, -radius, -radius);
    mesh->newVertex(radius,   radius, -radius);
    mesh->newVertex(radius,  -radius, -radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    mesh->newVertex( radius,  radius, -radius);
    mesh->newVertex(-radius, -radius, -radius);
    mesh->newVertex(-radius,  radius, -radius);
    mesh->newTriangle(cur_index,cur_index+1,cur_index+2);
    cur_index+=3;

    for(n=start_index; n<cur_index; n++) 
	{
      cVertex* curVertex = mesh->getVertex(n);
      curVertex->setNormal(0,0,-1);
	}
  
    start_index += 6;
}

