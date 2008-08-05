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
#include <conio.h>
#include <process.h>
#include "celapsed.h"

#ifndef M_PI
#define M_PI 3.1415926535898
#endif

/***

  This example demonstrates two ways to drive a haptic loop
  in CHAI.  One can use the cPrecisionTimer class, which uses
  the Windows Multimedia Timer system to request a 1kHz callback,
  which calls the program's haptic loop.  One can also spawn
  a high-priority thread that runs the haptic loop in a while(1)
  loop.

  The former approach uses less CPU but may not really deliver
  1kHz in all cases.  The latter allows complete control of the CPU
  but may slow down other threads.

  This #define controls which approach is used by this application.
***/

// #define USE_MM_TIMER_FOR_HAPTICS


/***

  CHAI supports two modes of communication with the Phantom... via
  the gstEffect Ghost class, and via Ghost's direct i/o model.  This
  #define allows you to switch between them.  Each may perform better
  in specific situations.

***/

#define USE_PHANTOM_DIRECT_IO 1

#define ALLOCATE_SCOPED_GLOBALS
#include "object_loader_globals.h"
#undef ALLOCATE_SCOPED_GLOBALS

// Default initial position for the object that will appear in the main window
cVector3d g_initial_object_pos(1.5,0,-1.2);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Turn off annoying compiler warnings
#pragma warning(disable: 4305)
#pragma warning(disable: 4800)

BEGIN_MESSAGE_MAP(Cobject_loaderApp, CWinApp)
	//{{AFX_MSG_MAP(Cobject_loaderApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// No one really knows why GetConsoleWindow() is not
// properly exported by the MS include files.
extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow (); 

Cobject_loaderApp theApp;

Cobject_loaderApp::Cobject_loaderApp() {

  m_last_animation_time = -1.0;

  moving_object = 0;

  selected_object = 0;

  haptics_enabled = 0;

  tool = 0;

  AllocConsole();

  HWND con_wnd = GetConsoleWindow();

  // We want the console to pop up over any non-TG2 windows, although
  // we'll put the main dialog window above it.
  ::SetForegroundWindow(con_wnd);
  
  SetWindowPos(con_wnd,HWND_TOP,0,0,0,0,SWP_NOSIZE);

  g_main_app = this;
}


void Cobject_loaderApp::uninitialize() {

  toggle_haptics(TOGGLE_HAPTICS_DISABLE);
  delete world;
  delete viewport;
  
}


BOOL Cobject_loaderApp::InitInstance() {

  AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

  memset(keys_to_handle,0,sizeof(keys_to_handle));
	
	g_main_dlg = new Cobject_loaderDlg;
  m_pMainWnd = g_main_dlg;

  g_main_dlg->Create(IDD_object_loader_DIALOG,NULL);
    
  // Now we should have a display context to work with...

  world = new cWorld();

  // set background color
  world->setBackgroundColor(0.0f,0.0f,0.0f);

  // Create a camera
  camera = new cCamera(world);
  world->addChild(camera);

  // set camera position and orientation
  // 
  // We choose to put it out on the positive z axis, so things appear
  // the way OpenGL users expect them to appear, with z going in and
  // out of the plane of the screen.
  int result = camera->set(
              cVector3d(0,0,4),           // position of camera
              cVector3d(0.0, 0.0, 0.0),   // camera looking at origin
              cVector3d(0.0, 1.0, 0.0)  // orientation of camera (standing up)
              );

  if (result == 0) {
    _cprintf("Could not create camera...\n");
  }


  // Create a display for graphic rendering
  viewport = new cViewport(g_main_dlg->m_gl_area_hwnd, camera, false);

  // Create a light source and attach it to camera
  light = new cLight(world);
  camera->addChild(light);
  light->setEnabled(true);
  light->setPos(cVector3d(-1,0,0));
  light->rotate(cVector3d(0,0,1), cDegToRad(180));

  // Create a mesh - we will build a pyramid manually, and later let the
  // user load 3d models
  object = new cMesh(world);
  world->addChild(object);

  double size = 1.0;

  // Create a nice little cube
  createCube(object,size);

  // Move the object over some so the Phantom will not initially be
  // inside the object.
  //object->translate(g_initial_object_pos);
  object->rotate(cVector3d(0,1,0),45.0 * 3.14159 / 180.0);

  object->computeGlobalPositions();
  object->computeBoundaryBox(true);

  // Note that I don't bother to build a fancy collision detector
  // for this cube, since the brute force collision detector -
  // which is the default - works fine for using the haptic proxy
  // on a simple shape with only a few triangles.
  //
  // I will create a fancy collision detector later when I load
  // a model from a file.

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

  
  material.m_ambient.set( 0.4, 0.2, 0.2, 1.0 );
  material.m_diffuse.set( 0.8, 0.6, 0.6, 1.0 );
  material.m_specular.set( 0.9, 0.9, 0.9, 1.0 );
  material.setShininess(100);
  object->m_material = material;

  // These two calls are useful for debugging, but not
  // really important...

  // Tells the object to compute a boundary box around
  // himself and all his children.  We do this here just so
  // he can render his bounding box for your viewing
  // pleasure.
  object->computeBoundaryBox(true);

  // Set the size of the coordinate frame that we're
  // going to gratuitously render
  object->setFrameSize(1.0, false);

  // Compute normals for each triangle
  // object->computeAllNormals();

  world->computeGlobalPositions(false);

  // This will set rendering and haptic options as they're defined in
  // the GUI...
  update_options_from_gui();

  return TRUE;
}


