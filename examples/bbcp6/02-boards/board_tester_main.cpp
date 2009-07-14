//===========================================================================
/*
    This file is part of the CHAI 3D visualization and haptics libraries.
    Copyright (C) 2003-#YEAR# by CHAI 3D. All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License("GPL") version 2
    as published by the Free Software Foundation.

    For using the CHAI 3D libraries with software that can not be combined
    with the GNU GPL, and for taking advantage of the additional benefits
    of our support services, please contact CHAI 3D about acquiring a
    Professional Edition License.

    \author:    <http://www.chai3d.org>
    \author:    Federico Barbagli
    \version    #CHAI_VERSION#
*/
//===========================================================================

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "board_tester_main.h"
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

void __fastcall TForm1::Button1Click(TObject *Sender)
{
        // create board
        board = new cGenericDevice;
        servotogoBoard = new cDriverServotogo;
        board = servotogoBoard;

        // open board
        board->open();

        // check if board opened properly
        if (board->isSystemReady())
        {
            Edit1->Text = "Servotogo Board Ready";
            // enable all buttons
            GroupBox1->Enabled = true;
            GroupBox3->Enabled = true;
        }
        else
                Edit1->Text = "Servotogo Board did NOT open properly";

        // finalize GUI
        Button1->Enabled = false;
        Button2->Enabled = false;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
        board = new cGenericDevice;
        sensorayBoard = new cDriverSensoray626;
        board = sensorayBoard;

        // open board
        board->open();

        // check if board opened properly
        if (board->isSystemReady())
        {
            Edit1->Text = "Sensoray626 Board Ready";
            // enable all buttons
            GroupBox1->Enabled = true;
            GroupBox3->Enabled = true;

            // disable buttons that cannot be used by the sensoray 626
            Button9->Enabled = false;
            Button10->Enabled = false;
            Button15->Enabled = false;
            Button16->Enabled = false;
            Button17->Enabled = false;
            Button18->Enabled = false;
        }
        else
                Edit1->Text = "Sensoray626 Board did NOT open properly";

        // finalize GUI
        Button1->Enabled = false;
        Button2->Enabled = false;

}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
        GroupBox1->Enabled = false;
        GroupBox3->Enabled = false;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
  long encoder_value;
  board->command(CHAI_CMD_GET_ENCODER_0, &encoder_value);
  Edit2->Text = encoder_value;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
  long encoder_value;
  board->command(CHAI_CMD_GET_ENCODER_1, &encoder_value);
  Edit3->Text = encoder_value;

}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button5Click(TObject *Sender)
{
  long encoder_value;
  board->command(CHAI_CMD_GET_ENCODER_2, &encoder_value);
  Edit4->Text = encoder_value;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button6Click(TObject *Sender)
{
  long encoder_value;
  board->command(CHAI_CMD_GET_ENCODER_3, &encoder_value);
  Edit5->Text = encoder_value;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button7Click(TObject *Sender)
{
  long encoder_value;
  board->command(CHAI_CMD_GET_ENCODER_4, &encoder_value);
  Edit6->Text = encoder_value;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button8Click(TObject *Sender)
{
  long encoder_value;
  board->command(CHAI_CMD_GET_ENCODER_5, &encoder_value);
  Edit7->Text = encoder_value;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button9Click(TObject *Sender)
{
  long encoder_value;
  board->command(CHAI_CMD_GET_ENCODER_6, &encoder_value);
  Edit8->Text = encoder_value;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button10Click(TObject *Sender)
{
  long encoder_value;
  board->command(CHAI_CMD_GET_ENCODER_7, &encoder_value);
  Edit9->Text = encoder_value;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button11Click(TObject *Sender)
{
        double DACvalue;
        DACvalue = (double) TckDACvolts->Position;
        board->command(CHAI_CMD_SET_DAC_0,&DACvalue);

}

//---------------------------------------------------------------------------

void __fastcall TForm1::TckDACvoltsChange(TObject *Sender)
{
        Edit18->Text = TckDACvolts->Position;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button12Click(TObject *Sender)
{
        double DACvalue;
        DACvalue = (double) TckDACvolts->Position;
        board->command(CHAI_CMD_SET_DAC_1,&DACvalue);

}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button13Click(TObject *Sender)
{
        double DACvalue;
        DACvalue = (double) TckDACvolts->Position;
        board->command(CHAI_CMD_SET_DAC_2,&DACvalue);

}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button14Click(TObject *Sender)
{
        double DACvalue;
        DACvalue = (double) TckDACvolts->Position;
        board->command(CHAI_CMD_SET_DAC_3,&DACvalue);

}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button15Click(TObject *Sender)
{
        double DACvalue;
        DACvalue = (double) TckDACvolts->Position;
        board->command(CHAI_CMD_SET_DAC_4,&DACvalue);

}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button16Click(TObject *Sender)
{
        double DACvalue;
        DACvalue = (double) TckDACvolts->Position;
        board->command(CHAI_CMD_SET_DAC_5,&DACvalue);

}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button17Click(TObject *Sender)
{
        double DACvalue;
        DACvalue = (double) TckDACvolts->Position;
        board->command(CHAI_CMD_SET_DAC_6,&DACvalue);

}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button18Click(TObject *Sender)
{
        double DACvalue;
        DACvalue = (double) TckDACvolts->Position;
        board->command(CHAI_CMD_SET_DAC_7,&DACvalue);

}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button19Click(TObject *Sender)
{
        double DACvalue = 0;
        TckDACvolts->Position = 0;
        board->command(CHAI_CMD_SET_DAC_0,&DACvalue);
        board->command(CHAI_CMD_SET_DAC_1,&DACvalue);
        board->command(CHAI_CMD_SET_DAC_2,&DACvalue);
        board->command(CHAI_CMD_SET_DAC_3,&DACvalue);
        board->command(CHAI_CMD_SET_DAC_4,&DACvalue);
        board->command(CHAI_CMD_SET_DAC_5,&DACvalue);
        board->command(CHAI_CMD_SET_DAC_6,&DACvalue);
        board->command(CHAI_CMD_SET_DAC_7,&DACvalue);
}

//---------------------------------------------------------------------------

