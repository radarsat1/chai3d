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
#ifndef hdPhantomH
#define hdPhantomH
//---------------------------------------------------------------------------
//#ifdef __cplusplus
//extern "C" {
//#endif
//---------------------------------------------------------------------------

//===========================================================================
// API
//===========================================================================

__declspec(dllexport) int __stdcall hdPhantomGetNumDevices();

__declspec(dllexport) int __stdcall hdPhantomOpen(int a_deviceID);

__declspec(dllexport) int __stdcall hdPhantomClose(int a_deviceID);

__declspec(dllexport) int __stdcall hdPhantomGetPosition(int a_deviceID, 
								double *a_posX,
		    					double *a_posY,
								double *a_posZ);

__declspec(dllexport) int __stdcall hdPhantomGetLinearVelocity(int a_deviceID, 
								double *a_velX,
		    					double *a_velY,
								double *a_velZ);

__declspec(dllexport) int __stdcall hdPhantomGetRotation(int a_deviceID, 
								double *a_rot00,
								double *a_rot01,
								double *a_rot02,
								double *a_rot10,
								double *a_rot11,
								double *a_rot12,
								double *a_rot20,
								double *a_rot21,
								double *a_rot22);

__declspec(dllexport) int __stdcall hdPhantomGetButtons(int a_deviceID);

__declspec(dllexport) int __stdcall hdPhantomSetForce(int a_deviceID, 
							  double *a_forceX,
							  double *a_forceY,
							  double *a_forceZ);

__declspec(dllexport) int __stdcall hdPhantomSetTorque(int a_deviceID, 
							  double *a_torqueX,
							  double *a_torqueY,
							  double *a_torqueZ);

__declspec(dllexport) int __stdcall hdPhantomGetWorkspaceRadius(int a_deviceID, 
							  double *a_workspaceRadius);

__declspec(dllexport) int __stdcall hdPhantomGetType(int a_deviceID, 
													 char* a_typeName);


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
