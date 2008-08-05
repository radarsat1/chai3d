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
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    // create time object
    time = new cPrecisionClock();

    // define timeout
    time->setTimeoutPeriod(4550000);

    // display current time
    labelTime->Caption = time->getCurrentTime();

    // display timeout period
    labelPeriod->Caption = time->getTimeoutPeriod();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
    time->start();

    while (time->on())
    {
        // display current time
        labelTime->Caption = time->getCurrentTime();

        // check if timeout occurred
        if (time->timeoutOccurred())
        {
            labelMessage->Caption = "Timeout Occurred!";
        }
        else
        {
            labelMessage->Caption = "";
        }

        // process windows messages
        Application->ProcessMessages();
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
    // stop counting time
    time->stop();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
    // initialize time
    time->initialize();

    // display current time
    labelTime->Caption = time->getCurrentTime();

    // display timeout period
    labelPeriod->Caption = time->getTimeoutPeriod();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
    // stop counting time
    time->stop();
}
//---------------------------------------------------------------------------

