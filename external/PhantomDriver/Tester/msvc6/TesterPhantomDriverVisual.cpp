#include "PhantomDriver.h"
#include <iostream.h>
#include <Windows.h>

double phantomPos1X, phantomPos1Y, phantomPos1Z;
double phantomForce1X, phantomForce1Y, phantomForce1Z;
int i = 0;
int ph_num1;

int main()
{

	
  int i = 10;
  ph_num1 = OpenPhantom(0);
  if (ph_num1<0)
  {
	  cout << "Phantom 0 does not exist" << endl;
	  exit(0);
  }

  cout << "Place the PHANToM in its reset position and press <ENTER>." << endl;
  
  // cout<< GetMaxForcePhantom(ph_num1) << endl;

  cin.get();
  cout << "You will now feel a vertical plane" << endl;
  ResetPhantomEncoders(ph_num1);
  StartCommunicationPhantom(ph_num1);
  i = 0;
  while (i<100000000)
  {
	    
    // read position
    ReadPositionPhantom(ph_num1,phantomPos1X, phantomPos1Y, phantomPos1Z);
    // make a vertical plane.		
    //cout << phantomPos1X << endl;
	  if (phantomPos1X < 0.0)
		  phantomForce1X = -1 * phantomPos1X;
	  else
		  phantomForce1X = 0.0;
	  phantomForce1Z = 0.0;
	  phantomForce1Y = 0.0;
	  SetForcePhantom(ph_num1, phantomForce1X, phantomForce1Y, phantomForce1Z);
	  i++;
  
  }

  cout << "Test complete.\n";  
  ClosePhantoms();

  cin.get();

  return 0;
}