int Cobject_loaderApp::Run() {

  MSG msg;

  // Position the console window so it looks nice...
  HWND con_wnd = GetConsoleWindow();
  RECT r;
  ::GetWindowRect(con_wnd,&r);

  // Move the real window up to the front
  ::SetWindowPos(g_main_dlg->m_hWnd,HWND_TOP,0,r.bottom,0,0,SWP_NOSIZE);  
  ::SetForegroundWindow(g_main_dlg->m_hWnd);

  // Loop forever looking for messages and rendering...
  while (1) {

    while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) {

      if (GetMessage(&msg, NULL, 0, 0)) {

        int message_processed = PreTranslateMessage(&msg);

        if (message_processed == 0) TranslateMessage(&msg);

        // Record any keystrokes that need to be handled by the rendering
        // or haptics loops...
        if (msg.message == WM_KEYDOWN) {
          keys_to_handle[msg.wParam] = 1;    
        }

        else if (msg.message == WM_KEYUP) {        
          keys_to_handle[msg.wParam] = 0;
        }

        if (message_processed == 0) DispatchMessage(&msg);

      }

      // Quit if GetMessage(...) fails
      else return TRUE;

    }    
    
    render_loop();

    // We can sleep (yield) here to be extra well-behaved, not necessary
    // for a game-like app that is happy to eat CPU
    // Sleep(1);

  }
  
} // Run()


// Our main drawing loop...
int Cobject_loaderApp::render_loop() {

  // Perform animation if the haptics thread isn't running but
  // we're supposed to be moving the object
  if (haptics_thread_running == 0 && moving_object) {

    animate();

  }


  viewport->render();

  return 0;

}


void Cobject_loaderApp::update_options_from_gui() {

  // The second parameter in each case transfers options to
  // any children the object has...
  object->setWireMode(g_main_dlg->m_usewireframe_check, true);
  object->setShowFrame(g_main_dlg->m_showframe_check, true);
  object->showNormals(g_main_dlg->m_shownormals_check, true);
  object->setShowBox(g_main_dlg->m_showbox_check, true);
  object->useTexture(g_main_dlg->m_usetexture_check, true);
  object->useColors(g_main_dlg->m_usecolors_check, true);
  object->useMaterial(g_main_dlg->m_material_check, true);
  
  object->setStiffness(g_main_dlg->m_stiffness,1);
  object->setFriction(g_main_dlg->m_static_friction,g_main_dlg->m_dynamic_friction,1);

  camera->setStereoEyeSeparation(g_main_dlg->m_separation);
  camera->setStereoFocalLength(g_main_dlg->m_focus);
}


