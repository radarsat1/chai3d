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
    \author:    Francois Conti
    \version    #CHAI_VERSION#
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "stdafx.h"
#include "assert.h"
#include <conio.h>
//---------------------------------------------------------------------------
#include "hdPhantom.h"
#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>
//---------------------------------------------------------------------------
#include "chai3d.h"
//---------------------------------------------------------------------------

// =============================================================================
// Global variables
// =============================================================================

// maximum number of Phantom devices
#define PHANTOM_NUM_DEVICES_MAX 2

// stucture used to store data related to each device entity
struct CPhantomDeviceStatus
{
    HHD handle;
    double position[3];
	double linearVelocity[3];
	double rotation[9];
    double force[3];
	double torque[3];
    int    button;
	bool   enabled;
	bool   initialized;
	double workspaceRadius;
};

// has dll been initialed
bool initPhantomDLL = false;

// table containing information for each device.
CPhantomDeviceStatus phantomDevices[PHANTOM_NUM_DEVICES_MAX];

// predefined value that expresses the absence of a Phantom.
int numPhantomDevices = 0;

// has servo controller been started yet
bool servoStarted = false;

// main servo controller callback
//HDLServoOpExitCode servophantomDevices(void* pUserData);
HDCallbackCode servoCallbackHandle;

// initialize servo controller
void initServo();


//=============================================================================
// DLL entry point
// =============================================================================

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
           )
{
	if (!initPhantomDLL)
	{
		initPhantomDLL = true;

		for (int i=0; i<PHANTOM_NUM_DEVICES_MAX; i++)
		{
			// init button data
			phantomDevices[i].button = 0;

			// init position data
			phantomDevices[i].position[0] = 0.0;
			phantomDevices[i].position[1] = 0.0;
			phantomDevices[i].position[2] = 0.0;

			// init rotation data
			phantomDevices[i].rotation[0] = 1.0;
			phantomDevices[i].rotation[1] = 0.0;
			phantomDevices[i].rotation[2] = 0.0;
			phantomDevices[i].rotation[3] = 0.0;
			phantomDevices[i].rotation[4] = 1.0;
			phantomDevices[i].rotation[5] = 0.0;
			phantomDevices[i].rotation[6] = 0.0;
			phantomDevices[i].rotation[7] = 0.0;
			phantomDevices[i].rotation[8] = 1.0;
			
			// init force data
			phantomDevices[i].force[0] = 0.0;
			phantomDevices[i].force[1] = 0.0;
			phantomDevices[i].force[2] = 0.0;
			
			// init torque data
			phantomDevices[i].torque[0] = 0.0;
			phantomDevices[i].torque[1] = 0.0;
			phantomDevices[i].torque[2] = 0.0;

			// init enable/disable data
			phantomDevices[i].enabled = false;

			// init phantom api initialized
			phantomDevices[i].initialized = false;
		}

		//------------------------------------------------------------------
		// INITIALIZE DEVICES
		//------------------------------------------------------------------	
		HDErrorInfo error;
		numPhantomDevices = 0;

		// search for a first device
		HHD hHD0 = hdInitDevice(HD_DEFAULT_DEVICE);

		// check if device is available
		if (HD_DEVICE_ERROR(error = hdGetError())) 
		{
			return(true);
		}

		// enable forces
		hdMakeCurrentDevice(hHD0);
		hdEnable(HD_FORCE_OUTPUT);

		// add device to list
		phantomDevices[0].handle = hHD0;
		phantomDevices[0].enabled = true;
		numPhantomDevices++;

		// search for a possible second device
		HHD hHD1 = hdInitDevice("Phantom2");

		// check if device is available
		if (HD_DEVICE_ERROR(error = hdGetError())) 
		{
			return(true);
		}

		// enable forces
		hdMakeCurrentDevice(hHD1);
		hdEnable(HD_FORCE_OUTPUT);

		// add device to list
		phantomDevices[1].handle = hHD0;
		phantomDevices[1].enabled = true;
		numPhantomDevices++;
	}
	return (true);
}


//==========================================================================
// FUNCTIONS ACCESSIBLE FROM OUTSIDE
//==========================================================================

//==========================================================================
/*!
  Retrieves the number of devices of type phantom.

  \fn     int hdPhantomGetNumDevices()
  \return returns the number of devices found      

*/
//==========================================================================
__declspec(dllexport) int __stdcall hdPhantomGetNumDevices() 
{
	return (numPhantomDevices);
}


