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
#include "CTexture2D.h"
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
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>
#include <Graphics.hpp>
#include "CPhantom3dofPointer.h"
#include "CMeta3dofPointer.h"
#include "CPrecisionClock.h"
#include "CDelta3dofPointer.h"
#include "CPrecisionTimer.h"
#include "CDriverSensoray626.h"
#include "CMaths.h"
#include <math.h>
#include <Math.hpp>

#include "bass.h"

//---------------------------------------------------------------------------
const int MAX_VAL_SLIDERS_P = 100000;
const int MAX_VAL_SLIDERS_I = 10000000;
const int FREQ = 1000;
const int MAX_TIME = 50;                 // length of time for chart (in 50ms blocks)
const double chartClock = 50;
//---------------------------------------------------------------------------

bool flagSimulationOn = false;
bool flagHasExitedSimulation = false;

DWORD ThreadId;

void createTexCoords(cMesh *a_mesh, double radius);

//---------------------------------------------------------------------------

class TForm1 : public TForm
{
__published:
    TTimer *Timer1;
    TPanel *Panel1;
    TSplitter *Splitter2;
    TOpenDialog *OpenDialog1;
    TPanel *Panel4;
  TMemo *Memo1;
    TButton *Button2;
    TButton *Button1;
    TLabel *Label2;
    TLabel *Label13;
    TLabel *Label14;
    TTrackBar *StiffnessSlider;
    TTrackBar *StaticFrictionSlider;
    TTrackBar *DynamicFrictionSlider;
    TComboBox *RecordSelect;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall ToggleHapticsButtonClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
  void __fastcall RecordSelectChange(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);

private:
    // virtual world
    cWorld* world;

    // camera
    cCamera* camera;

    // light source
    cLight* light;

    // viewport
    cViewport* viewport;

    // material properties
    cMaterial* material;

    // world scale
    double scale;

    // update settings of world
    void updateWorldSettings();


public:
    __fastcall TForm1(TComponent* Owner);

    // haptic tool
    cMeta3dofPointer* tool;

    // The high-precision timer that's used (optionally) to run
    // the haptic loop
    cPrecisionTimer timer;

    bool haptics_enabled;

    // object mesh
    cMesh* object;

    // animates the object
    void animateObject(cVector3d force);

    void load_record(int a_index);
    void load_player();

    // record mesh
    cMesh* m_movingObject;

    // interaction force between proxy and disc in global coordinates
    cVector3d m_interactionForce;
    cVector3d m_proxyPos;
    double m_torque;

    // clock
    cPrecisionClock m_clock;

    //=========================
    // RFD RELATED
    //=========================

    double DACvalue;

    // RFD angle
    double m_angle;
    // total reduction
    double m_reduction;
    // encoder counts per turn
    double m_cpt;
    // desired point
    double m_desiredPos;
    // P controller gain
    double m_P;
    // I countroller gain
    double m_I;
    // integrator memory
    double m_integratorVal;
    // D controller term
    double m_D;
    // position at the last time step
    double m_lastAngle;
    double m_velocity;
    double m_velocityOld;

    // actual commanded action from P, I, K term
    double m_actionK;
    double m_actionI;
    double m_actionD;

    // define clock for D term
    cPrecisionClock *m_time;

    double m_RFDInitialAngle;
    bool m_inContact;
    double m_lastGoodPosition;
    
    // properties of the object
    double m_rotPos;
    double m_rotVel;
    double m_inertia;

};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
