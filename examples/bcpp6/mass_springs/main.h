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
    \date       01/2004
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
#include "CLight.h"
#include "CViewport.h"
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
#include "CMeta3dofPointer.h"


// A couple of new object types required for our mass-spring
// simulation

class CBall;

// A spring (rendered as a line) used to connect two "balls"
class CSpring : public cGenericObject {

public:

  CSpring();
  virtual ~CSpring();

  // The graphic radius of the spring
  float m_radius;

  // The rest length (in world units) of the spring
  double m_rest_length;

  // The stiffness constant for the spring
  double m_spring_constant;

  // The objects to which this spring is connected
  CBall* m_endpoint_1;
  CBall* m_endpoint_2;

  // Transiently computed at each time step
  double m_current_length;

  // Render the spring graphically
  virtual void render(const int a_renderMode=0);

};


// A ball (rendered as a sphere) that can be pulled around by 
// springs
class CBall : public cGenericObject {

public:

  CBall();
  virtual ~CBall();

  // The graphic radius of the ball
  float m_radius;

  // The mass of the ball
  double m_mass;

  // Springs to which I'm attached
  std::vector<CSpring*> m_springs;

  // My current world-space velocity
  cVector3d m_velocity;

  virtual void render (const int a_renderMode=0);
  
  // Transiently computed at each time step
  cVector3d current_force;
};

// Various physical constants

#define FLOOR_X_SIZE 200.0
#define FLOOR_Z_SIZE 200.0
#define FLOOR_Y_POSITION -0.3
#define DEFAULT_FLOOR_SPRING_CONSTANT 100.0
#define INITIAL_BALL_SPACING 0.5
#define DEFAULT_OBJECT_MASS 0.5

// Should be a negative value
#define DAMPING_CONSTANT -1.0
#define GRAVITY_CONSTANT -1.0
#define DEFAULT_SPRING_CONSTANT 20.0

// The initial number of masses
#define INITIAL_NUM_BALLS 6

// A constant to translate virtual forces into real haptic
// forces
#define HAPTIC_FORCE_CONSTANT 0.1

//---------------------------------------------------------------------------

bool simulationOn = false;
DWORD ThreadId;

//---------------------------------------------------------------------------

class TForm1 : public TForm
{
__published:
        TTimer *Timer1;
    TPanel *Panel1;
    TSplitter *Splitter2;
    TPanel *Panel2;
    TButton *ToggleHapticsButton;
    TMemo *Memo1;
    TButton *Button1;
        void __fastcall MassSpringFormCreate(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall Panel1Resize(TObject *Sender);
        void __fastcall ToggleHapticsButtonClick(TObject *Sender);
        void __fastcall MassSpringFormClose(TObject *Sender,
          TCloseAction &Action);
    void __fastcall Button1Click(TObject *Sender);

private:
    // virtual world
    cWorld* world;
    // camera
    cCamera* camera;
    // viewport
    cViewport* viewport;
    // world scale
    double scale;
    // precision clock
    cPrecisionClock clock;
    // this is my light
    cLight *light;

/*** SIMULATION VARIABLES ***/
public:
  // Called from the haptics loop if it's running (from the
  // graphics loop otherwise) to compute and integrate forces
  //
  // Updates the position and velocity of all the masses
  void compute_spring_forces();

  // The masses and springs currently being simulated 
  std::vector<CBall*> m_active_balls;
  std::vector<CSpring*> m_active_springs;

  // The computed force to be applied to the haptic device
  cVector3d m_haptic_force;
  
  // A mesh representing the "floor"
  cMesh* m_floor;

  // The spring constant applied to pull the balls up above the floor
  float m_floor_spring_constant;

  // Add a mass to or remove a mass from the end of the chain
  void add_ball();
  
/*** END M-S SIMULATION VARIABLES ***/

    // options
    bool wireFrame;
    bool showFrame;
    bool showBoundaryBox;
    bool showNormals;
    bool showTree;
    bool showCollisionDetector;
    bool useTexture;
    bool useColors;
    bool useMaterial;
    bool useCulling;

    // init settings
    void initSettings();

public:
        __fastcall TForm1(TComponent* Owner);
        __fastcall ~TForm1() { }
        // haptic tool
        cMeta3dofPointer *tool;

};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
