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
    \author:    Christopher Sewell, Bera, and Federico Barbagli
    \version    1.2
    \date       06/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "main.h"
#pragma package(smart_init)
#pragma resource "*.dfm"

#define USE_PHANTOM_DIRECT_IO 1

// Maximum number of contact points handled by ODE
#define MAXCONTACTS 20

// Time step for ODE dynamics simulation
#define TIME_STEP	0.001//sec 300Hz

// Default stiffness and friction for the walls
#define K 20
#define MU_S 0.6
#define MU_D 0.6

// Size and position of the walls that form the room
// in which the box is pushed around
#define LEFT_WALL_OFFSET -12.0
#define RIGHT_WALL_OFFSET 12.0
#define FRONT_WALL_OFFSET 12.0
#define BACK_WALL_OFFSET -12.0
#define WALL_Z_OFFSET -8.0
#define BOTTOM_WALL_OFFSET -10.7
#define WALL_SIZE 20.0

//---------------------------------------------------------------------------
TForm1 *Form1;
void HapticLoop();

// Default initial position for the object that will appear in the main window
cVector3d g_initial_object_pos(0, 1, 0);
//---------------------------------------------------------------------------

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

// callback function for ODE
void TForm1::nearCallback (void *data, dGeomID o1, dGeomID o2) {
  TForm1 *pFrame = (TForm1*)data;

  int i;
  dBodyID b1 = dGeomGetBody(o1);
  dBodyID b2 = dGeomGetBody(o2);

  dContact contact[MAXCONTACTS];
  for (i=0; i<MAXCONTACTS; i++) {
    contact[i].surface.mode = dContactApprox1 | dContactBounce;
    contact[i].surface.mu = 0.2;
    contact[i].surface.bounce = 0.2;
    contact[i].surface.bounce_vel = 0.2;
  }
  if (int numc = dCollide (o1,o2,MAXCONTACTS,&contact[0].geom,sizeof(dContact))) {
    for (i=0; i<numc; i++) {
      dJointID c = dJointCreateContact (pFrame->ode_world,pFrame->ode_contact_group,contact+i);
      dJointAttach (c,b1,b2);
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
  haptics_enabled = 0;
  first_device_enabled = 0;
  second_device_enabled = 0;
  tool1 = 0;
  tool2 = 0;
  ready = true;
  tool1_ready = 0;
  tool2_ready = 0;

  // create a new world
  world = new cWorld();

  // set background color
  world->setBackgroundColor(0.0f,0.0f,0.0f);

  // create a camera
  camera = new cCamera(world);
  world->addChild(camera);

  camera->set(
            cVector3d(5,0.0,0),          // position of camera
            cVector3d(0.0, 0.0, 0.0),    // camera looking at origin
            cVector3d(0.0, 0.0, 1.0)     // orientation of camera (standing up)
            );

  // Create a light source and attach it to the camera
  light = new cLight(world);
  light->setEnabled(true);

  // A purely directional light, pointing into the monitor
  // and a little to the right... note that to accomplish
  // that, I "place" the light off to the left and behind the
  // origin (positive z relative to the origin).
  light->setDirectionalLight(false);
  light->setPos(cVector3d(4,1,1));
  camera->addChild(light);

  // create a display for graphic rendering
  viewport = new cViewport(Panel1->Handle, camera, true);
  viewport->setStereoOn(false);

  // Initialize the ODE context
  ode_world = dWorldCreate();
  dWorldSetGravity (ode_world,0,0,-5);
  ode_step = TIME_STEP;
  ode_space = dSimpleSpaceCreate(0);
  ode_contact_group = dJointGroupCreate(0);
  ode_collision_callback	  = TForm1::nearCallback;
  ode_collision_callback_data  = this;

  // create cube
  object = new cODEMesh(world, ode_world, ode_space);
  world->addChild(object);

  double size = 1.0;
  createCube(object,size);

  // initialize ODE parameters
  // Move the object over some so the Phantom will not initially be
  // inside the object.
  object->initDynamic(DYNAMIC_OBJECT, g_initial_object_pos.x , g_initial_object_pos.y, g_initial_object_pos.z);
  object->setMass(0.2);

  // Give a color to each vertex
  for (unsigned int i=0; i<object->getNumVertices(); i++)
  {
    cVertex* nextVertex = object->getVertex(i);
    cColorb color;
    color.set(
      GLuint(0xff*(size + nextVertex->getPos().x ) / (2.0 * size)),
      GLuint(0xff*(size + nextVertex->getPos().y ) / (2.0 * size)),
      GLuint(0xff* nextVertex->getPos().z / 2*size)
      );

    nextVertex->setColor(color);
  }

  // Initialize the box		
  object->computeBoundaryBox(true);
  object->computeAllNormals();
  object->setShowBox(true);
  object->setShowFrame(false,true);
  object->computeGlobalPositions(1);
  object->createAABBCollisionDetector(true,true);

  // Create a room, bounded by cubes, in which we can push around our little box
  bottom_wall = create_wall(0,0,BOTTOM_WALL_OFFSET,1);
  left_wall = create_wall(0,LEFT_WALL_OFFSET,WALL_Z_OFFSET,0);
  right_wall = create_wall(0,RIGHT_WALL_OFFSET,WALL_Z_OFFSET,0);
  back_wall = create_wall(BACK_WALL_OFFSET,0,WALL_Z_OFFSET,0);
  front_wall = create_wall(FRONT_WALL_OFFSET,0,0,0);

  world->computeGlobalPositions(false);

  // don't start the haptics loop yet
  flagSimulationOn = false;
  flagHasExitedSimulation = true;
}


cODEMesh* TForm1::create_wall(double a_x, double a_y, double a_z, bool a_top) {
	cODEMesh* new_mesh = new cODEMesh(world, ode_world, ode_space);
  createCube(new_mesh, WALL_SIZE, a_top);
  new_mesh->m_material.m_ambient.set( 0.7, 0.7, 0.7, 1.0 );
  new_mesh->m_material.m_diffuse.set( 0.7, 0.7, 0.7, 1.0 );
  new_mesh->m_material.m_specular.set( 0.7, 0.7, 0.7, 1.0 );
  new_mesh->m_material.setShininess(100);
  new_mesh->m_material.setStiffness(K);
  new_mesh->setFriction(MU_S, MU_D, 1);
  new_mesh->initDynamic(STATIC_OBJECT,a_x, a_y, a_z);
  world->addChild(new_mesh);
  new_mesh->computeAllNormals();

  // If this is the ground, actually make two boxes -- one used by ODE and one for
  // haptics, with the one used by ODE slightly above.  This will make the box "float"
  // slightly above the ground, avoiding the problem of when the proxy gets stuck 
  // between the falling box and the ground and has to penetrate one or the other
  if (a_top == 1)
  {
    new_mesh->setHapticEnabled(false, true);
    cMesh* offset_bottom = new cMesh(world);
    createCube(offset_bottom, WALL_SIZE, a_top);
    offset_bottom->m_material.m_ambient.set( 0.7, 0.7, 0.7, 1.0 );
    offset_bottom->m_material.m_diffuse.set( 0.7, 0.7, 0.7, 1.0 );
    offset_bottom->m_material.m_specular.set( 0.7, 0.7, 0.7, 1.0 );
    offset_bottom->m_material.setShininess(100);
    offset_bottom->m_material.setStiffness(K);
    offset_bottom->setFriction(MU_S, MU_D, 1);
    offset_bottom->translate(a_x, a_y, a_z-0.1);
    world->addChild(offset_bottom);
    offset_bottom->computeAllNormals();
  }
	
  return new_mesh;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
    // stop simulation
    flagSimulationOn = false;
    while (!flagHasExitedSimulation) { Sleep(1); }

    // stop graphic rendering
    Timer1->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  // set stiffness and friction based on current slider positions
  if (object)
  {
    object->setStiffness(StiffnessSlider->Position, true);
    object->setFriction(StaticSlider->Position/100.0, DynamicSlider->Position/100.0, true);
  }

  // render world in display
  if (viewport != NULL)
    viewport->render();
}
//---------------------------------------------------------------------------

void HapticLoop()
{
  // simulation in now ON
  flagSimulationOn = true;
  flagHasExitedSimulation = false;

  // main haptics loop
  while(flagSimulationOn)
  {
    // set it up so that wherever your tool is in the real world when you enable forces, it will be at (1,0,3) in the
    // virtual world, so that it is in side the virtual "room" and not penetrating anything
    if (Form1->first_device_enabled && Form1->tool1)
    {
      // update the tool's pose and compute and apply forces
      Form1->tool1->updatePose();

      if (!Form1->tool1_ready)
      {
        // Turn off haptic collision detection for the objects in the world
        for (unsigned int i=0; i<Form1->world->getNumChildren(); i++)
          Form1->world->getChild(i)->setHapticEnabled(0, 1);

        if ( (Form1->tool1->m_deviceGlobalPos.x > BACK_WALL_OFFSET+WALL_SIZE/2+0.2) &&
             (Form1->tool1->m_deviceGlobalPos.x < FRONT_WALL_OFFSET-WALL_SIZE/2-0.2) &&
             (Form1->tool1->m_deviceGlobalPos.y > LEFT_WALL_OFFSET+WALL_SIZE/2+0.2) &&
             (Form1->tool1->m_deviceGlobalPos.y < RIGHT_WALL_OFFSET-WALL_SIZE/2-0.2) &&
             (Form1->tool1->m_deviceGlobalPos.z > BOTTOM_WALL_OFFSET+WALL_SIZE/2+0.2))
               Form1->tool1_ready = 1;
      }
      else
      {
        // Turn on haptic collision detection for the objects in the world
        for (unsigned int i=0; i<Form1->world->getNumChildren(); i++)
          if (Form1->world->getChild(i) != Form1->bottom_wall)
            Form1->world->getChild(i)->setHapticEnabled(1, 1);
      }

      Form1->tool1->computeForces();
      if (Form1->tool1_ready)
      Form1->tool1->applyForces();
    }

    // if two devices are active, update the second one also...
    if (Form1->second_device_enabled && Form1->tool2)
    {
      Form1->tool2->updatePose();

      if (!Form1->tool2_ready)
      {
        // Turn off haptic collision detection for the objects in the world
        for (unsigned int i=0; i<Form1->world->getNumChildren(); i++)
          Form1->world->getChild(i)->setHapticEnabled(0, 1);

          if ( (Form1->tool2->m_deviceGlobalPos.x > BACK_WALL_OFFSET+WALL_SIZE/2+0.2) &&
               (Form1->tool2->m_deviceGlobalPos.x < FRONT_WALL_OFFSET-WALL_SIZE/2-0.2) &&
               (Form1->tool2->m_deviceGlobalPos.y > LEFT_WALL_OFFSET+WALL_SIZE/2+0.2) &&
               (Form1->tool2->m_deviceGlobalPos.y < RIGHT_WALL_OFFSET-WALL_SIZE/2-0.2) &&
               (Form1->tool2->m_deviceGlobalPos.z > BOTTOM_WALL_OFFSET+WALL_SIZE/2+0.2))
            Form1->tool2_ready = 1;
      }
      else
      {
        // Turn on haptic collision detection for the objects in the world
        for (unsigned int i=0; i<Form1->world->getNumChildren(); i++)
          if (Form1->world->getChild(i) != Form1->bottom_wall)
            Form1->world->getChild(i)->setHapticEnabled(1, 1);
      }

      Form1->tool2->computeForces();
      if (Form1->tool2_ready)
        Form1->tool2->applyForces();
    }

    Form1->object->m_historyValid = false;

    // code to get forces from CHAI and apply them to the appropriate meshes, calling ODE functions to calculate the dynamics
    if (Form1->ode_clock->on())
    {
      if ( (Form1->ode_clock->timeoutOccurred()) && Form1->ready)
      {
        Form1->ready = false;

        if (Form1->tool1 && Form1->first_device_enabled)
        {
          cProxyPointForceAlgo *proxy   = dynamic_cast<cProxyPointForceAlgo*>(Form1->tool1->getProxy());

          if (proxy && proxy->getContactObject() != NULL)
          {
            float x =  proxy->getContactPoint().x;
            float y =  proxy->getContactPoint().y;
            float z =  proxy->getContactPoint().z;

            float fx = -Form1->tool1->m_lastComputedGlobalForce.x ;
            float fy = -Form1->tool1->m_lastComputedGlobalForce.y ;
            float fz = -Form1->tool1->m_lastComputedGlobalForce.z ;

            cGenericObject* cur_object = proxy->getContactObject();
            bool found = false;
            cODEMesh* mesh = 0;
            while (cur_object && !found)
            {
              mesh = dynamic_cast<cODEMesh*>(cur_object);
              if (mesh) found = true;
              cur_object = cur_object->getParent();
            }

            if ((found) && (mesh) && (mesh->m_objType == DYNAMIC_OBJECT))
              dBodyAddForceAtPos(mesh->m_odeBody,fx,fy,fz,x,y,z);
          }
        }

        if (Form1->tool2 && Form1->second_device_enabled)
        {
          cProxyPointForceAlgo *proxy   = dynamic_cast<cProxyPointForceAlgo*>(Form1->tool2->getProxy());

          if (proxy && proxy->getContactObject() != NULL)
          {
            float x =  proxy->getContactPoint().x;
            float y =  proxy->getContactPoint().y;
            float z =  proxy->getContactPoint().z;

            float fx = -Form1->tool2->m_lastComputedGlobalForce.x ;
            float fy = -Form1->tool2->m_lastComputedGlobalForce.y ;
            float fz = -Form1->tool2->m_lastComputedGlobalForce.z ;

            cGenericObject* cur_object = proxy->getContactObject();
            bool found = false;
            cODEMesh* mesh = 0;
            while (cur_object && !found)
            {
              mesh = dynamic_cast<cODEMesh*>(cur_object);
              if (mesh) found = true;
              cur_object = cur_object->getParent();
            }

            if ((found) && (mesh) && (mesh->m_objType == DYNAMIC_OBJECT))
            dBodyAddForceAtPos(mesh->m_odeBody,fx,fy,fz,x,y,z);
          }
        }

        dSpaceCollide (Form1->ode_space,Form1->ode_collision_callback_data,Form1->ode_collision_callback);
        dWorldStep(Form1->ode_world,Form1->ode_step);
        dJointGroupEmpty(Form1->ode_contact_group);

        Form1->object->updateDynamicPosition();
        Form1->object->m_historyValid = true;
        if (Form1->tool1 && Form1->first_device_enabled)
          Form1->tool1->computeGlobalPositions(1);

        if (Form1->tool2 && Form1->second_device_enabled)
          Form1->tool2->computeGlobalPositions(1);

        Form1->ode_clock->initialize();
        Form1->ready = true;
      }
    }
  }

  // stop haptics
  Form1->tool1->stop();
  flagHasExitedSimulation = true;

  // exit thread
  ExitThread(0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ToggleHapticsButtonClick(TObject *Sender)
{
  flagHasExitedSimulation = false;

  // create a thread for the haptic loop
  if (!flagSimulationOn)
  {
    tool1_ready = 0;
    haptics_enabled = 1;

    // create a phantom tool with its graphical representation
    //
    // Use device zero, and use either the gstEffect or direct 
    // i/o communication mode, depending on the USE_PHANTOM_DIRECT_IO
    // constant
    if (tool1 == 0) 
    {
      // create a haptic tool and set up its workspace
      tool1 = new cMeta3dofPointer(world, 0, USE_PHANTOM_DIRECT_IO);
      tool1->setWorkspace(3.0,3.0,3.0);

      world->addChild(tool1);        
      tool1->setRadius(0.05);
      tool1->computeGlobalCurrentObjectOnly(true);

      // Replace the proxy with our custom ODE proxy
      cProxyPointForceAlgo* old_proxy = (cProxyPointForceAlgo*)(tool1->m_pointForceAlgos[0]);
      tool1->m_pointForceAlgos[0] = new cODEProxy(old_proxy);
      delete old_proxy;
    }

    // set up the device
    tool1->initialize();

    // open communication to the device
    tool1->start();

    // I need to call this so the tool can update its internal
    // transformations before performing collision detection, etc.
    tool1->computeGlobalPositions();
    tool1->setForcesON();

    tool1->updatePose();
    Sleep(1000);

    ode_clock = new cPrecisionClock();
    ode_clock->setTimeoutPeriod(TIME_STEP * 1e+6);
    ode_clock->start();

    haptics_thread_running = 1;
    first_device_enabled = 1;

    DWORD thread_id;
    ::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(HapticLoop), this, 0, &thread_id);

    // Boost thread and process priority
    ::SetThreadPriority(&thread_id, THREAD_PRIORITY_ABOVE_NORMAL);
  }
  else
  {
    // tell the haptic thread to quit
    haptics_enabled = 0;
    first_device_enabled = 0;

    // If a second device is also running, stop it too
    second_device_enabled = 0;
    if (tool2)
    {
      tool2->setForcesOFF();
      tool2->stop();
    }

    flagSimulationOn = false;
  }
}
//---------------------------------------------------------------------------

// A global function for sticking a cube in the given mesh
// 
// Manually creates the 12 triangles (two per face) required to
// model a cube
void createCube(cMesh *mesh, float edge, int include_top) {

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

  if (include_top) {
  
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

  // Give a color to each vertex
  for (unsigned int i=0; i<mesh->getNumVertices(); i++) {

    cVertex* nextVertex = mesh->getVertex(i);

    cColorb color;
    color.set(
      GLuint(0xff*(edge + nextVertex->getPos().x ) / (2.0 * edge)),
      GLuint(0xff*(edge + nextVertex->getPos().y ) / (2.0 * edge)),
      GLuint(0xff* nextVertex->getPos().z / 2*edge)
      );

    nextVertex->setColor(color);
  }

  // Set object settings.  The parameters tell the object
  // to apply this alpha level to his textures and to his
  // children (of course he has neither right now).
  // object->setTransparencyLevel(0.5, true, true);

  // Give him some material properties...
  cMaterial material;

  material.m_ambient.set( 0.6, 0.3, 0.3, 1.0 );
  material.m_diffuse.set( 0.8, 0.6, 0.6, 1.0 );
  material.m_specular.set( 0.9, 0.9, 0.9, 1.0 );
  material.setShininess(100);
  mesh->m_material = material;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  flagSimulationOn = false;
  Sleep(1000);
  exit(0);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button3Click(TObject *Sender)
{
  if (!second_device_enabled)
  {
    // temporarily disable the first device, because the scheduler will have to be restarted
    first_device_enabled = 0;

    // create a phantom tool with its graphical representation
    //
    // Use device zero, and use either the gstEffect or direct 
    // i/o communication mode, depending on the USE_PHANTOM_DIRECT_IO
    // constant
    if (tool2 == 0)
    {
      // create a new tool and set its workspace
      tool2 = new cMeta3dofPointer(world, 1, USE_PHANTOM_DIRECT_IO);
      tool2->setWorkspace(3.0,3.0,3.0);

      // turn on ode proxy algorithm
      //tool2->m_proxyODEOn = true;
      world->addChild(tool2);       
      tool2->setRadius(0.05);
      tool2->computeGlobalCurrentObjectOnly(true);

      // Replace the proxy with our custom ODE proxy
      cProxyPointForceAlgo* old_proxy = (cProxyPointForceAlgo*)(tool2->m_pointForceAlgos[0]);
      tool2->m_pointForceAlgos[0] = new cODEProxy(old_proxy);
      delete old_proxy;
    }

    // set up the device
    tool2->initialize();

    // open communication to the device
    tool2->start();

    // tell the tool to show his coordinate frame so you
    // can see tool rotation
    tool2->visualizeFrames(true);

    // I need to call this so the tool can update its internal
    // transformations before performing collision detection, etc.
    tool2->computeGlobalPositions();
    tool2->setForcesON();

    Sleep(100);

    // The devices are enabled, but are not "ready" for forces until
    // the user has moved them into the relevant workspace
    tool1_ready = 0;
    tool2_ready = 0;

    first_device_enabled = 1;
    second_device_enabled = 1;
	
  } // enabling

  else
  {
    // tell the haptic thread to quit
    second_device_enabled = 0;
    
    // Stop the haptic device...
    tool2->setForcesOFF();
    tool2->stop();        
  } // disabling
}
//---------------------------------------------------------------------------

