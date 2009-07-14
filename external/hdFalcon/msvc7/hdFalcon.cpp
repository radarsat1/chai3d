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
    \author:    Francois Conti
    \version    #CHAI_VERSION#
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "stdafx.h"
#include "assert.h"
#include <conio.h>
//---------------------------------------------------------------------------
#include "hdFalcon.h"
#include <hdl/hdl.h>
#include <hdlu/hdlu.h>
//---------------------------------------------------------------------------
#include "chai3d.h"
//---------------------------------------------------------------------------

//=============================================================================
// GLOBAL VARIABLES
//=============================================================================

// maximum number of falcon devices
#define FALCON_NUM_DEVICES_MAX 8

// stucture used to store data related to each device entity
struct CFalconDeviceStatus
{
    HDLDeviceHandle handle;
    double position[3];
    double force[3];
    int    button;
	bool   enabled;
	bool   initialized;
};

// has dll been initialed
bool initDLL = false;

// table containing information for each device.
CFalconDeviceStatus falconDevices[FALCON_NUM_DEVICES_MAX];

// predefined value that expresses the absence of a Falcon.
int numFalconDevices = 0;

// has servo controller been started yet
bool servoStarted = false;

// main servo controller callback
HDLServoOpExitCode servoFalconDevices(void* pUserData);

// initialize servo controller
void initServo();


//==========================================================================
// DLL entry point
//==========================================================================
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{
	if (!initDLL)
	{
		initDLL = true;

		for (int i=0; i<FALCON_NUM_DEVICES_MAX; i++)
		{
			// init button data
			falconDevices[i].button = 0;

			// init position data
			falconDevices[i].position[0] = 0.0;
			falconDevices[i].position[1] = 0.0;
			falconDevices[i].position[2] = 0.0;

			// init force data
			falconDevices[i].force[0] = 0.0;
			falconDevices[i].force[1] = 0.0;
			falconDevices[i].force[2] = 0.0;

			// init enable/disable data
			falconDevices[i].enabled = false;

			// init falcon api initialized
			falconDevices[i].initialized = false;
		}
	}
	return (true);
}


//==========================================================================
// FUNCTIONS ACCESSIBLE FROM OUTSIDE
//==========================================================================

//==========================================================================
/*!
  Retrieves the number of devices of type Falcon.

  \fn     int hdFalconGetNumDevices()
  \return returns the number of devices found      

*/
//==========================================================================
__declspec(dllexport) int __stdcall hdFalconGetNumDevices() 
{
	return (hdlCountDevices());
}


//==========================================================================
/*!
  Open a connexion to the device selected by a_deviceID.

  \fn     int hdFalconOpen(int a_deviceID)
  \param  a_deviceID device identification number. 
  \rteurn Return 0 if success, otherwise -1.
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdFalconOpen(int a_deviceID)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID > FALCON_NUM_DEVICES_MAX)) { return (-1); }

	// init device
	if (!falconDevices[a_deviceID].initialized)
	{
		falconDevices[a_deviceID].handle = hdlInitIndexedDevice(a_deviceID);
		falconDevices[a_deviceID].initialized = true;
	}

	// enable device
	falconDevices[a_deviceID].enabled = true;

	// return result
	return (falconDevices[a_deviceID].handle);
}


//==========================================================================
/*!
  Closes a connexion to the device selected by a_deviceID.

  \fn     int hdFalconClose(int a_deviceID)
  \param  a_deviceID device identification number. 
  \rteurn Return 0 if success, otherwise -1.
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdFalconClose(int a_deviceID)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID > FALCON_NUM_DEVICES_MAX)) { return (-1); }

	// disable device
	falconDevices[a_deviceID].enabled = false;
    
    // exit
    return (0);
}


//==========================================================================
/*!
  Returns the position of the device end-effector.

  \fn       int hdFalconGetPosition(int a_deviceID, 
									double *a_posX,
									double *a_posY,
				     				double *a_posZ)
  \param  a_deviceID device identification number.
  \param  double a_posX Component X of position vector.
  \parma  double a_posY Component Y of position vector.
  \param  double a_posZ Component Z of position vector.
  \rteurn Return 0 if success, otherwise -1.
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdFalconGetPosition(int a_deviceID, 
														double *a_posX,
														double *a_posY,
														double *a_posZ)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID > FALCON_NUM_DEVICES_MAX)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!falconDevices[a_deviceID].enabled) { return (-1); }

	// get position
	*a_posX = falconDevices[a_deviceID].position[2];
	*a_posY = falconDevices[a_deviceID].position[0];
	*a_posZ = falconDevices[a_deviceID].position[1];

	// success
	return (0);
}


//==========================================================================
/*!
  Returns the orientation matrix (frame) of the device end-effector.

  \fn     int hdFalconGetRotation(int a_deviceID, 
									double *a_rot00,
									double *a_rot01,
									double *a_rot02,
									double *a_rot10,
									double *a_rot11,
									double *a_rot12,
									double *a_rot20,
									double *a_rot21,
									double *a_rot22)
  \param  a_deviceID device identification number.
  \param  double a_rot00 Component [0,0] of rotation matrix.
  \parma  double a_rot01 Component [0,1] of rotation matrix.
  \param  double a_rot02 Component [0,2] of rotation matrix.
  \param  double a_rot10 Component [1,0] of rotation matrix.
  \parma  double a_rot11 Component [1,1] of rotation matrix.
  \param  double a_rot12 Component [1,2] of rotation matrix.
  \param  double a_rot20 Component [2,0] of rotation matrix.
  \parma  double a_rot21 Component [2,1] of rotation matrix.
  \param  double a_rot22 Component [2,2] of rotation matrix.
  \rteurn Return 0 if success, otherwise -1

*/
//==========================================================================
__declspec(dllexport) int __stdcall hdFalconGetRotation(int a_deviceID, 
						double *a_rot00,
						double *a_rot01,
						double *a_rot02,
						double *a_rot10,
						double *a_rot11,
						double *a_rot12,
						double *a_rot20,
						double *a_rot21,
						double *a_rot22)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID > FALCON_NUM_DEVICES_MAX)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!falconDevices[a_deviceID].enabled) { return (-1); }

	// return identity matrix
	*a_rot00 = 1.0;	  *a_rot01 = 0.0 ;	 *a_rot02 = 0.0;
	*a_rot10 = 0.0;	  *a_rot11 = 1.0 ;	 *a_rot12 = 0.0;
	*a_rot20 = 0.0;	  *a_rot21 = 0.0 ;	 *a_rot22 = 1.0;

	// success
	return (0);
}


