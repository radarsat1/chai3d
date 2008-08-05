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
    \author:    Federico Barbagli
    \version    1.2
    \date       01/2004
*/
//===========================================================================



#include "stdafx.h"
#include "assert.h"
#define _BUILD_DLL_
#include <iostream.h>
#include "phantomHD.h"
#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>



// =============================================================================
// Constants
// =============================================================================
const int SUCCESS = 1;

// =============================================================================
// Global variables
// =============================================================================

// predefined value that expresses the absence of a phantom.
int numdev = 0;

// handle for the scheduler
HDSchedulerHandle gSchedulerCallback = HD_INVALID_HANDLE;

hduVector3Dd pos[2];
hduVector3Dd force[2];
hduVector3Dd vel[2];
hduVector3Dd torque[2];
typedef HDdouble matrix[16];
matrix mat[2];

HDint stylus_switch[2];

HHD myPhantom[2] = {HD_INVALID_HANDLE, HD_INVALID_HANDLE};


bool phantomStatus[2] = {false, false};
bool forceStatus[2] = {false, false};


double device_workspace_center_x[2];
double device_workspace_center_y[2];
double device_workspace_center_z[2];
double cube_side[2];

int model_type;


//=============================================================================
// Callbacks
//=============================================================================



HDCallbackCode HDCALLBACK HapticCallBack(void *data)
{
	int valid; 
	int single = 0;

	if (myPhantom[0] == HD_INVALID_HANDLE)
		single = 1;
	else
		valid = 0;

	if (myPhantom[1] == HD_INVALID_HANDLE)
		single = 1;
	else
		valid = 1;
		
	// if only one of the two devices
	if (single)
	{
		hdBeginFrame(myPhantom[valid]);
		
		// read stuff
		hdGetDoublev(HD_CURRENT_POSITION,pos[valid]);
		hdGetDoublev(HD_CURRENT_VELOCITY,vel[valid]);
		hdGetDoublev(HD_CURRENT_TRANSFORM,mat[valid]);
		hdGetIntegerv(HD_CURRENT_BUTTONS,&stylus_switch[valid]);
		// write forces
		hdSetDoublev(HD_CURRENT_FORCE, force[valid]);
		// if device is 6DOF render torques too
		HDint num_dof;
		hdGetIntegerv(HD_OUTPUT_DOF	,&num_dof);
		if ((int)num_dof == 6)
			hdSetDoublev(HD_CURRENT_TORQUE, torque[valid]);

	    hdEndFrame(myPhantom[valid]);

	}
	else	// else do it for both
	{
		hdBeginFrame(myPhantom[0]);
		hdGetDoublev(HD_CURRENT_POSITION,pos[0]);
		hdGetDoublev(HD_CURRENT_VELOCITY,vel[0]);
		hdGetDoublev(HD_CURRENT_TRANSFORM,mat[0]);
		hdGetIntegerv(HD_CURRENT_BUTTONS,&stylus_switch[0]);

		hdBeginFrame(myPhantom[1]);
		hdGetDoublev(HD_CURRENT_POSITION,pos[1]);
		hdGetDoublev(HD_CURRENT_VELOCITY,vel[1]);
		hdGetDoublev(HD_CURRENT_TRANSFORM,mat[1]);
		hdGetIntegerv(HD_CURRENT_BUTTONS,&stylus_switch[1]);


		hdMakeCurrentDevice(myPhantom[0]);
		hdSetDoublev(HD_CURRENT_FORCE, force[0]);
		HDint num_dof;
		hdGetIntegerv(HD_OUTPUT_DOF	,&num_dof);
		if ((int)num_dof == 6)
			hdSetDoublev(HD_CURRENT_TORQUE, torque[0]);
	    hdEndFrame(myPhantom[0]);

		hdMakeCurrentDevice(myPhantom[1]);
		hdSetDoublev(HD_CURRENT_FORCE, force[1]);
		hdGetIntegerv(HD_OUTPUT_DOF	,&num_dof);
		if ((int)num_dof == 6)
			hdSetDoublev(HD_CURRENT_TORQUE, torque[1]);
        hdEndFrame(myPhantom[1]);

    }

    return HD_CALLBACK_CONTINUE;
}




//=============================================================================
// DLL entry point
// =============================================================================

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{

	return TRUE;
}



// =============================================================================
// Functions accessible from outside
// =============================================================================