// A function used to pop up a dialog box and ask the user to select
// a file (for selecting game files) (last five parameters are optional).
int FileBrowse(char* buffer, int length, int save, char* forceExtension,
               char* extension_string, char* title) {
  
  CFileDialog cfd(1-save,0,0,0,extension_string,g_main_dlg);

  if (forceExtension != 0) {
    cfd.m_ofn.lpstrDefExt = forceExtension;
  }

  if (title != 0) {
    cfd.m_ofn.lpstrTitle = title;
  }

  if (save) {
    cfd.m_ofn.Flags |= OFN_OVERWRITEPROMPT;
  }

  // We don't want to change the working path
  cfd.m_ofn.Flags |= OFN_NOCHANGEDIR;

  if (cfd.DoModal() != IDOK) return -1;

  CString result = cfd.GetPathName();

  // This should never happen, since I always pass in _MAX_PATH
  // for the length.
  if (result.GetLength() > length) {
    _cprintf("Warning: truncating filename to %d characters\n",length);
  }

  strncpy(buffer,(LPCSTR)(result),length);

  // strncpy needs help with the nul termination
  buffer[length-1] = '\0';

  return 0;
}


// Called by the GUI when the user clicks the "load model"
// button.  Opens the specified file and displays the model (graphically
// and haptically).
//
// To demonstrate some extra mesh processing, this function also scales
// the mesh's vertices to fit nicely in the display and centers the model
// in the viewing area.
int Cobject_loaderApp::LoadModel(char* filename) {

  // create a new mesh
  cMesh* new_object = new cMesh(world);
  
  _cprintf("Loading mesh file %s\n",filename);

  // load 3d object file
  int result = new_object->loadFromFile(filename);

  if (result == 0) {

    _cprintf("Could not load model %s\n",filename);
    delete new_object;
    return -1;

  }

  // I'm going to scale the object so his maximum axis has a
  // size of MESH_SCALE_SIZE. This will make him fit nicely in
  // our viewing area.
#define MESH_SCALE_SIZE 2.0

  // Tell him to compute a bounding box...
  new_object->computeBoundaryBox(true);

  cVector3d min = new_object->getBoundaryMin();
  cVector3d max = new_object->getBoundaryMax();

  // This is the "size" of the object
  cVector3d span = max;
  span.sub(min);

  // Find his maximum dimension
  float max_size = span.x;
  if (span.y > max_size) max_size = span.y;
  if (span.z > max_size) max_size = span.z;

  // We'll center all vertices, then multiply by this amount,
  // to scale to the desired size.
  float scale_factor = MESH_SCALE_SIZE / max_size;

  // To center vertices, we add this amount (-1 times the
  // center of the object's bounding box)
  cVector3d offset = max;
  offset.add(min);
  offset.div(2.0);
  offset.negate();

  // Now we need to actually scale all the vertices.  However, the
  // vertices might not actually be in this object; they might
  // be in children or grand-children of this mesh (depending on how the 
  // model was defined in the file).
  // 
  // So we find all the sub-meshes we loaded from this file, by descending
  // through all available children.

  // This will hold all the meshes we need to operate on... we'll fill
  // it up as we find more children.
  std::list<cMesh*> meshes_to_scale;

  // This will hold all the parents we're still searching...
  std::list<cMesh*> meshes_to_descend;
  meshes_to_descend.push_front(new_object);

  // Keep track of how many meshes we've found, just to print
  // it out for the user
  int total_meshes = 0;

  // While there are still parent meshes to process
  while(meshes_to_descend.empty() == 0) {

    total_meshes++;

    // Grab the next parent
    cMesh* cur_mesh = meshes_to_descend.front();
    meshes_to_descend.pop_front();
    meshes_to_scale.push_back(cur_mesh);

    // Put all his children on the list of parents to process
    for(int i=0; i<cur_mesh->getNumChildren(); i++) {

      cGenericObject* cur_object = cur_mesh->getChild(i);

      // Only process cMesh children
      cMesh* cur_mesh = dynamic_cast<cMesh*>(cur_object);
      if (cur_mesh) meshes_to_descend.push_back(cur_mesh);
    }
  }

  _cprintf("Loaded %d vertices in %d meshes from model %s\n",
    new_object->getNumVertices(true),total_meshes,filename);
  
  std::list<cMesh*>::iterator mesh_iter;

  // Now loop over _all_ the meshes we found...
  for(mesh_iter = meshes_to_scale.begin(); mesh_iter != meshes_to_scale.end(); mesh_iter++) {

    cMesh* cur_mesh = *mesh_iter;
    vector<cVertex>* vertices = cur_mesh->pVertices();
    int num_vertices = cur_mesh->getNumVertices(false);
    cVertex* cur_vertex = (cVertex*)(vertices);

    // Move and scale each vertex in this mesh...
    for(int i=0; i<num_vertices; i++) {
      cur_vertex = cur_mesh->getVertex(i);
      cVector3d pos = cur_vertex->getPos();      
      pos.add(offset);
      pos.mul(scale_factor);
      cur_vertex->setPos(pos);
      cur_vertex++;
    }
  }

  int size = new_object->pTriangles()->size();

  // Re-compute a bounding box
  new_object->computeBoundaryBox(true);
  
  // Build a nice collision-detector for this object, so
  // the proxy will work nicely when haptics are enabled.
  _cprintf("Building collision detector...\n");

  // new_object->createSphereTreeCollisionDetector(true,true);
  new_object->createAABBCollisionDetector(true,true);
  
  _cprintf("Finished building collision detector...\n");

  new_object->computeGlobalPositions();
  
  // Replace the old object we're displaying with the
  // new one
  //
  // Really I might synchronize this operation with the
  // haptic thread - if there is one - but that's beyond the scope
  // of this demo.
  world->removeChild(object);
  delete object;
  object = new_object;
  world->addChild(object);

  // This will set up default rendering options and haptic properties
  update_options_from_gui();

  return 0;

}


