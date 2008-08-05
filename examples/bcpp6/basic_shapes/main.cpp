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
    \author:    Francois Conti
    \version    1.1
    \date       06/2004
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

const double MESH_SCALE_SIZE = 1.0;

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{

}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    // create a new world
    world = new cWorld();

    // set background color
    world->setBackgroundColor(0.0f,0.0f,0.0f);

    // Create a camera
    camera = new cCamera(world);
    world->addChild(camera);

    // Create a light source and attach it to camera
    light = new cLight(world);
    light->setEnabled(true);
    light->setPos(cVector3d(2,1,1));

    // define camera position
    cameraAngleH = 10;
    cameraAngleV = 20;
    cameraDistance = 2.0;
    flagCameraInMotion = false;
    updateCameraPosition();
    camera->setClippingPlanes(0.01, 10.0);

    // create a display for graphic rendering
    viewport = new cViewport(Panel1->Handle, camera, true);
    viewport->setStereoOn(false);

    // create a mesh - we will build a simple cube, and later let the
    // user load 3d models
    object = new cMesh(world);
    world->addChild(object);

    // create a nice little cube
    createCube(object, 0.05);

    // display information about object
    NumVerticesLabel->Caption = object->getNumVertices(true);
    NumTrianglesLabel->Caption = object->getNumTriangles(true);

    // define a material property for this object
    cMaterial material;
    material.m_ambient.set( 0.4, 0.2, 0.2, 1.0 );
    material.m_diffuse.set( 0.8, 0.6, 0.6, 1.0 );
    material.m_specular.set( 0.9, 0.9, 0.9, 1.0 );
    material.setShininess(100);
    object->m_material = material;

    // update camera position
    updateCameraPosition();

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

void TForm1::updateCameraPosition()
{
    // check values
    if (cameraDistance < 0.1) { cameraDistance = 0.1; }
    if (cameraAngleV > 89) { cameraAngleV = 89; }
    if (cameraAngleV < -89) { cameraAngleV = -89; }

    // compute position of camera in space
    cVector3d pos = cAdd(
                        cameraPosition,
                        cVector3d(
                            cameraDistance * cCosDeg(cameraAngleH) * cCosDeg(cameraAngleV),
                            cameraDistance * cSinDeg(cameraAngleH) * cCosDeg(cameraAngleV),
                            cameraDistance * cSinDeg(cameraAngleV)
                        )
                    );

    // compute lookat position
    cVector3d lookat = cameraPosition;

    // define role orientation of camera
    cVector3d up(0.0, 0.0, 1.0);

    // set new position to camera
    camera->set(pos, lookat, up);

    // recompute global positions
    world->computeGlobalPositions(true);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Timer1Timer(TObject *Sender)
{

    // update camera position
    updateCameraPosition();

    // render world in display
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
        // create tool
        if (tool == NULL)
        {
            tool = new cMeta3dofPointer(world, 0);
            camera->addChild(tool);
            tool->setPos(-2.0, 0.0, 0.0);

            // set up a nice-looking workspace for the phantom so
            // it fits nicely with our models
            tool->setWorkspace(2.0,2.0,2.0);

            // Rotate the tool so its axes align with our opengl-like axes
            tool->setRadius(0.02);
            tool->setFrameSize(0.04);
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
        tool->setForcesON();

        // Enable the "dynamic proxy", which will handle moving objects
        cProxyPointForceAlgo* proxy = tool->getProxy();
        proxy->enableDynamicProxy(true);

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

void createCube(cMesh *a_mesh, double a_halfSize)
{
    // define eight corners of edges [x,y,z]
    cVector3d p000(-a_halfSize, -a_halfSize, -a_halfSize);
    cVector3d p100( a_halfSize, -a_halfSize, -a_halfSize);
    cVector3d p110( a_halfSize,  a_halfSize, -a_halfSize);
    cVector3d p010(-a_halfSize,  a_halfSize, -a_halfSize);
    cVector3d p001(-a_halfSize, -a_halfSize,  a_halfSize);
    cVector3d p101( a_halfSize, -a_halfSize,  a_halfSize);
    cVector3d p111( a_halfSize,  a_halfSize,  a_halfSize);
    cVector3d p011(-a_halfSize,  a_halfSize,  a_halfSize);

    // create two triangles for each face:

    //face -x
    a_mesh->newTriangle(p000, p001, p011);
    a_mesh->newTriangle(p000, p011, p010);

    //face +x
    a_mesh->newTriangle(p100, p110, p111);
    a_mesh->newTriangle(p100, p111, p101);

    //face -y
    a_mesh->newTriangle(p000, p100, p101);
    a_mesh->newTriangle(p000, p101, p001);

    //face +y
    a_mesh->newTriangle(p010, p011, p111);
    a_mesh->newTriangle(p010, p111, p110);

    //face -z
    a_mesh->newTriangle(p000, p010, p100);
    a_mesh->newTriangle(p010, p110, p100);

    //face +z
    a_mesh->newTriangle(p001, p101, p011);
    a_mesh->newTriangle(p101, p111, p011);

    // compute normals
    a_mesh->computeAllNormals(true);

    // compute boudary box
    a_mesh->computeBoundaryBox(true);
}
//---------------------------------------------------------------------------







void __fastcall TForm1::Panel1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    flagCameraInMotion = true;
    mouseX = X;
    mouseY = Y;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Panel1MouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
    if (flagCameraInMotion)
    {
        if (Shift.Contains(ssLeft))
        {
            cameraAngleH = cameraAngleH - (X - mouseX);
            cameraAngleV = cameraAngleV + (Y - mouseY);
        }
        else if (Shift.Contains(ssRight))
        {
            cameraDistance = cameraDistance - 0.01 * (Y - mouseY);
        }
    }

    updateCameraPosition();

    mouseX = X;
    mouseY = Y;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Panel1MouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
    flagCameraInMotion = false;
}

//---------------------------------------------------------------------------


