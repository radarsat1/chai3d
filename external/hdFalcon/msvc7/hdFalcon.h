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
#ifndef hdFalconH
#define hdFalconH
//---------------------------------------------------------------------------
//#ifdef __cplusplus
//extern "C" {
//#endif
//---------------------------------------------------------------------------

//===========================================================================
// API
//===========================================================================

__declspec(dllexport) int __stdcall hdFalconGetNumDevices();

__declspec(dllexport) int __stdcall hdFalconOpen(int a_deviceID);

__declspec(dllexport) int __stdcall hdFalconClose(int a_deviceID);

__declspec(dllexport) int __stdcall   hdFalconGetPosition(int a_deviceID, 
								double *a_posX,
		    					double *a_posY,
								double *a_posZ);

__declspec(dllexport) int __stdcall   hdFalconGetRotation(int a_deviceID, 
								double *a_rot00,
								double *a_rot01,
								double *a_rot02,
								double *a_rot10,
								double *a_rot11,
								double *a_rot12,
								double *a_rot20,
								double *a_rot21,
								double *a_rot22);

__declspec(dllexport) int __stdcall hdFalconGetButtons(int a_deviceID);

__declspec(dllexport) int __stdcall   hdFalconSetForce(int a_deviceID, 
							  double *a_forceX,
							  double *a_forceY,
							  double *a_forceZ);

__declspec(dllexport) int __stdcall   hdFalconSetTorque(int a_deviceID, 
							  double *a_torqueX,
							  double *a_torqueY,
							  double *a_torqueZ);

__declspec(dllexport) double __stdcall hdFalconGetWorkspaceRadius(int a_deviceID);


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------