//==========================================================================
/*!
	This function opens starts a scene if no phantom has been previously defined.
	Also this function attempts to create a phantom instance with name "char * name".
	If two phantoms exist or if the name of the phantom is wrong a negative int
	is returned. Otherwise a handle for the phantom is returned (which will be
	0 or 1 in value).

      \fn       OpenPhantom(char * name)
	  \param	char * name      

*/
//===========================================================================


FUNCTION int __stdcall   OpenPhantom(char * name)
{
	//printErrorMessages(false);
	int ret_val;	

	if (numdev == 2)
	{
		// overflow error
		// in this case return without incrementing numdev
		return PH_OF;
	}

	// if phantom already exists we want to create one in the free spot we have
	if (numdev == 1)
	{
			myPhantom[1] = hdInitDevice(name);
			HDErrorInfo error;
			if (HD_DEVICE_ERROR(error = hdGetError()))
				return PH_INIT_ERR;
			else
				ret_val = 1;
    
			hdEnable(HD_FORCE_OUTPUT);
			hdEnable(HD_FORCE_RAMPING);
			// scheduler
		}

	if (numdev == 0)
	{
			myPhantom[0] = hdInitDevice(name);
			HDErrorInfo error;
			if (HD_DEVICE_ERROR(error = hdGetError()))
				return PH_INIT_ERR;
			else
				ret_val = 0;
    
			hdEnable(HD_FORCE_OUTPUT);
			hdEnable(HD_FORCE_RAMPING);
			// scheduler

	}
	//printErrorMessages(true);
	
	numdev++;
	int x_center, y_center, z_center;
	if (ret_val >= 0)
	{
		HDdouble usableBounds[6];
		hdGetDoublev(HD_USABLE_WORKSPACE_DIMENSIONS, usableBounds);

		// determine model type
		switch ((int)usableBounds[3])
		{
		case 80:
			{
				// desktop or Omni?
				if (usableBounds[4] == 70)
				{
					// desktop
					model_type = 0;
					// pick the center of the workspace
					x_center = 0;
					y_center = 10;
					z_center = 10;
					break;
				}
				else
				{
					// Omni
					model_type = 5;
					// pick the center of the workspace
					x_center = 0;
					y_center = 10;
					z_center = 10;
					break;
				}

			}
		case 85:
			{
				// premiumA 1.0
				model_type = 1;
				// pick the center of the workspace
				x_center = 0;
				y_center = 0;
				z_center = 0;
				break;
			}
		case 140:
			{
				// premium 1.5 or premium 3
				model_type = 2;
				// pick the center of the workspace
				x_center = 0;
				y_center = 10;
				z_center = 32;
				break;
			}
		case 130:
			// 6dof 1.5
			model_type = 3;
			break;
		default:
			// all other phantom models, we'll have a conservative approach with them
			model_type = 4;
		}

		// get dimensions of usable workspace
		int x_total = usableBounds[3] - usableBounds[0];
		int y_total = usableBounds[4] - usableBounds[1];
		int z_total = usableBounds[5] - usableBounds[2];


		// pick the smallest which will be the side of the cube
		int side;
		if ((x_total <= y_total) && (x_total <= z_total))
			side = x_total;
		else 
			if ((y_total <= x_total) && (y_total <= z_total))
				side = y_total;
			else 
				side = z_total;
		
		device_workspace_center_x[ret_val] = (double) x_center;
		device_workspace_center_y[ret_val] = (double) y_center;
		device_workspace_center_z[ret_val] = (double) z_center;
		cube_side[ret_val] = ((double) side) / 2.0;

		
	}

		
	// this is the handle for this phantom from the user perspective
	return ret_val;
}




//==========================================================================
/*!
	This function closes stops the servoloop for all phantoms. This should
	be the last opeartion called before exiting the program. Also OpenPhantom()
	should not be called after this point.

      \fn       void ClosePhantoms()
	  
*/
//===========================================================================


FUNCTION void __stdcall   ClosePhantoms()
{
	hdStopScheduler();
    hdUnschedule(gSchedulerCallback);
    if (myPhantom[0] != HD_INVALID_HANDLE)
    {
        hdDisableDevice(myPhantom[0]);
        myPhantom[0] = HD_INVALID_HANDLE;
    }

    if (myPhantom[1] != HD_INVALID_HANDLE)
    {
        hdDisableDevice(myPhantom[1]);
        myPhantom[1] = HD_INVALID_HANDLE;
    }
	
	numdev = 0;
}



