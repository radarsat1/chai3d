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
    light->setPos(cVector3d(0,1,4));

    // Get a pointer to the panel object's handle and create a viewport, telling it to render to that panel
    viewport = new cViewport(Panel1->Handle, camera, true);

    // Load a gear mesh from a .3DS file
    object = new cMesh(world);
    object->loadFromFile("resources\\models\\small_gear.3ds");

    // Scale the object to fit nicely in our viewport
    // compute size of object
    object->computeBoundaryBox(true);

    cVector3d min = object->getBoundaryMin();
    cVector3d max = object->getBoundaryMax();

    // This is the "size" of the object
    cVector3d span = cSub(max, min);
    double size = cMax(span.x, cMax(span.y, span.z));

    // We'll center all vertices, then multiply by this amount,
    // to scale to the desired size.
    double scaleFactor = 2.0 / size;
    object->scale(scaleFactor);

    // Tell him to compute a bounding box...
    object->computeBoundaryBox(true);
					
    // Build a nice collision-detector for this object
    object->createSphereTreeCollisionDetector(true,true);
	
    // Automatically compute normals for all triangles
    object->computeAllNormals();

    // Translate and rotate so that the airplane is flying towards the right of the screen
    object->translate(0.7, 0.0, 0.0);
    object->rotate(cVector3d(0,1,0),-90.0 * 3.14159 / 180.0);
    object->rotate(cVector3d(1,0,0),-30.0 * 3.14159 / 180.0);
    object->computeGlobalPositions(false);

    // Use vertex colors so we can see which triangles collide
	  object->useColors(true, true);
				 
    // Add the mesh object to the world
    world->addChild(object);

    // Create a new tool with this mesh
    tool = new cMeshTool(world, 0, true);
    world->addChild(tool);

    // Load a gear mesh from a .3DS file
    tool_object = new cMesh(world);
    tool_object->loadFromFile("resources\\models\\small_gear.3ds");
    tool_object->computeGlobalPositions(false);

    // Scale the object to fit nicely in our viewport
    // compute size of object
    tool_object->computeBoundaryBox(true);

    min = tool_object->getBoundaryMin();
    max = tool_object->getBoundaryMax();

    // This is the "size" of the object
    span = cSub(max, min);
    size = cMax(span.x, cMax(span.y, span.z));

    // We'll center all vertices, then multiply by this amount,
    // to scale to the desired size.
    scaleFactor = 2.0 / size;
    tool_object->scale(scaleFactor);

    // Create a sphere tree bounding volume hierarchy for collision detection on this mesh
    tool_object->createSphereTreeCollisionDetector(true, true);

    // Use vertex colors so we can see which triangles collide
	  tool_object->useColors(true, true);

    // Add the mesh object to the world
    world->addChild(tool_object);

    // Set the mesh for this tool
    tool->setMesh(tool_object);

    // Tell the tool to search for collisions with this mesh
    tool->addCollisionMesh(object);

    // Set up the device
    tool->initialize();

    // Set up a nice-looking workspace for the phantom so
    // it fits nicely with our shape
    tool->setWorkspace(2.0, 2.0, 2.0);

    // Rotate the tool so its axes align with our opengl-like axes
    tool->rotate(cVector3d(0,0,1), -90.0*3.14159/180.0);
    tool->rotate(cVector3d(1,0,0), -90.0*3.14159/180.0);
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
    if (tool)
		    if (tool->m_collisionDetector)
			      tool->m_collisionDetector->setFindAll(FindAllCheck->Checked);

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

        // no forces in this example
        //Form1->tool->applyForces();
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


void __fastcall TForm1::Button1Click(TObject *Sender)
{
    exit(0);
}
//---------------------------------------------------------------------------


