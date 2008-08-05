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
    \version    1.1
    \date       06/2005
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

const double MESH_SCALE_SIZE = 0.5;

// vector of sound-enabled meshes in the world
std::vector<cSoundMesh*> sound_meshes;
cGenericObject* contactObject;
cVector3d normalForce, tangentialForce;


__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{

}

// Write the requested data from the .wav file to the sound card
DWORD CALLBACK MyStreamWriter(HSTREAM handle, void *buf, DWORD len, DWORD user)
{
	char *cb=(char*)buf;

	cSoundMesh* sound_mesh;
	sound_mesh = sound_meshes[user];
	for (unsigned int i=0; i<len; i++) 
    cb[i] = sound_mesh->getSound()->play();
	return len; 
}


void TForm1::CreateObject(std::string fileName, cVector3d loc, cColorf color, sounds type, int tag)
{
    cSoundMesh* new_mesh = new cSoundMesh(world);
    new_mesh->loadFromFile(fileName);

    // compute size of object
    new_mesh->computeBoundaryBox(true);
    cVector3d min = new_mesh->getBoundaryMin();
    cVector3d max = new_mesh->getBoundaryMax();

    // This is the "size" of the object
    cVector3d span = cSub(max, min);
    double size = cMax(span.x, cMax(span.y, span.z));

    // We'll center all vertices, then multiply by this amount,
    // to scale to the desired size.
    double scaleFactor = MESH_SCALE_SIZE / size;
    new_mesh->scale(scaleFactor);

    // compute size of object again
    new_mesh->computeBoundaryBox(true);

    // Build a collision-detector for this object, so
    // the proxy will work nicely when haptics are enabled.
    new_mesh->createAABBCollisionDetector(true,true);

    // set size of frame
    new_mesh->setFrameSize(0.2, true);

    // set size of normals
    new_mesh->setNormalsProperties(0.01, cColorf(1.0, 0.0, 0.0, 1.0), true);

    // update global position
    new_mesh->computeGlobalPositions();

		// set material properties
    cMaterial material;
    material.m_ambient.set( color.getR(), color.getG(), color.getB(), color.getA() );
    material.m_diffuse.set( color.getR(), color.getG(), color.getB(), color.getA() );
    material.m_specular.set( color.getR(), color.getG(), color.getB(), color.getA() );
    material.setShininess(100);
    new_mesh->setMaterial(material, true);
    new_mesh->useMaterial(true, true);

		// set sound parameters and create stream
    new_mesh->getSound()->setParams(type);
		new_mesh->getSound()->stream=BASS_StreamCreate(44100, 1 ,BASS_DEVICE_8BITS,&MyStreamWriter,tag);

		// translate and set stiffness and friction
    new_mesh->translate(loc.x, loc.y, loc.z);
    new_mesh->setStiffness(15.0, true);
		new_mesh->setFriction(0.6, 0.4, true);

		// add this to the world and to the vector of sound meshes
	  sound_meshes.push_back(new_mesh);
    world->addChild(new_mesh);
}


//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    // initialize the sound device
    if (!BASS_Init(1,44100,0,0,NULL))
	    Application->MessageBox("Initialization Error", "Error");

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
    light->setPos(cVector3d(0,0.5,1));
    camera->addChild(light);

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

    // create a sound-enabled bell
    CreateObject("resources\\models\\handbell.3ds", cVector3d(0,-0.5,0), cColorf(0.82, 0.7, 0.0, 1.0), BELL, 0);

    // create a sound-enabled teapot
    CreateObject("resources\\models\\teapot.3ds", cVector3d(0,0.5,0), cColorf(0.5, 0.5, 0.5, 1.0), TEAPOT, 1);

    // update camera position
    updateCameraPosition();

    // don't start the haptics loop yet
    flagSimulationOn = false;
    flagHasExitedSimulation = true;
    tool = 0;
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

    if (tool)
    {
      // see if there is a collision with a sound-enabled object (including
      // objects who have a sound-enabled parent mesh)
      cSoundMesh* sound_mesh = NULL;
      if (contactObject != NULL)
      {
        cGenericObject* parent = contactObject; //tool->getProxy()->getContactObject();
        contactObject = NULL;
        sound_mesh = dynamic_cast<cSoundMesh*>(parent);
        while ((parent) && (!sound_mesh))
        {
          parent = parent->getParent();
          sound_mesh = dynamic_cast<cSoundMesh*>(parent);
        }
      }

      // call play methods for sound-enabled objects
      for (unsigned int i=0; i<sound_meshes.size(); i++)
      {

        // send the current force to the currently contacted object, if any
        if (sound_meshes[i] == sound_mesh)
          sound_mesh->getSound()->setContactForce(normalForce, tangentialForce);
        else
          sound_meshes[i]->getSound()->setContactForce(cVector3d(0,0,0), cVector3d(0,0,0));

		    // if there is a new contact, reset and restart playing the sound
        if ((sound_meshes[i]->getSound()->getNormalForce().lengthsq() > 0.01) &&
			      (sound_meshes[i]->getSound()->getPreviousForce().lengthsq() <= 0.01))
		    {
          // reset for the new contact
          sound_meshes[i]->getSound()->reset();

			    // restart playing this stream
			    if (!(BASS_ChannelPlay(sound_meshes[i]->getSound()->stream,TRUE)))
		        Application->MessageBox("Playback Error", "Error");
		    }
      }
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
      // update tool position and rotation
      Form1->tool->updatePose();

      if (Form1->tool->getProxy()->getContactObject())
      {
        contactObject = Form1->tool->getProxy()->getContactObject();
        normalForce = Form1->tool->getProxy()->getNormalForce();
        tangentialForce = Form1->tool->getProxy()->getTangentialForce();
      }

      // calculate forces
      Form1->tool->computeForces();

      // apply forces
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

        // Enable the Zilles Friction algorithm (to get tangential forces)
        tool->getProxy()->setUseZillesFriction(true);

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
    exit(0);
}

//---------------------------------------------------------------------------