//==========================================================================
/*!
	This function resets encoders of phantom num. Returns 1 in case of success, 
	a negative value (see error_codes.h) in case of error.

      \fn       int ResetPhantomEncoders(int num)
	  \param	int num

*/
//===========================================================================

FUNCTION int __stdcall  ResetPhantomEncoders(int num)
{
	if ((num < numdev) && (num >= 0) && (myPhantom[num] != HD_INVALID_HANDLE))	
	{
		int supportedCalibrationStyles;
		int calibrationStyle;
		
		hdGetIntegerv(HD_CALIBRATION_STYLE, &supportedCalibrationStyles);
		if (supportedCalibrationStyles & HD_CALIBRATION_ENCODER_RESET)
		{
			calibrationStyle = HD_CALIBRATION_ENCODER_RESET;
		}
		if(supportedCalibrationStyles & HD_CALIBRATION_INKWELL)
		{
			calibrationStyle = HD_CALIBRATION_INKWELL;
	    }
		if (supportedCalibrationStyles & HD_CALIBRATION_AUTO)
	    {
		    calibrationStyle = HD_CALIBRATION_AUTO;
		}

	    if (calibrationStyle == HD_CALIBRATION_ENCODER_RESET)
		{
	        hdUpdateCalibration(calibrationStyle);
		    if (hdCheckCalibration() == HD_CALIBRATION_OK)
				return SUCCESS;
			else 
				return PH_RES_ENC_ERR;
		}
		else 
			return SUCCESS;
	}
	else
		return PH_OF;
}


//==========================================================================
/*!
	This function starts effect on phantom i. Note that the servoloop is still
	running even if the effect is off. This is to allow one phantom to be off 
	while the other one is on.

      \fn       int StartCommunication(int i)
	  \param	int i
*/
//===========================================================================
FUNCTION int __stdcall   StartCommunication(int num)
{
	if ((num < numdev) && (num >= 0) && (myPhantom[num] != HD_INVALID_HANDLE))	
	{
	    if (gSchedulerCallback == HD_INVALID_HANDLE)
		{
			hdStartScheduler();
			HDErrorInfo error;
			if (HD_DEVICE_ERROR(error = hdGetError()))
			{
				return PH_ENABLE_FORCES;
			}
			
			gSchedulerCallback = hdScheduleAsynchronous(
				HapticCallBack, 0, HD_DEFAULT_SCHEDULER_PRIORITY);
			phantomStatus[num] = true;
			forceStatus[num] = false;
			return SUCCESS;
		}
		else
		{
			hdMakeCurrentDevice(myPhantom[num]);
			hdEnable(HD_FORCE_OUTPUT);
			phantomStatus[num] = true;
			forceStatus[num] = false;
			return SUCCESS;
		}
	}
	else
		return PH_OF;
}


//==========================================================================
/*!
	This function turns effect off on phantom i. Note that the servoloop is still
	running even if the effect is off. This is to allow one phantom to be off 	while the other one is on.

      \fn       int StopCommunication(int i)
	  \param	int i
*/
//===========================================================================

FUNCTION int __stdcall   StopCommunication(int num)
{
	if ((num < numdev) && (num >= 0) && (myPhantom[num] != HD_INVALID_HANDLE))	
	{
		phantomStatus[num] = false;
		hdMakeCurrentDevice(myPhantom[num]);
		hdDisable(HD_FORCE_OUTPUT);
		return SUCCESS;
	}
	else
		return PH_OF;
}


//==========================================================================
/*!
	This function reads the position of the tip of the phantom in cartesian space.
	the position vector is passed by reference. the function returns 1 in case of
	success, a negative value in case of error. Note that positions are returned
	in the phantom reference frames, i.e. x horizontal to the right, y is vertical
	upward, z points toward you. Changes of references may be necessary to work
	in chai.

      \fn       int ReadPosition(int num, 
									  double &iPosX,
									  double &iPosY,
									  double &iPosZ)
	  \param	int num		handle to the phantom
	  \param	double iPosX, double iPosY, double iPosZ: position vector
*/
//===========================================================================
FUNCTION int __stdcall   ReadPosition(int num, 
									  double &iPosX,
									  double &iPosY,
									  double &iPosZ)
{
	if ((num < numdev) && (num >= 0) && (myPhantom[num] != HD_INVALID_HANDLE))	
	{
		if (phantomStatus[num])
		{
			iPosX = pos[num][0];
			iPosY = pos[num][1];
			iPosZ = pos[num][2];
			return SUCCESS;
		}
		else
		{
			iPosX = 0.0;
			iPosY = 0.0;
			iPosZ = 0.0;
			return SUCCESS;
		}
		
	}
	else
		return PH_OF;
}

 

