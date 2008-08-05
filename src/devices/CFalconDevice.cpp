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
#include "CFalconDevice.h"
//---------------------------------------------------------------------------
#ifndef _DISABLE_FALCON_DEVICE_SUPPORT
//---------------------------------------------------------------------------
#include <windows.h>
#include "CMaths.h"
//---------------------------------------------------------------------------

HINSTANCE dhdlcDLL = NULL;

int (__stdcall *dhdlcOpen)                       ();
int (__stdcall *dhdlcClose)                      ();
int (__stdcall *dhdlcGetButton)                  ();
int (__stdcall *dhdlcReset)                      ();
int (__stdcall *dhdlcGetPosition)                (double *px, double *py, double *pz);
int (__stdcall *dhdlcSetForce)                   (double  fx, double  fy, double  fz);
int (__stdcall *dhdlcUpdate)                     ();



//===========================================================================
/*!
    Constructor of cDeltaDevice.

    \fn     cFalconDevice::cFalconDevice(unsigned int a_deviceNumber)
*/
//===========================================================================
cFalconDevice::cFalconDevice(unsigned int a_deviceNumber)
{
    // init variable
    m_halfSizeWorkspace     = 0.075;
    m_maximumForces         = 5.0;

    // device is not yet available or ready
    m_systemAvailable = false;
    m_systemReady = false;

    // load dhdlc.dll library
    dhdlcDLL = LoadLibrary("C:\\Program Files\\Novint\\Falcon\\HDAL\\bin\\dhdlc.dll");

    // check if DLL loaded correctly
    if (dhdlcDLL == NULL)
    {
        return;
    }

    // load different callbacks
    dhdlcOpen        = (int (__stdcall*)())GetProcAddress(dhdlcDLL, "dhdOpen");
    dhdlcClose       = (int (__stdcall*)())GetProcAddress(dhdlcDLL, "dhdClose");
    dhdlcGetButton   = (int (__stdcall*)())GetProcAddress(dhdlcDLL, "dhdGetButton");
    dhdlcReset       = (int (__stdcall*)())GetProcAddress(dhdlcDLL, "dhdReset");
    dhdlcGetPosition = (int (__stdcall*)(double*, double*, double*))GetProcAddress(dhdlcDLL, "dhdGetPosition");
    dhdlcSetForce    = (int (__stdcall*)(double, double, double))GetProcAddress(dhdlcDLL, "dhdSetForce");
    dhdlcUpdate      = (int (__stdcall*)())GetProcAddress(dhdlcDLL, "dhdUpdate");

    // check if such device is available
    if (dhdlcOpen() >= 0)
    {
        m_systemAvailable = true;
    }
    else
    {
        m_systemAvailable = false;
    }

    dhdlcClose();
}


//===========================================================================
/*!
    Destructor of cFalconDevice.

    \fn         cFalconDevice::~cFalconDevice()
*/
//===========================================================================
cFalconDevice::~cFalconDevice()
{
    // close connection to device
    if (m_systemReady)
    {
        close();
    }


    if (dhdlcDLL != NULL)
    {
        FreeLibrary(dhdlcDLL);
        dhdlcDLL = 0;
    }
}


//===========================================================================
/*!
    Open connection to delta device.

    \fn     int cFalconDevice::open()
*/
//===========================================================================
int cFalconDevice::open()
{
    // check if the system is available
    if (!m_systemAvailable) return (-1);

    // if system is already opened then return
    if (m_systemReady) return (0);

    // try to open the device
    int result = dhdlcOpen();

    // update device status
    if (result == 0)
    {
        m_systemReady = true;
        return(0);
    }
    else
    {
        m_systemReady = false;
        return (-1);
    }
}


//===========================================================================
/*!
    Close connection to delta device.

    \fn     int cFalconDevice::close()
*/
//===========================================================================
int cFalconDevice::close()
{
    // check if the system has been opened previously
    if (!m_systemReady) return (-1);

    // yes, the device is open so let's close it
    int result = dhdlcClose();

    // update status
    m_systemReady = false;

    // exit
    return (result);
}


//===========================================================================
/*!
    Calibrate delta device. 
    
    This function does nothing right now; the a_resetEncoders parameter is ignored.

    \fn     int cFalconDevice::initialize(const bool a_resetEncoders = false)
    \param  a_resetEncoders Ignored; exists for forward compatibility.
    \return Always 0
*/
//===========================================================================
int cFalconDevice::initialize(const bool a_resetEncoders)
{
    // reset encoders
    return (0);
    //return (dhdlcReset());
}


//===========================================================================
/*!
    Send a command to the delta device

    \fn         int cFalconDevice::command(int a_command, void* a_data)
    \param      a_command  Selected command.
    \param      a_data  Pointer to the corresponding data structure.
    \return     Return status of command.  CHAI_MSG_OK is good, anything
                else is probably not good.
*/
//===========================================================================
int cFalconDevice::command(int a_command, void* a_data)
{
    // temp variables
    int result = CHAI_MSG_OK;
    double x=0.0,y=0.0,z=0.0;

    // check if device is open
    if (m_systemReady)
    {
        switch (a_command)
        {
            // read position of falcon device
            case CHAI_CMD_GET_POS_3D:
            {
                // read position from device
                cVector3d* position = (cVector3d *) a_data;
                dhdlcUpdate();
                dhdlcGetPosition(&x, &y, &z);
                position->set(x, y, z);
            }
            break;

            // read normalized position of the delta device
            case CHAI_CMD_GET_POS_NORM_3D:
            {
                // read position from device
                cVector3d* position = (cVector3d *) a_data;
                dhdlcUpdate();
                dhdlcGetPosition(&x, &y, &z);
                position->set(x, y, z);
                position->div(m_halfSizeWorkspace);
            }
            break;

            // read orientation angles of delta wrist
            case CHAI_CMD_GET_ROT_ANGLES:
            {
                cVector3d* angles = (cVector3d *) a_data;
                angles->set(0, 0, 0);
            }
            break;

            // read orientation matrix of wrist
            case CHAI_CMD_GET_ROT_MATRIX:
            {
                cMatrix3d frame;
                frame.identity();
                cMatrix3d* matrix = (cMatrix3d *) a_data;
                *matrix = frame;
            }
            break;

            // set force to device
            case CHAI_CMD_SET_FORCE_3D:
            {
                cVector3d* force = (cVector3d *) a_data;
                dhdlcSetForce(force->x, force->y, force->z);
            }
            break;


            // read user switch from wrist
            case CHAI_CMD_GET_SWITCH_0:
            {
                int* result = (int *) a_data;
                *result = dhdlcGetButton();
            }
            break;

            // read user switch from wrist
            case CHAI_CMD_GET_SWITCH_MASK:
            {
                int* result = (int *) a_data;

                // Force the result to be 0 or 1, since bit 0 should carry button 0's value
                *result = dhdlcGetButton() ? 1 : 0;
            }
            break;

            // read scale factor from normalized coords to mm
            case CHAI_CMD_GET_NORMALIZED_SCALE_FACTOR:
            {
                double* scale = (double*)a_data;

                // Multiply by m_halfSizeWorkspace to get meters back
                *scale = m_halfSizeWorkspace;

                result = CHAI_MSG_OK;
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

#endif