// Applies a texture - loaded from the specified file -
// to the current model, if texture coordinates are
// defined.
int Cobject_loaderApp::LoadTexture(char* filename) {

  cTexture2D *newTexture = world->newTexture();
  int result = newTexture->loadFromFile(filename);

  if (result == 0) {
    _cprintf("Could not load texture file %s\n",filename);
    delete newTexture;
    return -1;
  }

  object->setTexture(newTexture,1);
  object->useTexture(1,1);

  return 0;
}


// Called when the camera zoom slider changes
void Cobject_loaderApp::zoom(int zoom_level) {

  //camera->setPos(p.x,((float)(zoom_level))/100.0*10.0,p.z);
  camera->setFieldViewAngle(((float)(zoom_level)));
  
}


// Called when the user moves the mouse in the main window
// while a button is held down
void Cobject_loaderApp::scroll(CPoint p, int left_button) {

  if (selected_object == 0) return;

  // If the user hasn't clicked on any objects, we don't
  // have to move or rotate anyone
  cGenericObject* object_to_move = selected_object;

  // We want to move the _parent_ mesh around, so let's
  // walk up the tree, looking for the highest-level object
  // that's still a cMesh...
  //
  // The reason we do this is that maybe I've loaded, say,
  // a model of a person from a file, that might contain
  // separate meshes for each arm.  If I click on that mesh,
  // CHAI will indeed find that mesh, but really I don't
  // want the user to pull the model's arm off, I want to move
  // the larger model around.
  cGenericObject* tmp = object_to_move;

  // Walk up the tree...
  while(1) {

    tmp = tmp->getParent();
    
    // If there was no parent, we hit the highest-level mesh
    if (tmp == 0) break;

    // If the parent was not a cMesh, we hit the highest-level mesh
    cMesh* mesh = dynamic_cast<cMesh*>(tmp);
    if (mesh == 0) break;

    // Okay, this is a mesh...
    object_to_move = tmp;

  }

  if (left_button) {

    cVector3d axis1(-1,0,0);
    object_to_move->rotate(axis1,-1.0*(float)p.y / 50.0);
  
    cVector3d axis2(0,1,0);
    object_to_move->rotate(axis2,(float)p.x / 50.0);

  }

  else {

    object_to_move->translate((float)p.x / 100.0, 0, 0);
    object_to_move->translate(0, -1.0*(float)p.y / 100.0, 0);

  }

  object->computeGlobalPositions(1);
  // object->computeBoundaryBox(true);
  
}


// Called when the user clicks his mouse in the main window
//
// Lets CHAI figure out which object was clicked on.
void Cobject_loaderApp::select(CPoint p) {

  if (viewport->select(p.x, p.y, true)) {
    selected_object = viewport->getLastSelectedObject();         
  }
  else {    
    selected_object = 0;
  }

}


// Our haptic loop... just computes forces on the 
// phantom every iteration, until haptics are disabled
// in the supplied Cobject_loaderApp

