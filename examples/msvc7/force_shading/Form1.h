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

#pragma once

#include "CWorld.h"
#include "CViewport.h"
#include "CGenericObject.h"
#include "CMeta3dofPointer.h"
#include "CPrecisionTimer.h"
#include "CLight.h"
#include "CForceShadingProxy.h"

// Prototype for haptic iteration callback function
void haptic_iteration(void* param);


namespace force_shading
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class Form1 : public System::Windows::Forms::Form
	{	
	public:
		Form1(void)
		{
			InitializeComponent();
		}
  
	protected:
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::Panel *  panel1;
	private: System::Windows::Forms::Button *  button1;

	public:
		cWorld* world;
		cCamera* camera;
		cViewport* viewport;
		cLight* light;
		cMesh* object;
		cPrecisionTimer* mm_timer;
		cGeneric3dofPointer* tool;
		cForceShadingProxy* new_proxy;
		int haptics_enabled;
	private: System::Windows::Forms::Timer *  timer1;
	private: System::Windows::Forms::CheckBox *  ShadingBox;
	private: System::ComponentModel::IContainer *  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = new System::ComponentModel::Container();
			this->panel1 = new System::Windows::Forms::Panel();
			this->button1 = new System::Windows::Forms::Button();
			this->timer1 = new System::Windows::Forms::Timer(this->components);
			this->ShadingBox = new System::Windows::Forms::CheckBox();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->Location = System::Drawing::Point(16, 16);
			this->panel1->Name = S"panel1";
			this->panel1->Size = System::Drawing::Size(656, 464);
			this->panel1->TabIndex = 0;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(176, 496);
			this->button1->Name = S"button1";
			this->button1->Size = System::Drawing::Size(176, 24);
			this->button1->TabIndex = 1;
			this->button1->Text = S"Toggle Haptics";
			this->button1->Click += new System::EventHandler(this, button1_Click);
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 30;
			this->timer1->Tick += new System::EventHandler(this, timer1_Tick);
			// 
			// ShadingBox
			// 
			this->ShadingBox->Checked = true;
			this->ShadingBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->ShadingBox->Location = System::Drawing::Point(416, 496);
			this->ShadingBox->Name = S"ShadingBox";
			this->ShadingBox->Size = System::Drawing::Size(168, 24);
			this->ShadingBox->TabIndex = 2;
			this->ShadingBox->Text = S"Use Force Shading";
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(688, 534);
			this->Controls->Add(this->ShadingBox);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->panel1);
			this->Name = S"Form1";
			this->Text = S"Force Shading";
			this->Closing += new System::ComponentModel::CancelEventHandler(this, Form1_Closing);
			this->Load += new System::EventHandler(this, Form1_Load);
			this->ResumeLayout(false);

		}	
	private: System::Void Form1_Load(System::Object *  sender, System::EventArgs *  e)
			 {
				 // Allocate a console for outputting _cprintf statements for debugging
				 AllocConsole();

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
				 IntPtr ptr = panel1->get_Handle();
				 HWND hwnd = (HWND)ptr.ToPointer();
				 viewport = new cViewport(hwnd, camera, false);

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
				 cMaterial material1;
				 material1.setStiffness(3.0);
				 material1.m_ambient.set(0.0, 0.8, 0.0, 1.0);
				 material1.m_diffuse.set(0.0, 0.8, 0.0, 1.0);
				 material1.m_specular.set(1.0, 1.0, 1.0, 1.0);
				 material1.setShininess(100.0);
				 object->m_material = material1;

				 // Create a callback timer (to be used for haptics loop)
				 mm_timer = new cPrecisionTimer();

				 // Initilize some variables
				 world->computeGlobalPositions();
				 tool = 0;
				 haptics_enabled = 0;
			 }

	private: System::Void timer1_Tick(System::Object *  sender, System::EventArgs *  e)
			 {
				 // Set whether we are using force shading according to GUI checkbox
				 if (new_proxy)
				 {
					 if (ShadingBox->CheckState == Checked)
					   new_proxy->setUseShading(1);
					 else
					   new_proxy->setUseShading(0);
					 
				 }

				 // Render the scene
				 if (viewport)
					 viewport->render();
			 }

private: System::Void button1_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 // If haptics isn't on, turn it on...
			 if (haptics_enabled == 0) {
				 haptics_enabled = 1;

				 // Create a tool if we haven't yet
				 if (tool == 0) {
					 tool = new cMeta3dofPointer(world, 0, true);
					 world->addChild(tool);

					 // Set up the device
					 tool->initialize();

					 // Set up a nice-looking workspace for the phantom so 
                     // it fits nicely with our shape
					 tool->setWorkspace(2.0, 2.0, 2.0);

					 // Rotate the tool so its axes align with our opengl-like axes
					 tool->rotate(cVector3d(0,0,1), -90.0*3.14159/180.0);
					 tool->rotate(cVector3d(1,0,0), -90.0*3.14159/180.0);
					 tool->setRadius(0.05);
					 
					 // Replace the default point force algorithms with our force-shading proxy
					 tool->m_pointForceAlgos.clear();
					 cForceShadingProxy* shading_proxy = new cForceShadingProxy();
					 new_proxy = shading_proxy;
					 shading_proxy->initialize(world, cVector3d(0,0,0));
					 shading_proxy->setProxyRadius(0.05*0.001);
					 tool->m_pointForceAlgos.push_back(shading_proxy);
				 }
				 tool->computeGlobalPositions();

				 // Open communication with the device
				 tool->start();

				 // Enable forces
				 tool->setForcesON();

				 // Set the callback timer to call the haptic_iteration function every millisecond
				 mm_timer->set(0, haptic_iteration, tool);
			 }

			 // If haptics is already on, turn it off...
			 else {
				 haptics_enabled = 0;

				 // Stop the timer and the haptic device...
				 mm_timer->stop();
				 tool->setForcesOFF();
				 tool->stop();
			 }

		 }

private: System::Void Form1_Closing(System::Object *  sender, System::ComponentModel::CancelEventArgs *  e)
		 {
			 // Stop the haptic timer and device if they are on
			 if (haptics_enabled) {
				 mm_timer->stop();
				 tool->setForcesOFF();
				 tool->stop();
			 }

			 // Clean-up before closing...
			 if (world) delete world;
			 if (viewport) delete viewport;
			 if (camera) delete camera;
		 }

};
}


// This is the function that gets called by the multimedia timer
void haptic_iteration(void* param) {
	cGeneric3dofPointer* tool = (cGeneric3dofPointer*)(param);

	// Just update the pose, compute forces, and apply those forces to the device
	tool->updatePose();
	tool->computeForces();
	tool->applyForces();
}