//==========================================================================
/*!
  Read the values of each end-effector user button.

  \fn     int hdFalconGetButtons(int a_deviceID)
  \param  a_deviceID device identification number.
  \return Return a integer for which each bit corresponds to a button status.
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdFalconGetButtons(int a_deviceID)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID > FALCON_NUM_DEVICES_MAX)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!falconDevices[a_deviceID].enabled) { return (-1); }

	// return value
	return (falconDevices[a_deviceID].button);
}


//==========================================================================
/*!
	Send a force to the device

    \fn       int hdSetForce(int a_deviceID, 
						     double &a_forceX,
					 		 double &a_forceY,
							 double &a_forceZ)
	\param  a_deviceID device identification number.
	\param  double a_forceX Component X of force vector.
	\parma  double a_forceY Component Y of force vector.
	\param  double a_forceZ Component Z of force vector.
	\return Return 0 if success, otherwise -1
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdFalconSetForce(int a_deviceID, 
					 double *a_forceX,
			         double *a_forceY,
			         double *a_forceZ)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID > FALCON_NUM_DEVICES_MAX)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!falconDevices[a_deviceID].enabled) { return (-1); }

	// set position
	falconDevices[a_deviceID].force[2] = *a_forceX;
	falconDevices[a_deviceID].force[0] = *a_forceY;
	falconDevices[a_deviceID].force[1] = *a_forceZ;

	// success
	return (0);
}



//==========================================================================
// FUNCTIONS INTERNAL TO THE DLL
//==========================================================================

//==========================================================================
/*
	servo controller callback

	\fn     HDLServoOpExitCode servoFalconDevices(void* pUserData)
	\param  pUserData pointer to user data information (not used here)
*/
//==========================================================================
HDLServoOpExitCode servoFalconDevices(void* pUserData)
{
	for (int i=0; i<FALCON_NUM_DEVICES_MAX; i++)
	{
		// for each activated Falcon device
		if (falconDevices[i].enabled)
		{
			// activate ith device
			hdlMakeCurrent(falconDevices[i].handle);

			// update position
			hdlToolPosition(falconDevices[i].position);

			// update switch
			hdlToolButtons(&falconDevices[i].button);
		
			// update force
			hdlSetToolForce(falconDevices[i].force);
		}
	}

	return (HDL_SERVOOP_CONTINUE);
}


//==========================================================================
/*
	initialize servo controller

	\fn     void initServo()
*/
//==========================================================================
void initServo()
{
	if (!servoStarted)
	{
		// servo controller has been started
		servoStarted = true;

		// start servo controller
		hdlStart();	

		// create callback
		hdlCreateServoOp(servoFalconDevices,
                         NULL,
                         false);
	}
}
