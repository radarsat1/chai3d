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
#ifndef object_loaderH
#define object_loaderH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "CCamera.h"
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
    TGroupBox *GroupBox1;
    TLabel *Label8;
    TLabel *Label9;
    TLabel *Label10;
    TTrackBar *CameraZoomSlider;
    TTrackBar *VAngleSlider;
    TTrackBar *HAngleSlider;
    TButton *ToggleHapticsButton;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall ToggleHapticsButtonClick(TObject *Sender);
    void __fastcall updateCamera(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);

private:
    // virtual world
    cWorld* world;
    // camera
    cCamera* camera;
    // object mesh
    cMesh* object;
    // viewport
    cViewport* viewport;
    // material properties
    cMaterial* material;
    // size of the virtual pyramid object
    double objectSize;

public:
    __fastcall TForm1(TComponent* Owner);
    __fastcall ~TForm1() { delete object; }

    // haptic tool
    cMeta3dofPointer *tool;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
