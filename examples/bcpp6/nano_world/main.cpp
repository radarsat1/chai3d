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

const double MESH_SCALE_SIZE = 0.3;

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

    // create a camera and add it to the world
    camera = new cCamera(world);
    world->addChild(camera);

    // set the Near and Far clipping planes of the camera
    camera->setClippingPlanes(0.001, 100.0);

    // define a default position of the camera (described in spherical coordinates)
    cameraAngleH = -90;
    cameraAngleV = 45;
    cameraDistance = 0.5;
    updateCameraPosition();

    // Create a light source and attach it to camera
    light = new cLight(world);
    light->setEnabled(true);
    light->setPos(cVector3d(0,0,0));
    light->setDir(cVector3d(-1,0,0));
    camera->addChild(light);

    // create a display for graphic rendering
    viewport = new cViewport(Panel1->Handle, camera, true);

    // initialize a few flags
    flagSimulationOn = false;       // the simulation in OFF
    flagHasExitedSimulation = true;
    flagCameraInMotion = false;     // the camera is currently not being moved by the operator (mouse)

    // load default map
    loadHeightMap("resources\\images\\nano2.bmp");

    // start graphic rendering callback
    Timer1->Enabled = true;

    // start the haptics loop
    btnToggleHaptics->Click();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
    // stop simulation
    flagSimulationOn = false;

    // wait until the haptics thread exits
    while (!flagHasExitedSimulation) { Sleep(100); }

    // stop graphic rendering
    Timer1->Enabled = false;
}

//---------------------------------------------------------------------------

void TForm1::updateCameraPosition()
{
    // check values of camera angles
    if (cameraDistance < 0.3) { cameraDistance = 0.3; }
    if (cameraDistance > 1.0) { cameraDistance = 1.0; }
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

    // compute the lookat position
    cVector3d lookat = cameraPosition;

    // define the orientation of camera
    cVector3d up(0.0, 0.0, 1.0);

    // set new position to camera
    camera->set(pos, lookat, up);

    // recompute global positions of each object in the world
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
    // simulation in now turned ON...
    flagSimulationOn = true;

    // ...and simulation has not yet exited.
    flagHasExitedSimulation = false;

    // here is the main haptics loop
    while(flagSimulationOn)
    {
        // read position from haptic device
        Form1->tool->updatePose();

        // compute forces
        Form1->tool->computeForces();

        // send forces to haptic device
        Form1->tool->applyForces();
    }

    // simulation in now turned ON...
    flagSimulationOn = false;

    // simulation has now terminated
    flagHasExitedSimulation = true;

    // the simulation exits. we stop the haptics tool
    Form1->tool->stop();

    // exit thread
    ExitThread(0);
}

//---------------------------------------------------------------------------

void __fastcall TForm1::LoadModelButtonClick(TObject *Sender)
{
    // stop current simulation
    flagSimulationOn = false;

    // wait until simulation exits
    while (!flagHasExitedSimulation)
    {
        Sleep(1);
    }

    // open new object
    if (OpenPictureDialog1->Execute())
    {
        loadHeightMap(OpenPictureDialog1->FileName);
    }

    // start the haptics loop
    btnToggleHaptics->Click();
}

//---------------------------------------------------------------------------