//==========================================================================
/*!
  Open a connexion to the device selected by a_deviceID.

  \fn     int hdPhantomOpen(int a_deviceID)
  \param  a_deviceID device identification number. 
  \rteurn Return 0 if success, otherwise -1.
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdPhantomOpen(int a_deviceID)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID >= numPhantomDevices)) { return (-1); }

	// init device
	if (!phantomDevices[a_deviceID].initialized)
	{
		phantomDevices[a_deviceID].initialized = true;
	}

	// enable device
	phantomDevices[a_deviceID].enabled = true;

	// return result
	return (phantomDevices[a_deviceID].handle);
}


//==========================================================================
/*!
  Closes a connexion to the device selected by a_deviceID.

  \fn     int hdPhantomClose(int a_deviceID)
  \param  a_deviceID device identification number. 
  \rteurn Return 0 if success, otherwise -1.
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdPhantomClose(int a_deviceID)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID >= numPhantomDevices)) { return (-1); }

	// disable device
	phantomDevices[a_deviceID].enabled = false;

    // exit
    return (0);
}


//==========================================================================
/*!
  Returns the position of the device end-effector.

  \fn       int hdPhantomGetPosition(int a_deviceID, 
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
__declspec(dllexport) int __stdcall hdPhantomGetPosition(int a_deviceID, 
														double *a_posX,
														double *a_posY,
														double *a_posZ)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID >= numPhantomDevices)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!phantomDevices[a_deviceID].enabled) { return (-1); }

	// get position
	*a_posX = phantomDevices[a_deviceID].position[2];
	*a_posY = phantomDevices[a_deviceID].position[0];
	*a_posZ = phantomDevices[a_deviceID].position[1];

	// success
	return (0);
}


//==========================================================================
/*!
  Returns the linear velocity of the device end-effector.

  \fn       int hdPhantomGetLinearVelocity(int a_deviceID, 
								           double *a_posX,
										   double *a_posY,
				     				       double *a_posZ)
  \param  a_deviceID device identification number.
  \param  double a_velX Component X of velocity vector.
  \parma  double a_velY Component Y of velocity vector.
  \param  double a_velZ Component Z of velocity vector.
  \rteurn Return 0 if success, otherwise -1.
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdPhantomGetLinearVelocity(int a_deviceID, 
								double *a_velX,
		    					double *a_velY,
								double *a_velZ)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID >= numPhantomDevices)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!phantomDevices[a_deviceID].enabled) { return (-1); }

	// get position
	*a_velX = phantomDevices[a_deviceID].linearVelocity[2];
	*a_velY = phantomDevices[a_deviceID].linearVelocity[0];
	*a_velZ = phantomDevices[a_deviceID].linearVelocity[1];

	// success
	return (0);
}


//==========================================================================
/*!
  Returns the orientation matrix (frame) of the device end-effector.

  \fn     int hdPhantomGetRotation(int a_deviceID, 
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
__declspec(dllexport) int __stdcall hdPhantomGetRotation(int a_deviceID, 
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
	if ((a_deviceID < 0) || (a_deviceID >= numPhantomDevices)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!phantomDevices[a_deviceID].enabled) { return (-1); }

	// return rotation matrix
	/*
	*a_rot00 = phantomDevices[a_deviceID].rotation[8];	
	*a_rot01 = phantomDevices[a_deviceID].rotation[2];	
	*a_rot02 = phantomDevices[a_deviceID].rotation[5];	
	*a_rot10 = phantomDevices[a_deviceID].rotation[6];	
	*a_rot11 = phantomDevices[a_deviceID].rotation[0];	
	*a_rot12 = phantomDevices[a_deviceID].rotation[3];	
	*a_rot20 = phantomDevices[a_deviceID].rotation[7];	
	*a_rot21 = phantomDevices[a_deviceID].rotation[1];	
	*a_rot22 = phantomDevices[a_deviceID].rotation[4];	
	*/

	// read value from matrix and correct matrix to be orthogonal
	// unfortunately there seems be some small errors coming
	// from the OpenHaptics library.
	cVector3d v0, v1, v2;
	v0.set( phantomDevices[a_deviceID].rotation[8],
			phantomDevices[a_deviceID].rotation[6],
			phantomDevices[a_deviceID].rotation[7]);
	v1.set( phantomDevices[a_deviceID].rotation[2],
			phantomDevices[a_deviceID].rotation[0],
			phantomDevices[a_deviceID].rotation[1]);
	v0.normalize();
	v1.normalize();
	v0.crossr(v1, v2);
	v2.crossr(v0, v1);

	*a_rot00 = v0.x;	
	*a_rot01 = v1.x; 
	*a_rot02 = v2.x; 
	*a_rot10 = v0.y; 
	*a_rot11 = v1.y;
	*a_rot12 = v2.y;
	*a_rot20 = v0.z; 
	*a_rot21 = v1.z; 
	*a_rot22 = v2.z;

	// success
	return (0);
}


