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
    \author:    Christopher Sewell, Bera, and Federico Barbagli
    \version    1.1
    \date       06/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "CCamera.h"
#include "CViewport.h"
#include "CLight.h"
#include "CWorld.h"
#include "CMesh.h"
#include "CTriangle.h"
#include "CVertex.h"
#include "CMaterial.h"
#include "CMatrix3d.h"
#include "CVector3d.h"
#include "CString.h"
#include "CCollisionAABB.h"
#include "CCollisionSpheres.h"
#include "CPrecisionClock.h"
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>;
#include "CDelta3dofPointer.h"
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include <Graphics.hpp>
#include "CPhantom3dofPointer.h"
#include "CMeta3dofPointer.h"
#include "CODEMesh.h"

#include "ode/ode.h"

//---------------------------------------------------------------------------

bool flagSimulationOn = false;
bool flagHasExitedSimulation = false;

DWORD ThreadId;

//---------------------------------------------------------------------------

// A slightly modified proxy algorithm is required to support ODE;
// we override the relevant functions in a subclass...
class cODEProxy : public cProxyPointForceAlgo
{    
  
protected:
    //! Remove the approximate test for whether the proxy has reached the goal point; use exact distances
    virtual bool goalAchieved(const cVector3d& a_proxy, const cVector3d& a_goal) const
    {
        // Always fail this test to force the proxy to continue...
        return false;
    }

    //! Remove the offsetting of the goal to account for proxy volume; use exact collision vectors
    virtual void offsetGoalPosition(cVector3d& a_goal, const cVector3d& a_proxy) const
    {
        // Leave the goal where it is...
        return;
    }
  
public:
    //! A constructor that copies relevant initialization state from another proxy
    cODEProxy(cProxyPointForceAlgo* a_oldProxy)
    {
        m_deviceGlobalPos = a_oldProxy->getDeviceGlobalPosition();
        m_proxyGlobalPos = a_oldProxy->getProxyGlobalPosition();
        m_lastGlobalForce.zero();
        m_world = a_oldProxy->getWorld();
        m_radius = a_oldProxy->getProxyRadius();
    }
};


void createCube(cMesh *mesh, float edge, int include_top = 1);

class TForm1 : public TForm
{
__published:
    TTimer *Timer1;
    TPanel *Panel1;
    TSplitter *Splitter2;
    TOpenDialog *OpenDialog1;
    TPanel *Panel2;
    TButton *Button2;
    TMemo *Memo1;
    TButton *Button1;
  TTrackBar *StiffnessSlider;
  TTrackBar *StaticSlider;
  TTrackBar *DynamicSlider;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TButton *Button3;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall ToggleHapticsButtonClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);

public:
    // virtual world
    cWorld* world;

    // camera
    cCamera* camera;

    // light source
    cLight* light;

    // viewport
    cViewport* viewport;
    
    __fastcall TForm1(TComponent* Owner);

    // temp poiner
    cMesh* lastObject;

    // material properties
    cMaterial materialGray;
    cMaterial materialRed;

    cODEMesh* create_wall(double a_x, double a_y, double a_z, bool a_top);

    // The box to push around using ODE
    cODEMesh* object;

    // Cubes to form a room in which to push the box
    cODEMesh* bottom_wall;
    cODEMesh* left_wall;
    cODEMesh* right_wall;
    cODEMesh* back_wall;
    cODEMesh* front_wall;

    // The interface to the haptic device...
    cMeta3dofPointer *tool1;

    // A second haptic interface...
    cMeta3dofPointer *tool2;

	  // ODE clock
    cPrecisionClock *ode_clock;

    // A flag that indicates whether the haptic loop should be running
    int haptics_enabled;

    // Flags that indicate whether haptics are currently enabled on each device
	  int first_device_enabled;
    int second_device_enabled;

    // A flag that indicates whether the haptics thread is currently running
    //
    // This flag does not get set when the haptic callback is driven from a
    // multimedia timer
    int haptics_thread_running;

	  // Callback function for ODE
    static void nearCallback (void *data, dGeomID o1, dGeomID o2);

    bool ready;

	  // This flag is set when the user first moves the tool into the area surrounded
    // by the walls; force rendering is delayed until then so that you don't start
	  // out stuck outside the area of interest
    int tool1_ready;
    int tool2_ready;

    //! ODE world
	  dWorldID		ode_world;
	  //! ODE space
	  dSpaceID		ode_space;
	  //! ODE contact group
	  dJointGroupID	ode_contact_group;
	  //! Stepsize for ODE simulation
	  float ode_step;
	  //! Pointer to callback function
	  dNearCallback  *ode_collision_callback;
	  //! Pointer to any extra data
	  void *ode_collision_callback_data;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