// A single iteration through the loop...
void object_loader_haptic_iteration(void* param) {

  Cobject_loaderApp* app = (Cobject_loaderApp*)(param);

  app->animate();

  app->tool->updatePose();
  app->tool->computeForces();
  app->tool->applyForces();

}


void Cobject_loaderApp::animate() {

  // The translational and rotational velocities (gl units/s and radians/s)
  // at which the object is animated when the 'toggle animation' button gets
  // clicked
  #define OBJECT_X_VELOCITY -0.5
  #define OBJECT_R_VELOCITY (M_PI / 4.0)

  if (object == 0) return;
       
  // The A and D keys can be used to move the current object around
  if (keys_to_handle['A']) {
    object->translate(-0.02,0,0);
    object->computeGlobalPositions(1);
    keys_to_handle['A'] = 0;
  }
  if (keys_to_handle['D']) {
    object->translate( 0.02,0,0);
    object->computeGlobalPositions(1);
    keys_to_handle['D'] = 0;
  }
          

  CElapsed clock;
  double curtime = clock.GetTime();
    
  if (moving_object && m_last_animation_time >= 0) {
    double elapsed = curtime - m_last_animation_time;

    // Move the object...
    double x_delta = elapsed * OBJECT_X_VELOCITY;
    cVector3d delta(x_delta,0,0);
    object->translate(delta);
    
    double r_delta = elapsed * OBJECT_R_VELOCITY;
    object->rotate(cVector3d(0,1,0),r_delta);

    // This is necessary to allow collision detection to work
    // after object movement
    object->computeGlobalPositions(1);    
    // object->computeBoundaryBox(true);
  }

  m_last_animation_time = curtime;

}


void Cobject_loaderApp::toggle_animation() {

  if (object == 0) return;

  // Disable animation
  if (moving_object) {
    moving_object = 0;
    object->setPos(g_initial_object_pos);
    object->computeGlobalPositions(1);
  }

  // Enable animation
  else {
    moving_object = 1;
    m_last_animation_time = -1.0;
  }

}


// This loop is used only in the threaded version of this
// application... all it does is call the main haptic
// iteration loop, which is called directly from a timer
// callback if USE_MM_TIMER_FOR_HAPTICS is defined
DWORD object_loader_haptic_loop(void* param) {

  Cobject_loaderApp* app = (Cobject_loaderApp*)(param);

  while(app->haptics_enabled) {

    object_loader_haptic_iteration(param);

  }

  app->haptics_thread_running = 0;

  return 0;
}


void Cobject_loaderApp::reinitialize_viewport(int stereo_enabled) {

  if (viewport) delete viewport;

  viewport = new cViewport(g_main_dlg->m_gl_area_hwnd, camera, stereo_enabled);

  _cprintf("Stereo rendering is %s\n",viewport->getStereoOn()?"enabled":"disabled");

}


