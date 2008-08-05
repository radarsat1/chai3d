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

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "main.h"
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TForm1 *Form1;
void HapticLoop();
//---------------------------------------------------------------------------

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{

}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    // Create a world and set a white background color
    world = new cWorld();
    world->setBackgroundColor(1.0,1.0,1.0);

    // Create a camera and set its position, look-at point, and orientation (up-direction)
    camera = new cCamera(world);
    int result = camera->set(cVector3d(0,0,4), cVector3d(0,0,0), cVector3d(0,1,0));

    // Create, enable, and position a light source
    light = new cLight(world);
    light->setEnabled(true);
    light->setPos(cVector3d(2,2,4));

    // create a display for graphic rendering
    viewport = new cViewport(Panel1->Handle, camera, true);

    // Create and position an implicit sphere
    sphere = new cImplicitSphere(0.4);
    sphere->translate(1.6, 0.9, 0.0);
    world->addChild(sphere);

    // Create and position an implicit cone
    cone = new cImplicitCone(0.5);
    cone->rotate(cVector3d(1,0,0), -3.14159/2.0);
    world->addChild(cone);

    // Create and position an implicit cylinder
    cylinder = new cImplicitCylinder(0.4);
    cylinder->rotate(cVector3d(1,0,0), -3.14159/2.0);
    cylinder->translate(-1.5, 0.0, 0.0);
    world->addChild(cylinder);

    // Create and position an implicit torus
    torus = new cImplicitTorus(0.4, 0.2);
    torus->translate(1.6, -0.9, 0.0);
    world->addChild(torus);

    // Initilize some variables
    world->computeGlobalPositions();

    // create a tool and add it to the world.
    tool = new cMeta3dofPointer(world, 0);
    world->addChild(tool);
    tool->setPos(0.0, 0.0, 0.0);

    tool->rotate(cVector3d(0,0,1), -90.0*3.14159/180.0);
    tool->rotate(cVector3d(1,0,0), -90.0*3.14159/180.0);

    // set up a nice-looking workspace for the tool so it fits nicely with our
    // cube models we will be builing
    tool->setWorkspace(1.0,1.0,1.0);

    // tell the tool to show his coordinate frame so you can see tool position and rotation
    // define the size of the frame as well
    tool->setToolFrame(true, 0.1);

    // set the diameter of the ball representing the tool
    tool->setRadius(0.05);

    // don't start the haptics loop yet
    flagSimulationOn = false;
    flagHasExitedSimulation = true;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
    // stop simulation
    flagSimulationOn = false;
    while (!flagHasExitedSimulation) { Sleep(1); }

    // stop graphic rendering
    Timer1->Enabled = false;

    // cleanup memory
    delete world;
    delete viewport;
}

//---------------------------------------------------------------------------



void __fastcall TForm1::Timer1Timer(TObject *Sender)
{

    // Set the stiffnesses and friction coefficients based on current slider values
    if (cone) {
        cone->m_material.setStiffness(1.0*Stiffness->Position);
        cone->m_material.setStaticFriction(0.01*StaticFriction->Position);
        cone->m_material.setDynamicFriction(0.01*DynamicFriction->Position);
    }
    if (cylinder) {
        cylinder->m_material.setStiffness(1.0*Stiffness->Position);
        cylinder->m_material.setStaticFriction(0.01*StaticFriction->Position);
        cylinder->m_material.setDynamicFriction(0.01*DynamicFriction->Position);
    }
    if (sphere) {
        sphere->m_material.setStiffness(1.0*Stiffness->Position);
        sphere->m_material.setStaticFriction(0.01*StaticFriction->Position);
        sphere->m_material.setDynamicFriction(0.01*DynamicFriction->Position);
    }
    if (torus) {
        torus->m_material.setStiffness(1.0*Stiffness->Position);
        torus->m_material.setStaticFriction(0.01*StaticFriction->Position);
        torus->m_material.setDynamicFriction(0.01*DynamicFriction->Position);
    }

    // Render world in display
    if (viewport != NULL)
    {
        viewport->render();
    }
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
        // read position from haptic device
        Form1->tool->updatePose();

        // compute forces
        Form1->tool->computeForces();

        // detect which cubes that is being touched by the tool and modify its material property
        cProxyPointForceAlgo* proxy = Form1->tool->getProxy();
        cMesh* object = (cMesh*)proxy->getContactObject();
        if (object != Form1->lastObject)
        {
            if (Form1->lastObject != NULL)
            {
                Form1->lastObject->m_material = Form1->materialGray;
                Form1->lastObject = NULL;
            }
            if (object != NULL)
            {
                object->m_material = Form1->materialRed;
                Form1->lastObject = object;
            }
        }

        // send forces to haptic device
        Form1->tool->applyForces();
    }

    // stop haptics
    Form1->tool->stop();
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
        // set up the device
        tool->initialize();

        // open communication to the device
        tool->start();

        // I need to call this so the tool can update its internal
        // transformations before performing collision detection, etc.
        tool->computeGlobalPositions();
        tool->setForcesON();

        DWORD thread_id;
        ::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(HapticLoop), this, 0, &thread_id);

        // Boost thread and process priority
        ::SetThreadPriority(&thread_id, THREAD_PRIORITY_ABOVE_NORMAL);
    }
    else
    {
        flagSimulationOn = false;
    }
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------




void __fastcall TForm1::Button1Click(TObject *Sender)
{
    exit(0);
}
//---------------------------------------------------------------------------


