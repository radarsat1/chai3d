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
//---------------------------------------------------------------------------
#include "CMath.h"
#include "CVector3d.h"
#include "CMatrix3d.h"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TEdit *edtM0V00;
        TEdit *edtM0V10;
        TEdit *edtM0V20;
        TEdit *edtM0V01;
        TEdit *edtM0V11;
        TEdit *edtM0V21;
        TEdit *edtM0V02;
        TEdit *edtM0V12;
        TEdit *edtM0V22;
        TGroupBox *GroupBox2;
        TEdit *edtM1V00;
        TEdit *edtM1V10;
        TEdit *edtM1V20;
        TEdit *edtM1V01;
        TEdit *edtM1V11;
        TEdit *edtM1V21;
        TEdit *edtM1V02;
        TEdit *edtM1V12;
        TEdit *edtM1V22;
        TGroupBox *GroupBox3;
        TEdit *edtMRV00;
        TEdit *edtMRV10;
        TEdit *edtMRV20;
        TEdit *edtMRV01;
        TEdit *edtMRV11;
        TEdit *edtMRV21;
        TEdit *edtMRV02;
        TEdit *edtMRV12;
        TEdit *edtMRV22;
        TGroupBox *GroupBox4;
        TEdit *edtV0X;
        TEdit *edtV0Y;
        TEdit *edtV0Z;
        TGroupBox *GroupBox5;
        TEdit *edtV1X;
        TEdit *edtV1Y;
        TEdit *edtV1Z;
        TGroupBox *GroupBox6;
        TEdit *edtVRX;
        TEdit *edtVRY;
        TEdit *edtVRZ;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TButton *Button2;
        TButton *Button1;
        TButton *Button3;
        TButton *Button4;
        TButton *Button5;
        TButton *Button6;
        TButton *Button7;
        TButton *Button8;
        TEdit *edtDot;
        TTabSheet *TabSheet2;
        TButton *Button9;
        TButton *Button10;
        TButton *Button11;
        TButton *Button12;
        TButton *Button13;
        TButton *Button14;
        TButton *Button15;
        TButton *Button16;
        TButton *Button17;
        TEdit *edtMatrix;
    TMemo *Memo1;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall Button5Click(TObject *Sender);
        void __fastcall Button6Click(TObject *Sender);
        void __fastcall Button7Click(TObject *Sender);
        void __fastcall Button8Click(TObject *Sender);
        void __fastcall Button9Click(TObject *Sender);
        void __fastcall Button10Click(TObject *Sender);
        void __fastcall Button11Click(TObject *Sender);
        void __fastcall Button12Click(TObject *Sender);
        void __fastcall Button13Click(TObject *Sender);
        void __fastcall Button14Click(TObject *Sender);
        void __fastcall Button15Click(TObject *Sender);
        void __fastcall Button16Click(TObject *Sender);
        void __fastcall Button17Click(TObject *Sender);
private:	// User declarations

        // vectors
        cVector3d v0;
        cVector3d v1;
        cVector3d vr;

        // matrices
        cMatrix3d m0;
        cMatrix3d m1;
        cMatrix3d mr;

        // display values in Edit boxes
        void updateDisplay();

        // read values in Edit boxes
        void readValues();

public:		// User declarations
        __fastcall TForm2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
