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
    \author:    Force Dimension - www.forcedimension.com
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

//===========================================================================
/*!
    Constructor of cDeltaDevice.

    \fn     cDeltaDevice::cDeltaDevice()
*/
//===========================================================================
cDeltaDevice::cDeltaDevice(unsigned int a_deviceNumber)
{
    // device is not yet available or ready
    m_systemAvailable = false;
    m_systemReady = false;

    // get the number ID of the device we wish to communicate with
    m_deviceID = a_deviceNumber;

    // get the number of Force Dimension devices connected to this computer
    int numDevices = dhdGetDeviceCount();

    // check if such device is available
    if ((a_deviceNumber + 1) > (unsigned int)numDevices)
    {
        // no, such ID does not lead to an existing device
        m_systemAvailable = false;
    }
    else
    {
        // yes, this ID leads to an existing device
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
    // check if the system is available
    if (!m_systemAvailable) return (-1);

    // if system is already opened then return
    if (m_systemReady) return (0);

    // try to open the device
    int result = dhdOpenID(m_deviceID);

    // update device status
    if (result == 0)
    {
        m_systemReady = true;
    }
    else
    {
        m_systemReady = false;
        return (-1);
    }

    // read the device type
    int deviceType = dhdGetSystemType(m_deviceID);

    switch (deviceType)
    {
        case (DHD_DEVICE_3DOF):
        case (DHD_DEVICE_6DOF):
        case (DHD_DEVICE_6DOF_500):
        {
            m_halfSizeWorkspace = DELTA_DEVICE_WORKSPACE_HALF_SIZE;
            m_maximumForces     = DELTA_DEVICE_MAXIMUM_FORCE;
        }
        break;

        case (DHD_DEVICE_OMEGA):
        {
            m_halfSizeWorkspace = OMEGA_DEVICE_WORKSPACE_HALF_SIZE;
            m_maximumForces     = OMEGA_DEVICE_MAXIMUM_FORCE;
        }
        break;
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
    // check if the system has been opened previously
    if (!m_systemReady) return (-1);

    // yes, the device is open so let's close it
    int result = dhdClose(m_deviceID);

    // update status
    m_systemReady = false;

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
    return (0);
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
    // temp variables
    int result = CHAI_MSG_OK;
    double x=0.0,y=0.0,z=0.0;

    // check if device is open
    if (m_systemReady)
    {
        switch (a_command)
        {
            // read position of delta device
            case CHAI_CMD_GET_POS_3D:
            {
                dhdGetPosition(&x, &y, &z, m_deviceID);

                cVector3d* position = (cVector3d *) a_data;
                position->set(x, y, z);
            }
            break;

            // read normalized position of the delta device
            case CHAI_CMD_GET_POS_NORM_3D:
            {
                dhdGetPosition(&x, &y, &z, m_deviceID);

                cVector3d* position = (cVector3d *) a_data;
                position->set(x, y, z);
                position->div(m_halfSizeWorkspace);
            }
            break;

            // read orientation angles of delta wrist
            case CHAI_CMD_GET_ROT_ANGLES:
            {
                cVector3d* angles = (cVector3d *) a_data;
                angles->set(0, 0, 0);

                dhdGetOrientationRad(&angles->x, &angles->y, &angles->z, m_deviceID);
            }
            break;

            // read orientation matrix of wrist
            case CHAI_CMD_GET_ROT_MATRIX:
            {
                cVector3d angles;
                angles.set(0,0,0);

                dhdGetOrientationRad(&angles.x, &angles.y, &angles.z, m_deviceID);

                angles.mul(1.5);
                cMatrix3d* matrix = (cMatrix3d *) a_data;
                matrix->identity();
                matrix->rotate(cVector3d(1,0,0), angles.x);
                matrix->rotate(cVector3d(0,1,0), angles.y);
                matrix->rotate(cVector3d(0,0,1), angles.z);
            }
            break;

            // set force to delta device
            case CHAI_CMD_SET_FORCE_3D:
            {
                cVector3d* force = (cVector3d *) a_data;

                dhdSetForce(force->x, force->y, force->z, m_deviceID);
            }
            break;

            // set torque to delta wrist
            case CHAI_CMD_SET_TORQUE_3D:
            {
                cVector3d* torque = (cVector3d *) a_data;

                dhdSetTorque(torque->x, torque->y, torque->z, m_deviceID);
            }
            break;

            // read user switch from wrist
            case CHAI_CMD_GET_SWITCH_0:
            {
                int* result = (int *) a_data;

                *result = dhdGetButton(0, m_deviceID);
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


