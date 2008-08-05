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
#include "CDeltaDevices.h"
#include "CVector3d.h"
#include "CMatrix3d.h"
#include "dhdc.h"
//---------------------------------------------------------------------------
const double DELTA_DEVICE_WORKSPACE_HALF_SIZE  = 0.15;  // [meters]
const double DELTA_DEVICE_MAXIMUM_FORCE        = 20.0;  // [newtons]
const double OMEGA_DEVICE_WORKSPACE_HALF_SIZE  = 0.075; // [meters]
const double OMEGA_DEVICE_MAXIMUM_FORCE        = 12.0;  // [newtons]
//---------------------------------------------------------------------------

// I am having serious problems linking to the delta libraries on some machines,
// with the important consequence being that some machines don't run CHAI
// binaries linked against dhdms.lib.  So I'm adding a #define to remove all
// dhd references, so I can temporarily build binaries that run anywhere.
//
// This should be commented out by default, so delta support actually
// works.
// #define NUKE_REFERENCES_TO_DHDMS

//===========================================================================
/*!
    Constructor of cDeltaDevice.

    \fn     cDeltaDevice::cDeltaDevice()
*/
//===========================================================================
cDeltaDevice::cDeltaDevice(unsigned int a_deviceNumber)
{
    m_systemReady = false;
    m_deviceID = a_deviceNumber;

    int numDevices = 0;

#ifndef NUKE_REFERENCES_TO_DHDMS
    numDevices = dhdGetDeviceCount();
#endif

    if ((a_deviceNumber + 1) > (unsigned int)numDevices)
    {
        m_systemAvailable = false;
    }
    else
    {
        m_systemAvailable = true;
    }
}


//===========================================================================
/*!
    Destructor of cDeltaDevice.

    \fn         cDeltaDevice::~cDeltaDevice()
*/
//===========================================================================
cDeltaDevice::~cDeltaDevice()
{
    // close connection to device
    if (m_systemReady)
    {
        close();
    }
}


//===========================================================================
/*!
    Open connection to delta device.

    \fn     int cDeltaDevice::open()
*/
//===========================================================================
int cDeltaDevice::open()
{

   if (m_systemReady) return true;

   int result = -1;

#ifndef NUKE_REFERENCES_TO_DHDMS
   result = dhdOpenID(m_deviceID);
#endif

   if (result == 0)
   {
        m_systemReady = true;
        m_systemAvailable = true;
   }
   else
   {
        m_systemReady = false;
        m_systemAvailable = false;
   }
   return (result);
}


//===========================================================================
/*!
    Close connection to delta device.

    \fn     int cDeltaDevice::close()
*/
//===========================================================================
int cDeltaDevice::close()
{
    int result = -1;
    
#ifndef NUKE_REFERENCES_TO_DHDMS
    result = dhdClose(m_deviceID);
#endif

    return (result);
}


//===========================================================================
/*!
    Calibrate delta device.

    \fn     int cDeltaDevice::initialize()
*/
//===========================================================================
int cDeltaDevice::initialize()
{

    int result = -1;

#ifndef NUKE_REFERENCES_TO_DHDMS
    result = dhdReset(m_deviceID);
#endif

    return (result);
}


//===========================================================================
/*!
    Set command for the delta device

    \fn         int cDeltaDevice::command(int a_command, void* a_data)
    \param      a_command  Selected command.
    \param      a_data  Pointer to the corresponding data structure.
    \return     Return status of command.
*/
//===========================================================================
int cDeltaDevice::command(int a_command, void* a_data)
{
    int result = CHAI_MSG_OK;
    double x=0.0,y=0.0,z=0.0;

    if (m_systemReady)
    {
        switch (a_command)
        {
            // read position of delta device
            case CHAI_CMD_GET_POS_3D:
            {
#ifndef NUKE_REFERENCES_TO_DHDMS
                dhdGetPosition(&x, &y, &z, m_deviceID);
#endif
                cVector3d* position = (cVector3d *) a_data;
                position->set(x, y, z);
            }
            break;

            case CHAI_CMD_GET_POS_NORM_3D:
            {
#ifndef NUKE_REFERENCES_TO_DHDMS
                dhdGetPosition(&x, &y, &z, m_deviceID);
#endif
                cVector3d* position = (cVector3d *) a_data;
                position->set(x, y, z);
                position->div(DELTA_DEVICE_WORKSPACE_HALF_SIZE);
            }
            break;

            // read orientation angles of delta wrist
            case CHAI_CMD_GET_ROT_ANGLES:
            {
                cVector3d* angles = (cVector3d *) a_data;
                angles->set(0, 0, 0);
#ifndef NUKE_REFERENCES_TO_DHDMS
                dhdGetOrientationRad(&angles->x, &angles->y, &angles->z, m_deviceID);
#endif
            }
            break;

            // read orientation matrix of wrist
            case CHAI_CMD_GET_ROT_MATRIX:
            {
                cMatrix3d* matrix = (cMatrix3d *) a_data;
                matrix->identity();
            }
            break;

            // set normalized force to delta device
            case CHAI_CMD_SET_FORCE_3D:
            {
                cVector3d* force = (cVector3d *) a_data;
#ifndef NUKE_REFERENCES_TO_DHDMS
                dhdSetForce(force->x, force->y, force->z, m_deviceID);
#endif
            }
            break;

            // set torque to delta wrist
            case CHAI_CMD_SET_TORQUE_3D:
            {
                cVector3d* torque = (cVector3d *) a_data;
#ifndef NUKE_REFERENCES_TO_DHDMS
                dhdSetTorque(torque->x, torque->y, torque->z, m_deviceID);
#endif
            }
            break;

            // read user switch from wrist
            case CHAI_CMD_GET_SWITCH_0:
            {
                int* result = (int *) a_data;
#ifndef NUKE_REFERENCES_TO_DHDMS
                *result = dhdGetButton(0, m_deviceID);
#endif
            }
            break;

            // function is not implemented for delta devices
            default:
                result = CHAI_MSG_NOT_IMPLEMENTED;
        }
    }
    else
    {
        result = CHAI_MSG_SYSTEM_NOT_READY;
    }
    return (result);
}