//==========================================================================
/*!
  Read the values of each end-effector user button.

  \fn     int hdPhantomGetButtons(int a_deviceID)
  \param  a_deviceID device identification number.
  \return Return a integer for which each bit corresponds to a button status.
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdPhantomGetButtons(int a_deviceID)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID >= numPhantomDevices)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!phantomDevices[a_deviceID].enabled) { return (-1); }

	// return value
	return (phantomDevices[a_deviceID].button);
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
__declspec(dllexport) int __stdcall hdPhantomSetForce(int a_deviceID, 
					 double *a_forceX,
			         double *a_forceY,
			         double *a_forceZ)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID >= numPhantomDevices)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!phantomDevices[a_deviceID].enabled) { return (-1); }

	// set force
	phantomDevices[a_deviceID].force[2] = *a_forceX;
	phantomDevices[a_deviceID].force[0] = *a_forceY;
	phantomDevices[a_deviceID].force[1] = *a_forceZ;

	// success
	return (0);
}


//==========================================================================
/*!
	Send a torque to the device

    \fn       int hdSetForce(int a_deviceID, 
						     double &a_torqueX,
					 		 double &a_torqueY,
							 double &a_torqueZ)
	\param  a_deviceID device identification number.
	\param  double a_torqueX Component X of torque vector.
	\parma  double a_torqueY Component Y of torque vector.
	\param  double a_torqueZ Component Z of torque vector.
	\return Return 0 if success, otherwise -1
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdPhantomSetTorque(int a_deviceID, 
							  double *a_torqueX,
							  double *a_torqueY,
							  double *a_torqueZ)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID >= numPhantomDevices)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!phantomDevices[a_deviceID].enabled) { return (-1); }

	// set torque
	phantomDevices[a_deviceID].torque[2] = *a_torqueX;
	phantomDevices[a_deviceID].torque[0] = *a_torqueY;
	phantomDevices[a_deviceID].torque[1] = *a_torqueZ;

	// success
	return (0);
}


//==========================================================================
/*!
	Send a force to the device

    \fn     int hdPhantomGetWorkspaceRadius(int a_deviceID, double *a_workspaceRadius)
	\param  a_deviceID device identification number.
	\param  a_workspaceRadius  Radius of the workspace
	\return Return 0 if success, otherwise -1
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdPhantomGetWorkspaceRadius(int a_deviceID, double *a_workspaceRadius)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID >= numPhantomDevices)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!phantomDevices[a_deviceID].enabled) { return (-1); }

	// retrieve handle
	HHD hHD = phantomDevices[a_deviceID].handle;

	// activate ith device
	hdMakeCurrentDevice(hHD);

	// read workspace of device
	double size[6];
	hdGetDoublev(HD_USABLE_WORKSPACE_DIMENSIONS, size);
	double sizeX = size[3] - size[0];
	double sizeY = size[4] - size[1];
	double sizeZ = size[5] - size[2];
	double radius = 0.5 * sqrt(sizeX * sizeX +
				     		   sizeY * sizeY +
					           sizeZ * sizeZ);

	// convert value to [m]
	phantomDevices[a_deviceID].workspaceRadius = 0.001 * radius;

	// return estimated workspace radius
	*a_workspaceRadius = phantomDevices[a_deviceID].workspaceRadius;

	// success
	return (0);
}


//==========================================================================
/*!
	Read the name type of the device

    \fn     __declspec(dllexport) int __stdcall hdPhantomGetType(int a_deviceID, 
													 const char* a_typeName)
	\param  a_deviceID device identification number.
	\param  a_typeName  String containing the the device type
	\return Return 0 if success, otherwise -1
*/
//==========================================================================
__declspec(dllexport) int __stdcall hdPhantomGetType(int a_deviceID, 
													 char* a_typeName)
{
	// check id
	if ((a_deviceID < 0) || (a_deviceID >= numPhantomDevices)) { return (-1); }

	// check if servo started
	if (!servoStarted) { initServo(); }

	// check if enabled
	if (!phantomDevices[a_deviceID].enabled) { return (-1); }

	// retrieve handle
	HHD hHD = phantomDevices[a_deviceID].handle;

	// activate ith device
	hdMakeCurrentDevice(hHD);

	// read device model
	const char* typeName = hdGetString(HD_DEVICE_MODEL_TYPE);
	strcpy(a_typeName, typeName);

    // exit
    return (0);
}


