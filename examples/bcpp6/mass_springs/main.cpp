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

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include "CElapsed.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;

// Choose collision detection algorithm --
// 0 for axis-aligned bounding boxes, 1 for sphere trees
int collisions = 0;
CElapsed g_timer;
double g_last_iteration_time = -1.0;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void HapticLoop();

void __fastcall TForm1::MassSpringFormCreate(TObject *Sender)
{
    m_floor = 0;
    m_floor_spring_constant = DEFAULT_FLOOR_SPRING_CONSTANT;

    // init world scale
    scale = 2.0;

    // create a world
    world = new cWorld();

    // set background properties
    world->setBackgroundColor(0.2,0.2,0.2);

    // create a camera in world
    camera  = new cCamera(world);
    world->addChild(camera);

    // set camera position and orientation
    camera->set(   cVector3d(0.0, 0.0, 7.0),   // position of camera.
                   cVector3d(0.0, 0.0, 0.0),   // camera looking at origin.
                   cVector3d(0.0, 1.0, 0.0));  // orientation of camera. (standing up.)

    // Create a light source and attach it to camera
    light = new cLight(world);
    camera->addChild(light);
    light->setEnabled(true);
    light->setPos(cVector3d(0,2,4));
    light->rotate(cVector3d(0,0,1), cDegToRad(180));

    // create a display for graphic rendering
    viewport = new cViewport(Panel1->Handle, camera, false);

    // create a tool
    tool = new cMeta3dofPointer(world, 0, true);
    tool->initialize();
    tool->setRenderingMode(RENDER_DEVICE);

    // Rotate the tool so its axes align with our opengl-like axes
    tool->rotate(cVector3d(0,0,1),-90.0*M_PI/180.0);
    tool->rotate(cVector3d(1,0,0),-90.0*M_PI/180.0);
    tool->computeGlobalPositions();

    tool->setWorkspace(3.5*scale, 3.5*scale, 3.5*scale);
    tool->setRadius(scale / 70.0);

    // tool becomes a child of the camera which we can control from
    world->addChild(tool);

    // Create a mesh to represent the floor
    m_floor = new cMesh(world);
    world->addChild(m_floor);

    // Fill in meaningful vertex positions
    m_floor->newVertex(-FLOOR_X_SIZE/2.0,  FLOOR_Y_POSITION, -FLOOR_Z_SIZE/2.0);
    m_floor->newVertex(-FLOOR_X_SIZE/2.0,  FLOOR_Y_POSITION,  FLOOR_Z_SIZE/2.0);
    m_floor->newVertex( FLOOR_X_SIZE/2.0,  FLOOR_Y_POSITION,  FLOOR_Z_SIZE/2.0);
    m_floor->newTriangle(0,1,2);

    m_floor->newVertex( FLOOR_X_SIZE/2.0,  FLOOR_Y_POSITION, -FLOOR_Z_SIZE/2.0);
    m_floor->newVertex(-FLOOR_X_SIZE/2.0,  FLOOR_Y_POSITION, -FLOOR_Z_SIZE/2.0);
    m_floor->newVertex( FLOOR_X_SIZE/2.0,  FLOOR_Y_POSITION,  FLOOR_Z_SIZE/2.0);
    m_floor->newTriangle(3,4,5);

    for(int n=0; n<6; n++)
    {
      cVertex* curVertex = m_floor->getVertex(n);
      curVertex->setNormal(0,1,0);
    }

    // Give him some material properties...
    cMaterial material;
    material.m_ambient.set( 0.2, 0.2, 0.2, 1.0 );
    material.m_diffuse.set( 0.6, 0.6, 0.6, 1.0 );
    material.m_specular.set( 0.9, 0.9, 0.9, 1.0 );
    material.setShininess(100);
    m_floor->m_material = material;

    // Create an initial ball
    CBall* b = new CBall();
    m_active_balls.push_back(b);
    cVector3d pos(-1.0,0,1.0);
    b->setPos(pos);
    world->addChild(b);

    // Create a series of masses connected by springs
    for(int i=1; i<INITIAL_NUM_BALLS; i++) {
      add_ball();
    }
    simulationOn = false;
}
//---------------------------------------------------------------------------