//==========================================================================
/*!
	ReadNormalizedPosition(int num,double &iPosX,double &iPosY,double &iPosZ); reads tip position for phantom num
	the function returns 1 if everything went ok, a negative value otherwise (check list of errors)
	Note that positions are expressed with a value included in the interval [-1,1] for a cube centered in the device's workspace center.
	This is to ensure that a same demo may be used using different devices without having to change any of the code

      \fn       int ReadNormalizedPosition(int num, 
									  double &iPosX,
									  double &iPosY,
									  double &iPosZ)
	  \param	int num		handle to the phantom
	  \param	double iPosX, double iPosY, double iPosZ: position vector
*/
//===========================================================================

FUNCTION int __stdcall   ReadNormalizedPosition(int num, 
									  double &iPosX,
									  double &iPosY,
									  double &iPosZ)
{
	if ((num < numdev) && (num >= 0) && (myPhantom[num] != HD_INVALID_HANDLE))	
	{
		if (phantomStatus[num])
		{
			iPosX = (pos[num][0] - device_workspace_center_x[num])/cube_side[num];
			iPosY = (pos[num][1] - device_workspace_center_y[num])/cube_side[num];
			iPosZ = (pos[num][2] - device_workspace_center_z[num])/cube_side[num];
			return SUCCESS;
		}
		else
		{
			iPosX = 0.0;
			iPosY = 0.0;
			iPosZ = 0.0;
			return SUCCESS;
		}
		
	}
	else
		return PH_OF;
}		


//==========================================================================
/*!
	This function sets the forces that will be applied to the tip of the phantom 
	in cartesian space. The force vector is passed by reference. the function 
	returns 1 in case of success, a negative value in case of error. Note that 
	forces are considered in the phantom reference frames, i.e. x horizontal to the right, y is vertical
	upward, z points toward you. Changes of references may be necessary to work
	in chai.

      \fn       int SetForce(int num, 
									  double &iForceX,
									  double &iForceY,
									  double &iForceZ)
	  \param	int num		handle to the phantom
	  \param	double iForceX, double iForceY, double iForceZ: force vector
*/
//===========================================================================
FUNCTION int __stdcall   SetForce(int num, 
								  const double &iForceX,
				                  const double &iForceY,
								  const double &iForceZ)
{
	// make sure the first forces we pass to the device are zero
	if ((fabs(iForceX) == 0.0) && (fabs(iForceY) == 0.0) && (fabs(iForceZ) == 0.0) && (!forceStatus[num]))
		forceStatus[num] = true;

	
	if ((num < numdev) && (num >= 0) && (myPhantom[num] != HD_INVALID_HANDLE))	
	{
		if ((phantomStatus[num]) && (forceStatus[num]))
		{
			force[num][0] = iForceX;
			force[num][1] = iForceY;
			force[num][2] = iForceZ;
			return SUCCESS;
		}
		else
		{
			force[num][0] = 0.0;
			force[num][1] = 0.0;
			force[num][2] = 0.0;
			return SUCCESS;
		}
	}
	else
		return PH_OF;

}


//==========================================================================
/*!
	This function sets the forces and torques that will be applied to the tip of the phantom 
	in cartesian space. The force and torque vectors are passed by reference. the function 
	returns 1 in case of success, a negative value in case of error. Note that 
	torque are considered with respect to the phantom reference frames, i.e. x horizontal to the right, y is vertical
	upward, z points toward you. Changes of references may be necessary to work
	in chai.

      \fn       int SetForceTorque(int num, 
								  const double &iForceX,
				                  const double &iForceY,
								  const double &iForceZ,
								  const double &iTorqueX,
				                  const double &iTorqueY,
								  const double &iTorqueZ)

	  \param	int num		handle to the phantom
	  \param	double iTorqueX, double iTorqueY, double iTorqueZ: Torque vector
*/
//===========================================================================
FUNCTION int __stdcall   SetForceTorque(int num, 
								  const double &iForceX,
				                  const double &iForceY,
								  const double &iForceZ,
								  const double &iTorqueX,
				                  const double &iTorqueY,
								  const double &iTorqueZ)
{
	if ((num < numdev) && (num >= 0) && (myPhantom[num] != HD_INVALID_HANDLE))	
	{
		if (phantomStatus[num])
		{
			force[num][0] = iForceX;
			force[num][1] = iForceY;
			force[num][2] = iForceZ;
			torque[num][0] = iTorqueX;
			torque[num][1] = iTorqueY;
			torque[num][2] = iTorqueZ;

			return SUCCESS;
		}
		else
		{
			force[num][0] = 0.0;
			force[num][1] = 0.0;
			force[num][2] = 0.0;
			torque[num][0] = 0.0;
			torque[num][1] = 0.0;
			torque[num][2] = 0.0;
			return SUCCESS;
		}
	}
	else
		return PH_OF;

}

