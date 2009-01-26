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
    \author:    Stephen Sinclair (MPBT)
    \version    1.0
    \date       05/2005
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CLibNIFalconDevice.h"
//---------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#endif
#include "CVector3d.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cLibNIFalconDevice.
    Loads interface DLL.

    \fn     cLibNIFalconDevice::cLibNIFalconDevice()
*/
//===========================================================================
cLibNIFalconDevice::cLibNIFalconDevice() : cGenericDevice()
{
    m_systemReady = false;
    m_systemAvailable = false;

    m_activeLibNIFalconDevices++;

    m_systemAvailable = true;
}

//===========================================================================
/*!
    Destructor of cLibNIFalconDevice.

    \fn     cLibNIFalconDevice::~cLibNIFalconDevice()
*/
//===========================================================================
cLibNIFalconDevice::~cLibNIFalconDevice()
{
    m_activeLibNIFalconDevices--;
}

//===========================================================================
/*!
    Open connection to LibNIFalcon device.

    \fn     int cLibNIFalconDevice::open()
    \return Return 0 is operation succeeds, -1 if an error occurs.
*/
//===========================================================================
int cLibNIFalconDevice::open()
{
   return 0;
}

//===========================================================================
/*!
    Close connection to LibNIFalcon device.

    \fn     int cLibNIFalconDevice::close()
    \return Return 0 is operation succeeds, -1 if an error occurs.
*/
//===========================================================================
int cLibNIFalconDevice::close()
{
   return 0;
}

//===========================================================================
/*!
    Calibrate LibNIFalcon device. Initializes the driver, loading appropriate
  settings according to current LibNIFalcon configuration.

    \fn     int cLibNIFalconDevice::initialize(const bool a_resetEncoders=false)
    \param  a_resetEncoders Ignored; exists for forward compatibility.
    \return Return 0 is operation succeeds, -1 if an error occurs.
*/
//===========================================================================
int cLibNIFalconDevice::initialize(const bool a_resetEncoders)
{
    if (m_hf6s != 0)
        return -1;

    F6SRC rc = f6s_Initialize(&m_hf6s);
    if (m_hf6s && rc == F6SRC_NOERROR)
    {  
        // Joint velocity computation:
        //   timestep = 1ms
        //   sample buffer size = 15
        f6s_ComputeJointVel(m_hf6s, 0.001f, 15);
        return 0;
    }

    m_hf6s = 0;
    return -1;
}

//===========================================================================
/*!
    Set command for the LibNIFalcon device

    \fn     int cLibNIFalconDevice::command(int iCommand, void* iData)
    \param  iCommand    Selected command.
    \param  iData       Pointer to the corresponding data structure.
    \return Return status of command.
*/
//===========================================================================
int cLibNIFalconDevice::command(int a_command, void* a_data)
{
  if (m_hf6s==0)
    return CHAI_MSG_SYSTEM_NOT_READY;

  cVector3d *v;
  double kinemat[16];
  double force[3];
  double torque[3];
  double velLinear[3], velAngular[3];
  torque[0] = torque[1] = torque[2] = 0;

  switch (a_command)
  {
  case CHAI_CMD_GET_POS_3D:
  case CHAI_CMD_GET_POS_NORM_3D:
    f6s_UpdateKinematics(m_hf6s);
    f6s_GetPositionMatrixGL(m_hf6s, kinemat);

    // kinemat is a row-major 4x4 rotation/translation matrix

    v = (cVector3d*)a_data;

    v->x = kinemat[14];
    v->y = kinemat[12];
    v->z = kinemat[13];

    // workspace is approximately from -0.1 to 0.1 meters.
    if (a_command == CHAI_CMD_GET_POS_NORM_3D) {
      v->div(0.02);
    }

    // convert to mm
    else {
      v->mul(1000.0);
    }

    break;

  case CHAI_CMD_SET_FORCE_3D:
    v = (cVector3d*)a_data;

    // coordinates need to be changed for the LibNIFalcon API
    // torque is assumed to be zero

    force[0] = v->y;
    force[1] = -v->x;
    force[2] = v->z;
    f6s_SetForceTorque(m_hf6s, force, torque);

    break;

  case CHAI_CMD_GET_VEL_3D:
    f6s_GetVelocityGL(m_hf6s, velLinear, velAngular);

    v = (cVector3d*)a_data;
    v->x = velLinear[2];
    v->y = velLinear[0];
    v->z = velLinear[1];

    // Now convert from m/s to mm/s
    v->mul(1000.0);

    break;

  // read scale factor from normalized coords to mm
  case CHAI_CMD_GET_NORMALIZED_SCALE_FACTOR:   
    {
    double* scale = (double*)a_data;

    // Multiply .1 to get meters back
    *scale = 0.1;

    // Then multiply by 1000 to get millimeters
    *scale *= 1000.0;
    }
    break;

  default:
    return CHAI_MSG_NOT_IMPLEMENTED;
  }

  return CHAI_MSG_OK;
}
