//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "testerBCB.h"



//#define TWO_PHANTOMS
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
bool simulationOn = false;
DWORD ThreadId;
  double phantomPos1X, phantomPos1Y, phantomPos1Z;
  double phantomPos1XN, phantomPos1YN, phantomPos1ZN;
  double phantomForce1X, phantomForce1Y, phantomForce1Z;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}

__fastcall TForm1::~TForm1(void)
{

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
  Button1->Enabled = false;

  // open the phantom
  PhantomAcces(0);
  ph_num1 = OpenPhantom(0);
  ResetPhantomEncoders(ph_num1);
  StartCommunicationPhantom(ph_num1);

  double maxStiffness = -100;

  FLAG_RUN = true;
  while (FLAG_RUN)
  {
     ReadPositionPhantom(ph_num1,phantomPos1X, phantomPos1Y, phantomPos1Z);
     ReadNormalizedPositionPhantom(ph_num1,phantomPos1XN, phantomPos1YN, phantomPos1ZN);
     if (phantomPos1XN < 0.0)
        phantomForce1X = maxStiffness * phantomPos1XN;
     else
        phantomForce1X = 0.0;

     phantomForce1Y = 0.0;
     phantomForce1Z = 0.0;

     SetForcePhantom(Form1->ph_num1, phantomForce1X, phantomForce1Y, phantomForce1Z);

     Edit2->Text = phantomPos1X;
     Edit3->Text = phantomPos1Y;
     Edit4->Text = phantomPos1Z;

     Edit6->Text = phantomForce1X;
     Edit7->Text = phantomForce1Y;
     Edit8->Text = phantomForce1Z;

     Edit8->Text = phantomPos1XN;
     Edit9->Text = phantomPos1YN;
     Edit10->Text = phantomPos1ZN;

     Application->ProcessMessages();
   }
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
        // kill loop
        FLAG_RUN = false;
        Application->ProcessMessages();
        StopCommunicationPhantom(ph_num1);
        ClosePhantoms();
        Button1->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormDestroy(TObject *Sender)
{
        FLAG_RUN = false;
        Edit1->Text = "THE END";
        ClosePhantoms();
        Application->ProcessMessages();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
        FLAG_RUN = false;
             Application->ProcessMessages();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
        StopCommunicationPhantom(ph_num1);
        Application->ProcessMessages();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
        StartCommunicationPhantom(ph_num1);
        Application->ProcessMessages();
}
//---------------------------------------------------------------------------