// Add a mass to or remove a mass from the end of the chain
void TForm1::add_ball() {

  // Put this ball to the right of the current last ball
  CBall* b = new CBall();
  m_active_balls.push_back(b);

  int ball_index = m_active_balls.size() - 1;

  CBall* neighbor = m_active_balls[ball_index - 1];

  // Create a position for each ball, moving from left to right
  // with increasing i
  cVector3d pos = neighbor->getPos();
  pos.add(INITIAL_BALL_SPACING,0,0);
  b->setPos(pos);
  world->addChild(b);

  CSpring* s = new CSpring();
  m_active_springs.push_back(s);

  s->m_endpoint_1 = m_active_balls[ball_index];
  s->m_endpoint_2 = m_active_balls[ball_index-1];

  s->m_endpoint_1->m_springs.push_back(s);
  s->m_endpoint_2->m_springs.push_back(s);

  // Set the spring's rest length to be the initial distance between
  // the balls
  double d = cDistance(s->m_endpoint_1->getPos(),s->m_endpoint_2->getPos());
  s->m_rest_length = d;
  world->addChild(s);
}


//---------------------------------------------------------------------------

void TForm1::initSettings()
{
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{

    // render world in display
    if (viewport != NULL)
    {
        viewport->render();
    }
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Panel1Resize(TObject *Sender)
{
    // render world in display
    if (viewport != NULL)
    {
        viewport->render();
    }
}

//---------------------------------------------------------------------------

int g_haptic_iteration = 0;

// haptic loop reads device position, computes, and applies forces to device
void HapticLoop()
{
    Form1->tool->start();
    Form1->tool->setForcesON();
    g_haptic_iteration = 0;

    // init ball velocity
    // Clear the force that's applied to each ball
    g_last_iteration_time = g_timer.GetTime();
    for(unsigned int i=0; i<Form1->m_active_balls.size(); i++)
    {
        Form1->m_active_balls[i]->m_velocity.zero();
    }

    while(simulationOn)
    {
        Form1->tool->updatePose();
        Form1->compute_spring_forces();

        if (g_haptic_iteration) Form1->tool->m_lastComputedGlobalForce = Form1->m_haptic_force;
        else Form1->tool->m_lastComputedGlobalForce.set(0,0,0);
        g_haptic_iteration++;

        Form1->tool->applyForces();
    }

    Form1->tool->stop();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::ToggleHapticsButtonClick(TObject *Sender)
{
    // create a thread for the haptic loop
    simulationOn = !simulationOn;
    if (simulationOn)
    {
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(HapticLoop), NULL, NULL, &ThreadId);
        SetThreadPriority(&ThreadId, THREAD_PRIORITY_ABOVE_NORMAL);
    }
}

//---------------------------------------------------------------------------

// Method definitions for the mass/spring objects
CSpring::CSpring()
{
    m_radius = 0.05;
    m_rest_length = 1.0;
    m_spring_constant = DEFAULT_SPRING_CONSTANT;
    m_endpoint_1 = 0;
    m_endpoint_2 = 0;
}

//---------------------------------------------------------------------------

CSpring::~CSpring()
{
}

//---------------------------------------------------------------------------

// Render the spring graphically
void CSpring::render(const int a_renderMode)
{
    if (m_endpoint_1 == 0 || m_endpoint_2 == 0) return;

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.8,0.2,0.2);

    glLineWidth(4.0);

    // Use a line for now, maybe a cylinder later...
    glBegin(GL_LINES);
    cVector3d v1 = m_endpoint_1->getPos();
    cVector3d v2 = m_endpoint_2->getPos();
    glVertex3f(v1.x,v1.y,v1.z);
    glVertex3f(v2.x,v2.y,v2.z);
    glEnd();
}

//---------------------------------------------------------------------------

// A ball (rendered as a sphere) that can be pulled around by
// springs
CBall::CBall()
{
    m_radius = 0.2;
    m_velocity.set(0,0,0);
    m_mass = DEFAULT_OBJECT_MASS;
}

//---------------------------------------------------------------------------

CBall::~CBall()
{
}

//---------------------------------------------------------------------------

void CBall::render (const int a_renderMode)
{
    static GLUquadricObj *sphereObj = 0;

    if (sphereObj == 0) sphereObj = gluNewQuadric();

    // Render a sphere at the location of this ball
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.8,0.8,0.5);

    #define BALL_SLICES 36
    #define BALL_STACKS 36

    gluSphere(sphereObj, m_radius, BALL_SLICES, BALL_STACKS);
}

