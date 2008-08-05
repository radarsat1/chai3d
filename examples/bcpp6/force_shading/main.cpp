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
    light->setPos(cVector3d(4,1,0));

    // Get a pointer to the panel object's handle and create a viewport, telling it to render to that panel
    viewport = new cViewport(Panel1->Handle, camera, true);

    // Create an octagonal prism...
    object = new cMesh(world);
    float d = 1.0;

    // Vertices forming an octagon in y=-1 plane
    object->newVertex(0.924*d, -1, -0.383*d);
    cVector3d norm(2,0,-1); norm.normalize();
    object->getVertex(0)->setNormal(norm);
    object->newVertex(0.383*d, -1, -0.924*d);
    norm = cVector3d(1,0,-2); norm.normalize();
    object->getVertex(1)->setNormal(norm);
    object->newVertex(-0.383*d, -1, -0.924*d);
    norm = cVector3d(-1,0,-2); norm.normalize();
    object->getVertex(2)->setNormal(norm);
    object->newVertex(-0.924*d, -1, -0.383*d);
    norm = cVector3d(-2,0,-1); norm.normalize();
    object->getVertex(3)->setNormal(norm);
    object->newVertex(-0.924*d, -1, 0.383*d);
    norm = cVector3d(-2,0,1); norm.normalize();
    object->getVertex(4)->setNormal(norm);
    object->newVertex(-0.383*d, -1, 0.924*d);
    norm = cVector3d(-1,0,2); norm.normalize();
    object->getVertex(5)->setNormal(norm);
    object->newVertex(0.383*d, -1, 0.924*d);
    norm = cVector3d(1,0,2); norm.normalize();
    object->getVertex(6)->setNormal(norm);
    object->newVertex(0.924*d, -1, 0.383*d);
    norm = cVector3d(2,0,1); norm.normalize();
    object->getVertex(7)->setNormal(norm);

    // Vertices forming an octagon in y=1 plane
    object->newVertex(0.924*d, 1, -0.383*d);
    norm = cVector3d(2,0,-1); norm.normalize();
    object->getVertex(8)->setNormal(norm);
    object->newVertex(0.383*d, 1, -0.924*d);
    norm = cVector3d(1,0,-2); norm.normalize();
    object->getVertex(9)->setNormal(norm);
    object->newVertex(-0.383*d, 1, -0.924*d);
    norm = cVector3d(-1,0,-2); norm.normalize();
    object->getVertex(10)->setNormal(norm);
    object->newVertex(-0.924*d, 1, -0.383*d);
    norm = cVector3d(-2,0,-1); norm.normalize();
    object->getVertex(11)->setNormal(norm);
    object->newVertex(-0.924*d, 1, 0.383*d);
    norm = cVector3d(-2,0,1); norm.normalize();
    object->getVertex(12)->setNormal(norm);
    object->newVertex(-0.383*d, 1, 0.924*d);
    norm = cVector3d(-1,0,2); norm.normalize();
    object->getVertex(13)->setNormal(norm);
    object->newVertex(0.383*d, 1, 0.924*d);
    norm = cVector3d(1,0,2); norm.normalize();
    object->getVertex(14)->setNormal(norm);
    object->newVertex(0.924*d, 1, 0.383*d);
    norm = cVector3d(2,0,1); norm.normalize();
    object->getVertex(15)->setNormal(norm);

    // "Bottom" triangles of each face
    object->newTriangle(5,6,13);
    object->newTriangle(6,7,14);
    object->newTriangle(7,0,15);
    object->newTriangle(0,1,8);
    object->newTriangle(1,2,9);
    object->newTriangle(2,3,10);
    object->newTriangle(3,4,11);
    object->newTriangle(4,5,12);

    // "Top" triangles of each face
    object->newTriangle(14,13,6);
    object->newTriangle(15,14,7);
    object->newTriangle(8,15,0);
    object->newTriangle(9,8,1);
    object->newTriangle(10,9,2);
    object->newTriangle(11,10,3);
    object->newTriangle(12,11,4);
    object->newTriangle(13,12,5);

    // Draw all triangles
    object->useCulling(false, true);

    // Show the normals we set
    object->showNormals(true, true);
    object->setNormalsProperties(1.0, cColorf(1.0,0,0), true);

    // Add the octagonal prism to the world
    world->addChild(object);

    // Create a soft, green material for the object
    material1.setStiffness(3.0);
    material1.m_ambient.set(0.0, 0.8, 0.0, 1.0);
    material1.m_diffuse.set(0.0, 0.8, 0.0, 1.0);
    material1.m_specular.set(1.0, 1.0, 1.0, 1.0);
    material1.setShininess(100.0);
    object->useMaterial(true);
    object->m_material = material1;

    // create a tool and add it to the world.
    tool = new cMeta3dofPointer(world, 0);
    world->addChild(tool);
    tool->setPos(0.0, 0.0, 0.0);

    // Rotate the tool so its axes align with our opengl-like axes
    tool->rotate(cVector3d(0,0,1), -90.0*3.14159/180.0);
    tool->rotate(cVector3d(1,0,0), -90.0*3.14159/180.0);
    tool->setRadius(0.05);

    // set up the device
    tool->initialize();

    // Replace the default point force algorithms with our force-shading proxy
    tool->m_pointForceAlgos.clear();
    cForceShadingProxy* shading_proxy = new cForceShadingProxy();
    new_proxy = shading_proxy;
    shading_proxy->initialize(world, cVector3d(0,0,0));
    shading_proxy->setProxyRadius(0.05*0.001);
    tool->m_pointForceAlgos.push_back(shading_proxy);

    // set up a nice-looking workspace for the tool so it fits nicely with our
    // cube models we will be builing
    tool->setWorkspace(1.0,1.0,1.0);

    // tell the tool to show his coordinate frame so you can see tool position and rotation
    // define the size of the frame as well
    tool->setToolFrame(true, 0.1);

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
    // Set whether we are using force shading according to GUI checkbox
    if (new_proxy)
    {
        if (UseForceShading->Checked)
            new_proxy->setUseShading(1);
        else
            new_proxy->setUseShading(0);
    }

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


void __fastcall TForm1::Button1Click(TObject *Sender)
{
    exit(0);
}
//---------------------------------------------------------------------------


