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
#include "CImplicitSphere.h"
#include "CImplicitCone.h"
#include "CImplicitCylinder.h"
#include "CImplicitTorus.h"

// Prototype for haptic iteration callback function
void haptic_iteration(void* param);


namespace implicit_functions
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

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
		cImplicitSphere* sphere;
		cImplicitCone* cone;
		cImplicitCylinder* cylinder;
		cImplicitTorus* torus;
		cPrecisionTimer* mm_timer;
		cGeneric3dofPointer* tool;
		int haptics_enabled;
	private: System::Windows::Forms::Timer *  timer1;
	private: System::Windows::Forms::TrackBar *  stiffnessBar;
	private: System::Windows::Forms::TrackBar *  staticFrictionBar;
	private: System::Windows::Forms::TrackBar *  dynamicFrictionBar;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
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
			this->stiffnessBar = new System::Windows::Forms::TrackBar();
			this->staticFrictionBar = new System::Windows::Forms::TrackBar();
			this->dynamicFrictionBar = new System::Windows::Forms::TrackBar();
			this->label1 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->label3 = new System::Windows::Forms::Label();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->stiffnessBar))->BeginInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->staticFrictionBar))->BeginInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->dynamicFrictionBar))->BeginInit();
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
			this->button1->Location = System::Drawing::Point(32, 496);
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
			// stiffnessBar
			// 
			this->stiffnessBar->Location = System::Drawing::Point(232, 488);
			this->stiffnessBar->Name = S"stiffnessBar";
			this->stiffnessBar->Size = System::Drawing::Size(136, 45);
			this->stiffnessBar->TabIndex = 2;
			this->stiffnessBar->Value = 3;
			// 
			// staticFrictionBar
			// 
			this->staticFrictionBar->Location = System::Drawing::Point(384, 488);
			this->staticFrictionBar->Name = S"staticFrictionBar";
			this->staticFrictionBar->Size = System::Drawing::Size(136, 45);
			this->staticFrictionBar->TabIndex = 3;
			this->staticFrictionBar->Value = 6;
			// 
			// dynamicFrictionBar
			// 
			this->dynamicFrictionBar->Location = System::Drawing::Point(536, 488);
			this->dynamicFrictionBar->Name = S"dynamicFrictionBar";
			this->dynamicFrictionBar->Size = System::Drawing::Size(136, 45);
			this->dynamicFrictionBar->TabIndex = 4;
			this->dynamicFrictionBar->Value = 3;
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(271, 536);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(48, 23);
			this->label1->TabIndex = 5;
			this->label1->Text = S"Stiffness";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(413, 536);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(88, 23);
			this->label2->TabIndex = 6;
			this->label2->Text = S"Static Friction";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(558, 536);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(104, 23);
			this->label3->TabIndex = 7;
			this->label3->Text = S"Dynamic Friction";
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(720, 566);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->dynamicFrictionBar);
			this->Controls->Add(this->staticFrictionBar);
			this->Controls->Add(this->stiffnessBar);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->panel1);
			this->Name = S"Form1";
			this->Text = S"Implicit Functions";
			this->Closing += new System::ComponentModel::CancelEventHandler(this, Form1_Closing);
			this->Load += new System::EventHandler(this, Form1_Load);
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->stiffnessBar))->EndInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->staticFrictionBar))->EndInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->dynamicFrictionBar))->EndInit();
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
				 light->setPos(cVector3d(2,2,4));

				 // Get a pointer to the panel object's handle and create a viewport, telling it to render to that panel
				 IntPtr ptr = panel1->get_Handle();
				 HWND hwnd = (HWND)ptr.ToPointer();
				 viewport = new cViewport(hwnd, camera, false);

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

				 // Create a callback timer (to be used for haptics loop)
				 mm_timer = new cPrecisionTimer();

				 // Initilize some variables
				 world->computeGlobalPositions();
				 tool = 0;
				 haptics_enabled = 0;
			 }

	private: System::Void timer1_Tick(System::Object *  sender, System::EventArgs *  e)
			 {
				 // Set the stiffnesses and friction coefficients based on current slider values
				 if (cone) {
					 cone->m_material.setStiffness(10.0*stiffnessBar->Value);
					 cone->m_material.setStaticFriction(0.1*staticFrictionBar->Value);
					 cone->m_material.setDynamicFriction(0.1*dynamicFrictionBar->Value);
				 }
				 if (cylinder) {
					 cylinder->m_material.setStiffness(10.0*stiffnessBar->Value);
					 cylinder->m_material.setStaticFriction(0.1*staticFrictionBar->Value);
					 cylinder->m_material.setDynamicFriction(0.1*dynamicFrictionBar->Value);
				 }
				 if (sphere) {
					 sphere->m_material.setStiffness(10.0*stiffnessBar->Value);
					 sphere->m_material.setStaticFriction(0.1*staticFrictionBar->Value);
					 sphere->m_material.setDynamicFriction(0.1*dynamicFrictionBar->Value);
				 }
				 if (torus) {
					 torus->m_material.setStiffness(10.0*stiffnessBar->Value);
					 torus->m_material.setStaticFriction(0.1*staticFrictionBar->Value);
					 torus->m_material.setDynamicFriction(0.1*dynamicFrictionBar->Value);
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