/***
 
  Enable or disable haptics; called when the user clicks
  the enable/disable haptics button.  The "enable" parameter
  is one of :

  #define TOGGLE_HAPTICS_TOGGLE  -1
  #define TOGGLE_HAPTICS_DISABLE  0
  #define TOGGLE_HAPTICS_ENABLE   1

***/
void Cobject_loaderApp::toggle_haptics(int enable) {

  if (enable == TOGGLE_HAPTICS_TOGGLE) {

    if (haptics_enabled) toggle_haptics(TOGGLE_HAPTICS_DISABLE);
    else toggle_haptics(TOGGLE_HAPTICS_ENABLE);

  }

  else if (enable == TOGGLE_HAPTICS_ENABLE) {
  
    if (haptics_enabled) return;

    haptics_enabled = 1;

    // create a phantom tool with its graphical representation
    //
    // Use device zero, and use either the gstEffect or direct 
    // i/o communication mode, depending on the USE_PHANTOM_DIRECT_IO
    // constant
    if (tool == 0) {
 
      tool = new cMeta3dofPointer(world, 0, USE_PHANTOM_DIRECT_IO);
      
      // The tool is rendered as a child of the camera.
      //
      // This has the nice property of always aligning the Phantom's
      // axes with the camera's axes.

      // camera ->addChild(tool);

      // Actually, that doesn't work well... need to sort out why cd 
      // doesn't operate in the right frame if the tool is somebody's
      // child.
      world->addChild(tool);
    
      // set up a nice-looking workspace for the phantom so 
      // it fits nicely with our models
      tool->setWorkspace(2.0,2.0,2.0);
    
      // Rotate the tool so its axes align with our opengl-like axes
      tool->rotate(cVector3d(0,0,1),-90.0*M_PI/180.0);
      tool->rotate(cVector3d(1,0,0),-90.0*M_PI/180.0);
      tool->setRadius(0.05);
    
    }
    
    // set up the device
    tool->initialize();

    // open communication to the device
    tool->start();

    // update initial orientation and position of device
    tool->updatePose();

    // tell the tool to show his coordinate frame so you
    // can see tool rotation
    tool->visualizeFrames(true);

    // I need to call this so the tool can update its internal
    // transformations before performing collision detection, etc.
    tool->computeGlobalPositions();
    tool->ForcesON();
    
    // Enable the "dynamic proxy", which will handle moving objects
    cProxyPointForceAlgo* proxy = tool->getProxy();

    proxy->enableDynamicProxy(1);

#ifdef USE_MM_TIMER_FOR_HAPTICS

    // start the mm timer to run the haptic loop
    timer.set(0,object_loader_haptic_iteration,this);

#else

    // start haptic thread
    haptics_thread_running = 1;

    DWORD thread_id;
    ::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(object_loader_haptic_loop), this, 0, &thread_id);

    // Boost thread and process priority
    ::SetThreadPriority(&thread_id, THREAD_PRIORITY_ABOVE_NORMAL);
    //::SetPriorityClass(GetCurrentProcess(),ABOVE_NORMAL_PRIORITY_CLASS);

#endif
       
  } // enabling

  else if (enable == TOGGLE_HAPTICS_DISABLE) {

    // Don't do anything if haptics are already off
    if (haptics_enabled == 0) return;

    // tell the haptic thread to quit
    haptics_enabled = 0;

#ifdef USE_MM_TIMER_FOR_HAPTICS

    timer.stop();

#else

    // wait for the haptic thread to quit
    while(haptics_thread_running) Sleep(1);

#endif
    
    // Stop the haptic device...
    tool->ForcesOFF();
    tool->stop();
    
    // SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);    

  } // disabling

} // toggle_haptics()


// A global function for sticking a cube in the given mesh
// 
// Manually creates the 12 triangles (two per face) required to
// model a cube
void createCube(cMesh *mesh, float edge) {

  // I define the cube's "radius" to be half the edge size
  float radius = edge / 2.0;
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

  for(n=start_index; n<cur_index; n++) {
    cVertex* curVertex = mesh->getVertex(n);
    curVertex->setTexCoord(
      (curVertex->getPos().y + radius) / (2.0 * radius),
      (curVertex->getPos().z + radius) / (2.0 * radius)
      );
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
    curVertex->setTexCoord(
      (curVertex->getPos().y + radius) / (2.0 * radius),
      (curVertex->getPos().z + radius) / (2.0 * radius)
      );
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

  for(n=start_index; n<cur_index; n++) {
    cVertex* curVertex = mesh->getVertex(n);
    curVertex->setTexCoord(
      (curVertex->getPos().x + radius) / (2.0 * radius),
      (curVertex->getPos().z + radius) / (2.0 * radius)
      );
    curVertex->setNormal(0,1,0);
  }
  
  start_index += 6;

  // +y face
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

  for(n=start_index; n<cur_index; n++) {
    cVertex* curVertex = mesh->getVertex(n);
    curVertex->setTexCoord(
      (curVertex->getPos().x + radius) / (2.0 * radius),
      (curVertex->getPos().z + radius) / (2.0 * radius)
      );
    curVertex->setNormal(0,-1,0);
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

  for(n=start_index; n<cur_index; n++) {
    cVertex* curVertex = mesh->getVertex(n);
    curVertex->setTexCoord(
      (curVertex->getPos().x + radius) / (2.0 * radius),
      (curVertex->getPos().y + radius) / (2.0 * radius)
      );
    curVertex->setNormal(0,0,-1);
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

  for(n=start_index; n<cur_index; n++) {
    cVertex* curVertex = mesh->getVertex(n);
    curVertex->setTexCoord(
      (curVertex->getPos().x + radius) / (2.0 * radius),
      (curVertex->getPos().y + radius) / (2.0 * radius)
      );
    curVertex->setNormal(0,0,1);
  }
  
  start_index += 6;

  
}
