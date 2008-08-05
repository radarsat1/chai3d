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
#include "CCallback.h"
//---------------------------------------------------------------------------

bool flagSimulationOn = false;
bool flagHasExitedSimulation = false;

DWORD ThreadId;

//---------------------------------------------------------------------------

void createCube(cMesh *a_mesh, double a_halfSize);

class TForm1 : public TForm
{
__published:
    TTimer *Timer1;
    TPanel *Panel1;
    TSplitter *Splitter2;
    TOpenDialog *OpenDialog1;
    TPanel *Panel4;
    TGroupBox *GroupBox2;
    TButton *Button1;
    TButton *Button2;
    TGroupBox *GroupBox3;
    TCheckBox *WireFrameBox;
    TCheckBox *ShowFrameBox;
    TCheckBox *ShowBoundaryBoxBox;
    TCheckBox *ShowNormalsBox;
    TCheckBox *UseTextureMappingBox;
    TCheckBox *UseVertexColorsBox;
    TCheckBox *UseMaterialPropertiesBox;
    TTrackBar *TransparencyLevelSlider;
    TLabel *Label16;
    TGroupBox *GroupBox5;
    TRadioButton *AABBTreeButton;
    TRadioButton *SphereTreeButton;
    TGroupBox *GroupBox6;
    TLabel *Label2;
    TTrackBar *StiffnessSlider;
    TLabel *Label13;
    TTrackBar *StaticFrictionSlider;
    TLabel *Label14;
    TTrackBar *DynamicFrictionSlider;
    TLabel *Label1;
    TGroupBox *GroupBox4;
    TCheckBox *UseStereoBox;
    TLabel *Label7;
    TLabel *Label11;
    TTrackBar *EyeSeparationSlider;
    TTrackBar *FocalLengthSlider;
    TGroupBox *GroupBox7;
    TLabel *Label5;
    TLabel *Label4;
    TLabel *NumVerticesLabel;
    TLabel *NumTrianglesLabel;
    TCheckBox *ShowCollisionDetectorBox;
    TLabel *Label17;
    TTrackBar *CollisionDepthSlider;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall ToggleHapticsButtonClick(TObject *Sender);
    void __fastcall LoadModelButtonClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall CollisionDepthSliderChange(TObject *Sender);
    void __fastcall AABBTreeButtonClick(TObject *Sender);
    void __fastcall Panel1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall Panel1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall Panel1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);

private:
    // virtual world
    cWorld* world;

    // camera
    cCamera* camera;

    // light source
    cLight* light;

    // object mesh
    cMesh* object;
    double objectToCameraDistance;


    // viewport
    cViewport* viewport;

    // material properties
    cMaterial* material;

    // world scale
    double scale;

    // update settings of world
    void updateWorldSettings();

    // mouse selection
    bool flagObjectInMotion;
    int mouseX, mouseY;

    // using callback instead of haptic thread
    bool usingHapticCallback;

    // stops the haptic loop
    void stopSimulation();
    class HapticCallback : cCallback {
    public:
        void callback();
        int data;
    };

    HapticCallback myHapticCallback;

public:
    __fastcall TForm1(TComponent* Owner);

    // haptic tool
    cMeta3dofPointer* tool;

    // flag
    bool flagBusy;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