//==========================================================================
/*!
	This function reads the orientation of the phantom stylus in the case of a 
	3dof wrist with respect to a chai reference frame. Returns 1 in case of success,
	a negative value in case of error.

      \fn       int ReadOrientMat3DOF(int num, 
 									  double *m)
	  \param	int num		handle to the phantom
	  \param	double *m: array representing matrix
*/
//===========================================================================


FUNCTION int __stdcall   ReadOrientMat3DOF(int num, 
 									  double *m)
{
	if ((num < numdev) && (num >= 0) && (myPhantom[num] != HD_INVALID_HANDLE))	
	{
		if (phantomStatus[num])
		{
			m[0] = mat[num][0];
			m[1] = mat[num][1];
			m[2] = mat[num][2];
			m[3] = mat[num][4];
			m[4] = mat[num][5];
			m[5] = mat[num][6];
			m[6] = mat[num][8];
			m[7] = mat[num][9];
			m[8] = mat[num][10];

			return SUCCESS;
		}
		else
		{
			m[0] = 1;
			m[1] = 0;
			m[2] = 0;
			m[3] = 0;
			m[4] = 1;
			m[5] = 0;
			m[6] = 0;
			m[7] = 0;
			m[8] = 1;
			return SUCCESS;
		}
		
	}
	else
		return PH_OF;

}



//==========================================================================
/*!
	This function reads the Phantom switch. Returns 0 or 1 if everything is
	fine, negative value in case of error.

      \fn       int ReadSwitch(int num)
	  \param	int num		handle to the phantom
*/
//===========================================================================
FUNCTION int __stdcall   ReadSwitch(int num)
{
	if ((num < numdev) && (num >= 0) && (myPhantom[num] != HD_INVALID_HANDLE))	
		return stylus_switch[num];
	else
		return PH_OF;
}




//==========================================================================
/*!
	This function reads the max force that can be applied by the Phantom switch. 


      \fn       double GetMaxForce(int num)
	  \param	int num		handle to the phantom
*/
//===========================================================================
FUNCTION double __stdcall   GetMaxForce(int num)
{
	hdMakeCurrentDevice(myPhantom[num]);
	HDdouble forceMagnitude;
    hdGetDoublev(HD_NOMINAL_MAX_FORCE, &forceMagnitude);
	return (double) forceMagnitude;
}




//==========================================================================
/*!
	This function reads the velocity of the tip of the phantom in cartesian space.
	the velocity vector is passed by reference. the function returns 1 in case of
	success, a negative value in case of error. Note that velocity is returned
	in mm/sec with respect to a phantom reference frames, i.e. x horizontal to the right, y is vertical
	upward, z points toward you. Changes of references may be necessary to work
	in chai.

      \fn       int ReadVelocity(int num, 
									  double &iVelX,
									  double &iVelY,
									  double &iVelZ)
	  \param	int num		handle to the phantom
	  \param	double iVelX, double iVelY, double iVelZ: velocity vector
*/
//===========================================================================
FUNCTION int __stdcall   ReadVelocity(int num, 
									  double &iVelX,
									  double &iVelY,
									  double &iVelZ)
{
	if ((num < numdev) && (num >= 0) && (myPhantom[num] != HD_INVALID_HANDLE))	
	{
		if (phantomStatus[num])
		{
			iVelX = vel[num][0];
			iVelY = vel[num][1];
			iVelZ = vel[num][2];
			return SUCCESS;
		}
		else
		{
			iVelX = 0.0;
			iVelY = 0.0;
			iVelZ = 0.0;
			return SUCCESS;
		}
		
	}
	else
		return PH_OF;
}

 
