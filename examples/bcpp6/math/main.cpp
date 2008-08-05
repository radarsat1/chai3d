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
#include <vcl.h>
#pragma hdrstop
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm2::FormCreate(TObject *Sender)
{
  // initialise vector v0
  v0.set(1.0, 2.0, 3.0);

  // copy values of vector v0 to vector v1
  v1.copyfrom(v0);

  // copy values from vector v1 to vector vr
  v1.copyto(vr);

  // initialise matrix m0 with identity matrix
  m0.identity();

  // initilise matrix m2 with values
  m1.set(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);

  // copy values from m2 to mr
  m1.copyto(mr);

  // update display of vector in edit boxes
  updateDisplay();

  // read values from edit boxes
  readValues();
}


void TForm2::updateDisplay()
{
  // display vector v0
  edtV0X->Text = v0.x;
  edtV0Y->Text = v0.y;
  edtV0Z->Text = v0.z;

  // display vector v1
  edtV1X->Text = v1.x;
  edtV1Y->Text = v1.y;
  edtV1Z->Text = v1.z;

  // display vector vr
  edtVRX->Text = vr.x;
  edtVRY->Text = vr.y;
  edtVRZ->Text = vr.z;

  // display matrix m0
  edtM0V00->Text = m0.m[0][0];
  edtM0V01->Text = m0.m[0][1];
  edtM0V02->Text = m0.m[0][2];
  edtM0V10->Text = m0.m[1][0];
  edtM0V11->Text = m0.m[1][1];
  edtM0V12->Text = m0.m[1][2];
  edtM0V20->Text = m0.m[2][0];
  edtM0V21->Text = m0.m[2][1];
  edtM0V22->Text = m0.m[2][2];

  // display matrix m1
  edtM1V00->Text = m1.m[0][0];
  edtM1V01->Text = m1.m[0][1];
  edtM1V02->Text = m1.m[0][2];
  edtM1V10->Text = m1.m[1][0];
  edtM1V11->Text = m1.m[1][1];
  edtM1V12->Text = m1.m[1][2];
  edtM1V20->Text = m1.m[2][0];
  edtM1V21->Text = m1.m[2][1];
  edtM1V22->Text = m1.m[2][2];

  // display matrix mr
  edtMRV00->Text = mr.m[0][0];
  edtMRV01->Text = mr.m[0][1];
  edtMRV02->Text = mr.m[0][2];
  edtMRV10->Text = mr.m[1][0];
  edtMRV11->Text = mr.m[1][1];
  edtMRV12->Text = mr.m[1][2];
  edtMRV20->Text = mr.m[2][0];
  edtMRV21->Text = mr.m[2][1];
  edtMRV22->Text = mr.m[2][2];
}


void TForm2::readValues()
{
  // read values for vector v0
  v0.x = atof(edtV0X->Text.c_str());
  v0.y = atof(edtV0Y->Text.c_str());
  v0.z = atof(edtV0Z->Text.c_str());

  // read values for vector v1
  v1.x = atof(edtV1X->Text.c_str());
  v1.y = atof(edtV1Y->Text.c_str());
  v1.z = atof(edtV1Z->Text.c_str());

  // read values for vector vr
  vr.x = atof(edtVRX->Text.c_str());
  vr.y = atof(edtVRY->Text.c_str());
  vr.z = atof(edtVRZ->Text.c_str());

  // read values for matrix m0
  m0.m[0][0] = atof(edtM0V00->Text.c_str());
  m0.m[0][1] = atof(edtM0V01->Text.c_str());
  m0.m[0][2] = atof(edtM0V02->Text.c_str());
  m0.m[1][0] = atof(edtM0V10->Text.c_str());
  m0.m[1][1] = atof(edtM0V11->Text.c_str());
  m0.m[1][2] = atof(edtM0V12->Text.c_str());
  m0.m[2][0] = atof(edtM0V20->Text.c_str());
  m0.m[2][1] = atof(edtM0V21->Text.c_str());
  m0.m[2][2] = atof(edtM0V22->Text.c_str());

  // read values for matrix m1
  m1.m[0][0] = atof(edtM1V00->Text.c_str());
  m1.m[0][1] = atof(edtM1V01->Text.c_str());
  m1.m[0][2] = atof(edtM1V02->Text.c_str());
  m1.m[1][0] = atof(edtM1V10->Text.c_str());
  m1.m[1][1] = atof(edtM1V11->Text.c_str());
  m1.m[1][2] = atof(edtM1V12->Text.c_str());
  m1.m[2][0] = atof(edtM1V20->Text.c_str());
  m1.m[2][1] = atof(edtM1V21->Text.c_str());
  m1.m[2][2] = atof(edtM1V22->Text.c_str());

  // read values for matrix mr
  mr.m[0][0] = atof(edtMRV00->Text.c_str());
  mr.m[0][1] = atof(edtMRV01->Text.c_str());
  mr.m[0][2] = atof(edtMRV02->Text.c_str());
  mr.m[1][0] = atof(edtMRV10->Text.c_str());
  mr.m[1][1] = atof(edtMRV11->Text.c_str());
  mr.m[1][2] = atof(edtMRV12->Text.c_str());
  mr.m[2][0] = atof(edtMRV20->Text.c_str());
  mr.m[2][1] = atof(edtMRV21->Text.c_str());
  mr.m[2][2] = atof(edtMRV22->Text.c_str());
}

//---------------------------------------------------------------------------

void __fastcall TForm2::Button1Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  vr.copyfrom(v0);
  vr.add(v1);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button2Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  vr.copyfrom(v0);
  vr.sub(v1);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button3Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  vr.mul(2.0);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button4Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  vr.div(2.0);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button5Click(TObject *Sender)
{
  // compute operation
  vr.normalize();

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button16Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  double value = vr.length();

  // display results in edit boxes
  edtDot->Text = value;
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button6Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  v0.crossr(v1, vr);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button7Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  vr.cross(v1);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button8Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  vr.copyfrom(v0);
  double value = vr.dot(v1);

  // display results in edit boxes
  edtDot->Text = value;
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button9Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  m0.identity();
  m1.identity();
  mr.identity();

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button10Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  //mr.copyfrom(m0);
  m0.mulr(m1,mr);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button11Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  if (m0.invert())
  {
    edtMatrix->Text = "Operation performed";
  }
  else
  {
    edtMatrix->Text = "Operation failed";
  }

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button12Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  m1.copyfrom(m0);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button13Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  mr.trans();

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button14Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  m1.transr(mr);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button15Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  m0.mulr(v0,vr);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button17Click(TObject *Sender)
{
  // read values from edit boxes
  readValues();

  // compute operation
  mr.set(0.0);

  // display results in edit boxes
  updateDisplay();
}
//---------------------------------------------------------------------------

