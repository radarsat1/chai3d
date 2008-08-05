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
#include "cVBOMesh.h"
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TForm1 *Form1;
void HapticLoop();
//---------------------------------------------------------------------------

const double MESH_SCALE_SIZE = 1.0;

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner), usingHapticCallback(false)
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
    light->setPos(cVector3d(0.0,0.5,0.7));
    camera->addChild(light);

    // define the default position of the camera
    camera->set( cVector3d (2.0, 0.0, 0.0),  // camera position
                 cVector3d (0.0, 0.0, 0.0),  // lookat position
                 cVector3d (0.0, 0.0, 1.0)); // camera up vector

    // flag used when the operator rotates the camera with the mouse
    flagObjectInMotion = false;

    // set the near and far clipping planes of the camera
    camera->setClippingPlanes(0.01, 10.0);

    // create a display for graphic rendering
    viewport = new cViewport(Panel1->Handle, camera, true);
    viewport->setStereoOn(false);

    // create a mesh
    object = new cMesh(world);
    world->addChild(object);

    // set the size of the visual frame for this object
    object->setFrameSize(0.2, 0.2);

    // initialise
    updateWorldSettings();

    // don't start the haptics loop yet
    flagSimulationOn = false;
    flagHasExitedSimulation = true;

    myHapticCallback.data = 1234;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
    // stop simulation
    stopSimulation();
    
    // stop graphic rendering
    Timer1->Enabled = false;

    // cleanup memory
    delete world;
    delete viewport;
}

//---------------------------------------------------------------------------