//==========================================================================
// FUNCTIONS INTERNAL TO THE DLL
//==========================================================================

//==========================================================================
/*
	servo controller callback

	\fn     HDLServoOpExitCode servophantomDevices(void* pUserData)
	\param  pUserData pointer to user data information (not used here)
*/
//==========================================================================
HDCallbackCode HDCALLBACK servophantomDevices(void* pUserData)
{
	for (int i=0; i<PHANTOM_NUM_DEVICES_MAX; i++)
	{
		// for each activated phantom device
		if (phantomDevices[i].enabled)
		{
			// retrieve handle
			HHD hHD = phantomDevices[i].handle;

			// activate ith device
			hdMakeCurrentDevice(hHD);

			// start sending commands
			hdBeginFrame(hHD);
			
			// retrieve the position and orientation of the end-effector.
			double frame[16];
			hdGetDoublev(HD_CURRENT_TRANSFORM, frame);

			// convert position from [mm] to [m] 
			frame[12] = frame[12] * 0.001;
			frame[13] = frame[13] * 0.001;
			frame[14] = frame[14] * 0.001;

			phantomDevices[i].position[0] = frame[12];
			phantomDevices[i].position[1] = frame[13];
			phantomDevices[i].position[2] = frame[14];

			phantomDevices[i].rotation[0] = frame[0];
			phantomDevices[i].rotation[1] = frame[1];
			phantomDevices[i].rotation[2] = frame[2];
			phantomDevices[i].rotation[3] = frame[4];
			phantomDevices[i].rotation[4] = frame[5];
			phantomDevices[i].rotation[5] = frame[6];
			phantomDevices[i].rotation[6] = frame[8];
			phantomDevices[i].rotation[7] = frame[9];
			phantomDevices[i].rotation[8] = frame[10];

			// read linear velocity
			double vel[3];
			hdGetDoublev(HD_CURRENT_VELOCITY, vel);

			// convert position from [mm] to [m] 
			vel[0] = vel[0] * 0.001;
			vel[1] = vel[1] * 0.001;
			vel[2] = vel[2] * 0.001;
			
			phantomDevices[i].linearVelocity[0] = vel[0];
			phantomDevices[i].linearVelocity[1] = vel[1];
			phantomDevices[i].linearVelocity[2] = vel[2];

			// read user buttons
			int buttons;
			hdGetIntegerv(HD_CURRENT_BUTTONS, &buttons);
			phantomDevices[i].button = buttons;

			// send force to end-effector
			double force[3];
			force[0] = phantomDevices[i].force[0];
			force[1] = phantomDevices[i].force[1];
			force[2] = phantomDevices[i].force[2];
			hdSetDoublev(HD_CURRENT_FORCE, force);

			// send torque to end-effector
			double torque[3];
			torque[0] = phantomDevices[i].torque[0];
			torque[1] = phantomDevices[i].torque[1];
			torque[2] = phantomDevices[i].torque[2];
			hdSetDoublev(HD_CURRENT_GIMBAL_TORQUE, torque);

			// flush commands
			hdEndFrame(hHD);
		}
	}

	return (HD_CALLBACK_CONTINUE);
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

		// create callback
		HDCallbackCode servoCallbackHandle = hdScheduleAsynchronous(
		servophantomDevices, NULL, HD_DEFAULT_SCHEDULER_PRIORITY);

		// start scheduler
		hdStartScheduler();
	}
}