//---------------------------------------------------------------------------

// Called from the haptics loop if it's running (from the
// graphics loop otherwise) to compute and integrate forces
//
// Updates the position and velocity of all the masses

void TForm1::compute_spring_forces()
{
    double curtime = g_timer.GetTime();

    if (g_last_iteration_time < 0)
    {
      g_last_iteration_time = curtime;
      return;
    }

    double elapsed = curtime - g_last_iteration_time;
    g_last_iteration_time = curtime;

    unsigned int i;

    // Clear the force that's applied to each ball
    for(i=0; i<m_active_balls.size(); i++)
    {
        m_active_balls[i]->current_force.set(0,0,0);
    }

    if (simulationOn)
    {
        CBall* b = m_active_balls[0];
        cVector3d old_p = b->getPos();
        b->setPos(tool->m_deviceGlobalPos);
        b->m_velocity = cDiv(elapsed,cSub(b->getPos(),old_p));
    }

    // Compute the current length of each spring and apply forces
    // on each mass accordingly
    for(i=0; i<m_active_springs.size(); i++)
    {
        CSpring* s = m_active_springs[i];

        double d = cDistance(s->m_endpoint_1->getPos(),s->m_endpoint_2->getPos());
        s->m_current_length = d;

        // This spring's deviation from its rest length
        //
        // (positive deviation -> spring is too long)
        double x = s->m_current_length - s->m_rest_length;

        // Apply a force to ball 1 that pulls it toward ball 2
        // when the spring is too long
        cVector3d f1 = cMul(s->m_spring_constant*x*1.0,
          cSub(s->m_endpoint_2->getPos(),s->m_endpoint_1->getPos()));
        s->m_endpoint_1->current_force.add(f1);

        // Add the opposite force to ball 2
        s->m_endpoint_2->current_force.add(cMul(-1.0,f1));
    }

    // Update velocities and positions based on forces
    for(i=0; i<m_active_balls.size(); i++)
    {
        CBall* b = m_active_balls[i];

        // Certain forces don't get applied to the "haptic ball"
        // when haptics are enabled...
        if (simulationOn == 0 || i != 0) {
          cVector3d f_damping = cMul(DAMPING_CONSTANT,b->m_velocity);
          b->current_force.add(f_damping);
        }

        cVector3d f_gravity(0,GRAVITY_CONSTANT*b->m_mass,0);
        b->current_force.add(f_gravity);

        cVector3d p = b->getPos();

        if (p.y - b->m_radius < FLOOR_Y_POSITION) {
          double penetration = FLOOR_Y_POSITION - (p.y - b->m_radius);
          b->current_force.add(0,m_floor_spring_constant*penetration,0);
        }

        cVector3d f_floor(0,0,0);

        cVector3d a = cDiv(b->m_mass,b->current_force);

        b->m_velocity.add(cMul(elapsed,a));

        // We handle the 0th ball specially when haptics is enabled
        if (simulationOn == 0 || i != 0) {
          p.add(cMul(elapsed,b->m_velocity));
          b->setPos(p);
        }
    }

    // Set the haptic force appropriately to reflect the force
    // applied to ball 0
    m_haptic_force = cMul(HAPTIC_FORCE_CONSTANT,m_active_balls[0]->current_force);
}

//---------------------------------------------------------------------------

void __fastcall TForm1::MassSpringFormClose(TObject *Sender,
      TCloseAction &Action)
{
    simulationOn = 0;

    std::vector<CBall*>::iterator i1;
    for(i1 = m_active_balls.begin(); i1 != m_active_balls.end(); i1++)
    {
        CBall* b = (*i1);
        delete b;
    }
    m_active_balls.clear();

    std::vector<CSpring*>::iterator i2;
    for(i2 = m_active_springs.begin(); i2 != m_active_springs.end(); i2++)
    {
        CSpring* s = (*i2);
        delete s;
    }
    m_active_springs.clear();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
    exit(0);
}

//---------------------------------------------------------------------------