void TForm1::updateWorldSettings()
{
    // apply settings to object
    object->setWireMode(WireFrameBox->Checked, true);
    object->setShowFrame(ShowFrameBox->Checked, true);
    object->showNormals(ShowNormalsBox->Checked, true);
    object->setShowBox(ShowBoundaryBoxBox->Checked, true);
    object->showCollisionTree(ShowCollisionDetectorBox->Checked, true);
    object->useTexture(UseTextureMappingBox->Checked, true);
    object->useColors(UseVertexColorsBox->Checked, true);
    object->useMaterial(UseMaterialPropertiesBox->Checked, true);

    // set transparency
    float transparency = (float)TransparencyLevelSlider->Position / 10.0f;
    object->setTransparencyLevel(transparency, true, true);

    // set stiffness
    double stiffness = (double)StiffnessSlider->Position;
    object->setStiffness(stiffness, true);

    // set static and dynamic friction
    double staticFriction = (double)StaticFrictionSlider->Position / 100.0;
    double dynamicFriction = (double)DynamicFrictionSlider->Position / 100.0;
    object->setFriction(staticFriction, dynamicFriction, true);

    // set collision detector display properties
    object->setCollisionDetectorProperties( CollisionDepthSlider->Position,
                                            cColorf(0.7, 0.0, 0.0),
                                            true);

    // modify viewport if stereo has been toggled
    if ((UseStereoBox->Checked) && (!viewport->getStereoOn()))
    {
        viewport->setStereoOn(true);
    }
    else if ((!UseStereoBox->Checked) && (viewport->getStereoOn()))
    {
        viewport->setStereoOn(false);
    }

    // set stereo properties
    camera->setStereoEyeSeparation( 2.0 * EyeSeparationSlider->Position/100.0 );
    camera->setStereoFocalLength( FocalLengthSlider->Position );
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    // update options
    updateWorldSettings();

    // render world in display
    if (viewport != NULL)
    {
        viewport->render();
    }
}
//---------------------------------------------------------------------------
void HapticIter()
{
        // read position from haptic device
        Form1->tool->updatePose();

        // compute forces
        if (!Form1->flagBusy)
        {
            Form1->tool->computeForces();
        }

        // send forces to haptic device
        Form1->tool->applyForces();
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
        HapticIter();
    }

    // stop haptics
    Form1->tool->stop();
    flagHasExitedSimulation = true;

    // exit thread
    ExitThread(0);
}
//---------------------------------------------------------------------------
void TForm1::HapticCallback::callback()
{
    if (data!=1234) {
        data=0;
    }
    HapticIter();
}
//---------------------------------------------------------------------------
void TForm1::stopSimulation()
{
    // stop current simulation
    flagSimulationOn = false;
    if (usingHapticCallback) {
        tool->getDevice()->setCallback(0);
        Form1->tool->stop();
        flagHasExitedSimulation = true;
    } else {
        while (!flagHasExitedSimulation)
        {
            Sleep(1);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::LoadModelButtonClick(TObject *Sender)
{
    // stop current simulation
    stopSimulation();
    
    if (OpenDialog1->Execute())
    {
        // create a new mesh
        cMesh* newObject = new cMesh(world);

        // If you don't plan to ever modify the mesh, you can use the
        // vertex-buffer version instead for faster rendering...
        // cMesh* newObject = new cVBOMesh(world);

        newObject->setPos(0,0,0);
        objectToCameraDistance = 0.0;

        // load 3d object file
        newObject->loadFromFile(OpenDialog1->FileName.c_str());

        // compute size of object
        newObject->computeBoundaryBox(true);

        cVector3d min = newObject->getBoundaryMin();
        cVector3d max = newObject->getBoundaryMax();

        // This is the "size" of the object
        cVector3d span = cSub(max, min);
        double size = cMax(span.x, cMax(span.y, span.z));

        // We'll center all vertices, then multiply by this amount,
        // to scale to the desired size.
        double scaleFactor = MESH_SCALE_SIZE / size;
        newObject->scale(scaleFactor);

        // compute size of object again
        newObject->computeBoundaryBox(true);

        // Build a collision-detector for this object, so
        // the proxy will work nicely when haptics are enabled.

        if (AABBTreeButton->Checked)
        {
            newObject->createAABBCollisionDetector(true, false);
        }
        else
        {
            newObject->createSphereTreeCollisionDetector(true, false);
        }

        // set size of frame
        newObject->setFrameSize(0.2, true);

        // set size of normals
        newObject->setNormalsProperties(0.01, cColorf(1.0, 0.0, 0.0, 1.0), true);

        // update global position
        newObject->computeGlobalPositions();

        // remove previous object from world and add new one
        world->removeChild(object);
        delete object;
        object = newObject;
        world->addChild(object);

        // This will set up default rendering options and haptic properties
        updateWorldSettings();

        // display information about object
        NumVerticesLabel->Caption = object->getNumVertices(true);
        NumTrianglesLabel->Caption = object->getNumTriangles(true);
    }
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

            tool->setRadius(0.02);
            tool->setFrameSize(0.04);
        }

        // set up the device
        tool->initialize();

        // open communication to the device
        tool->start();

        // tell the tool to show his coordinate frame so you
        // can see tool rotation
        tool->setToolFrame(true, 0.1);

        // I need to call this so the tool can update its internal
        // transformations before performing collision detection, etc.
        tool->computeGlobalPositions();
        tool->setForcesON();

        // Enable the "dynamic proxy", which will handle moving objects
        cProxyPointForceAlgo* proxy = tool->getProxy();
        proxy->enableDynamicProxy(true);

        // update the global position of all objects in the world
        world->computeGlobalPositions(true);

        if (tool->getDevice()->setCallback(reinterpret_cast<cCallback*>(&myHapticCallback))) {
            flagSimulationOn = true;
            usingHapticCallback = true;
            flagHasExitedSimulation = false;
        } else {
            usingHapticCallback = false;
            DWORD thread_id;
            ::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)(HapticLoop), this, 0, &thread_id);

            // Boost thread and process priority
            ::SetThreadPriority(&thread_id, THREAD_PRIORITY_ABOVE_NORMAL);
        }
    }
    else
    {
        stopSimulation();
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

void __fastcall TForm1::CollisionDepthSliderChange(TObject *Sender)
{
    cGenericCollision* col = object->getCollisionDetector();
    col->setDisplayDepth(CollisionDepthSlider->Position);
}

//---------------------------------------------------------------------------

void __fastcall TForm1::AABBTreeButtonClick(TObject *Sender)
{
    // inform the haptic thread not to compute any more collisions
    flagBusy = true;
    Sleep(10);
    if (tool != NULL) tool->setForcesOFF();

    if (AABBTreeButton->Checked)
    {
        object->createAABBCollisionDetector(true, false);
    }
    else
    {
        object->createSphereTreeCollisionDetector(true, false);
    }

    // collision tree has been updated
    if (tool != NULL) tool->setForcesON();
    flagBusy = false;

}

//---------------------------------------------------------------------------

void __fastcall TForm1::Panel1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    flagObjectInMotion = true;
    mouseX = X;
    mouseY = Y;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Panel1MouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
    double angleH, angleV;
    double K = 0.01;

    if (flagObjectInMotion)
    {
        if (Shift.Contains(ssLeft))
        {
            angleH =  K *(X - mouseX);
            angleV =  K *(Y - mouseY);
            object->rotate(cVector3d(0, angleV, angleH), sqrt(cSqr(angleV)+cSqr(angleH)));
            world->computeGlobalPositions(true);
        }
        else if (Shift.Contains(ssRight))
        {
            objectToCameraDistance = objectToCameraDistance - 0.01 * (Y - mouseY);
            object->setPos(objectToCameraDistance, 0, 0);
            world->computeGlobalPositions(true);
        }
    }

    mouseX = X;
    mouseY = Y;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Panel1MouseUp(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
    flagObjectInMotion = false;
}

//---------------------------------------------------------------------------