bool TForm1::loadHeightMap(String a_filename)
{
    // create a new mesh
    cMesh* newObject = new cMesh(world);

    // create a texture file
    newTexture = new cTexture2D();
    world->addTexture(newTexture);

    // texture 2D
    newTexture->loadFromFile(a_filename.c_str());

    // get the size of the texture image (U and V)
    int texSizeU = newTexture->m_image.getWidth();
    int texSizeV = newTexture->m_image.getHeight();

    // check size of image
    if ((texSizeU < 1) || (texSizeV < 1)) { return (false); }

    // we look for the largest side
    int largestSide;
    if (texSizeU > texSizeV)
    {
        largestSide = texSizeU;
    }
    else
    {
        largestSide = texSizeV;
    }

    // The largest side of the map has a length of 1.0
    // we now compute the respective size for 1 pixel of the image in world space.
    double size = 1.0 / (double)largestSide;

    // we will create an triangle based object. For centering puposes we
    // compute an offset for axis X and Y corresponding to the half size
    // of the image map.
    double offsetU = 0.5 * (double)texSizeU * size;
    double offsetV = 0.5 * (double)texSizeV * size;

    // For each pixel of the image, create a vertex
    int u,v;
    for (v=0; v<texSizeV; v++)
    {
        for (u=0; u<texSizeU; u++)
        {
            double px, py, tu, tv;

            // compute the height of the vertex
            cColorb color = newTexture->m_image.getPixelColor(u,v);
            double height = 0.1 * ((double)color.getR() + (double)color.getG() + (double)color.getB()) / (3.0 * 255.0);

            // compute the position of the vertex
            px = size * (double)u - offsetU;
            py = size * (double)v - offsetV;

            // create new vertex
            unsigned int index = newObject->newVertex(px, py, height);
            cVertex* vertex = newObject->getVertex(index);

            // compute texture coordinate
            tu = (double)u / texSizeU;
            tv = (double)v / texSizeV;
            vertex->setTexCoord(tu, tv);
        }
    }

    // Create a triangle based map using the above pixels
     for (v=0; v<(texSizeV-1); v++)
    {
        for (u=0; u<(texSizeU-1); u++)
        {
            // get the indexing numbers of the next four vertices
            unsigned int index00 = ((v + 0) * texSizeU) + (u + 0);
            unsigned int index01 = ((v + 0) * texSizeU) + (u + 1);
            unsigned int index10 = ((v + 1) * texSizeU) + (u + 0);
            unsigned int index11 = ((v + 1) * texSizeU) + (u + 1);

            // create two new triangles
            newObject->newTriangle(index00, index01, index10);
            newObject->newTriangle(index10, index01, index11);
        }
    }

    // apply texture to object
    newObject->setTexture(newTexture);
    newObject->useTexture(true);

    // compute normals
    newObject->computeAllNormals(true);

    // compute size of object
    newObject->computeBoundaryBox(true);

    cVector3d min = newObject->getBoundaryMin();
    cVector3d max = newObject->getBoundaryMax();

    // This is the "size" of the object
    cVector3d span = cSub(max, min);
    size = cMax(span.x, cMax(span.y, span.z));

    // We'll center all vertices, then multiply by this amount,
    // to scale to the desired size.
    double scaleFactor = MESH_SCALE_SIZE / size;
    newObject->scale(scaleFactor);

    // compute size of object again
    newObject->computeBoundaryBox(true);

    // Build a collision-detector for this object, so
    // the proxy will work nicely when haptics are enabled.

    newObject->createAABBCollisionDetector(true, false);

    // set size of frame
    newObject->setFrameSize(0.2, true);

    // set size of normals
    newObject->setNormalsProperties(0.01, cColorf(1.0, 0.0, 0.0, 1.0), true);

    // render graphically both sides of triangles
    newObject->useCulling(false);

    // update global position
    newObject->computeGlobalPositions();

    // set the haptic stiffness of the object
    newObject->setStiffness(300);

    // use display lists
    newObject->useDisplayList(true);

    // remove previous object from world and add new one
    world->removeChild(object);
    delete object;
    object = newObject;
    world->addChild(object);

    // load image in right panel
    Image1->Picture->LoadFromFile(a_filename.c_str());
    Image1->Stretch = true;
    Panel2->Height =  (int)(((double)texSizeV / (double)texSizeU) * (double)Panel2->Width);
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
            tool->setPos(-0.55, 0.0, 0.03);

            // set up a nice-looking workspace for the phantom so
            // it fits nicely with our models
            tool->setWorkspace(0.8,0.8,0.8);

            // Rotate the tool so its axes align with our opengl-like axes
            tool->setRadius(0.002);
            tool->setFrameSize(0.04);
            tool->setShowFrame(false, true);
            tool->setRenderingMode(RENDER_PROXY);
        }

        // set up the device
        tool->initialize();

        // open communication to the device
        tool->start();

        // tell the tool to show his coordinate frame so you
        // can see tool rotation
        tool->visualizeFrames(true);

        // I need to call this so the tool can update its internal
        // transformations before performing collision detection, etc.
        tool->computeGlobalPositions();
        tool->setForcesON();

        // Enable the "dynamic proxy", which will handle moving objects
        cProxyPointForceAlgo* proxy = tool->getProxy();
        //proxy->enableDynamicProxy(true);

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

void __fastcall TForm1::Button3Click(TObject *Sender)
{
    Sleep(100);
    flagSimulationOn = false;
    while (!flagHasExitedSimulation) { Sleep(100); }
    Close();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Panel1Resize(TObject *Sender)
{
    if (object != NULL)
    {
        object->invalidateDisplayList();
    }
}

//---------------------------------------------------------------------------





