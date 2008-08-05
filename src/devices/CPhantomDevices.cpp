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

//---------------------------------------------------------------------------
#include "CPhantomDevices.h"
#include <windows.h>
//---------------------------------------------------------------------------
int cPhantomDevice::m_num_phantoms = 0;
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cPhantomDevice. \r
    No servo loop is yet created, encoders are NOT reset.

    \fn     cPhantomDevice::cPhantomDevice(int a_num, bool a_dio_access)

    \param  num  0-based index of the Phantom that this
                 tool should talk to.
    \param  dio_access  If this is 'true', the tool will use the
            Ghost API's direct-i/o model, if it's available.  Otherwise
            the gstEffect i/o model will be used.
*/
//===========================================================================
cPhantomDevice::cPhantomDevice(int a_num, bool a_dio_access)
{
    HINSTANCE checkVal;

    // load phantom driver
    checkVal = LoadLibrary("PhantomDriver.dll");

    // check if driver is available
    if (checkVal == NULL)
    {
        m_systemReady = false;
        m_systemAvailable = false;
    }
    else
    {
        if (a_dio_access)
        {
            PhantomAcces(1);
        }

        m_phantomHandle = OpenPhantom(a_num);

        if (m_phantomHandle < 0)
        {
            m_systemReady = false;
            m_systemAvailable = false;
        }
        else
        {
            m_systemReady = false;
            m_systemAvailable = true;
            m_num_phantoms++;
        }
    }
}


//===========================================================================
/*!
    Destructor of cPhantomDevice.

    \fn     cPhantomDevice:~cPhantomDevice()
*/
//===========================================================================
cPhantomDevice::~cPhantomDevice()
{
    if (m_systemReady)
    {
        StopCommunicationPhantom(m_phantomHandle);
        m_num_phantoms--;
        if (m_num_phantoms == 0)
        {
            ClosePhantoms();
        }
    }
}


//===========================================================================
/*!
    Open connection to phantom device.

    \fn     int cPhantomDevice::open()
    \return Return 0 is operation succeeds, -1 if an error occurs.
*/
//===========================================================================
int cPhantomDevice::open()
{
    if (m_systemAvailable)
    {
        StartCommunicationPhantom(m_phantomHandle);
        m_systemReady = true;
        return 0;
    }

    return -1;
}


//===========================================================================
/*!
    Close connection to phantom device.

    \fn     int cPhantomDevice::close()
    \return Return 0 is operation succeeds, -1 if an error occurs.
*/
//===========================================================================
int cPhantomDevice::close()
{
    if (m_systemReady)
    {
        StopCommunicationPhantom(m_phantomHandle);
        return 0;
    }

    return -1;
}


//===========================================================================
/*!
    Calibrate phantom device. Desktop devices that have been initialized with
    Dice will not be initialized. For Phantom Premiums it is fundamental to place
    the device in its reset position before calling init.

    \fn     int cPhantomDevice::initialize()
    \return Return 0 is operation succeeds, -1 if an error occurs.
*/
//===========================================================================
int cPhantomDevice::initialize()
{
    if (m_systemReady)
    {
        ResetPhantomEncoders(m_phantomHandle);
        return 0;
    }

    return -1;
}


//===========================================================================
/*!
    Set command for the phantom device

    \fn     int cPhantomDevice::command(int iCommand, void* iData)
    \param  iCommand    Selected command.
    \param  iData       Pointer to the corresponding data structure.
    \return Return status of command.
*/
//===========================================================================
int cPhantomDevice::command(int a_command, void* a_data)
{
    int result = CHAI_MSG_OK;

    if (m_systemReady)
    {
       switch (a_command)
       {
           // read position of phantom device
           case CHAI_CMD_GET_POS_3D:
           {
                cVector3d* position = (cVector3d *) a_data;
                // Note: we're doing a change in reference here to pass from the phantom
                // convention to the CHAI one.
                ReadPositionPhantom(m_phantomHandle, position->y, position->z, position->x);
           }
           break;

           // read normalized position of phantom device
           case CHAI_CMD_GET_POS_NORM_3D:
           {
                cVector3d* position = (cVector3d *) a_data;
                // Note: we're doing a change in reference here to pass from the phantom
                // convention to the CHAI one.
                ReadNormalizedPositionPhantom(m_phantomHandle, position->y, position->z, position->x);
           }
           break;

           // read velocity of phantom device
           case CHAI_CMD_GET_VEL_3D:
           {
                cVector3d* velocity = (cVector3d *) a_data;
                // Note: we're doing a change in reference here to pass from the phantom
                // convention to the CHAI one.
                ReadVelocityPhantom(m_phantomHandle, velocity->y, velocity->z, velocity->x);
           }
           break;

           // read orientation matrix of phantom
           case CHAI_CMD_GET_ROT_MATRIX:
           {
                cMatrix3d* mat3 = (cMatrix3d *) a_data;
                double m[9];
                // Note: we're doing a change in reference here to pass from the phantom
                // convention to the CHAI one.
                ReadOrientMat3DOFPhantom(m_phantomHandle,m);
                mat3->set(m[8], m[2], m[5], m[6], m[0], m[3], m[7], m[1], m[4]);
           }
           break;

           // set force to phantom device
           case CHAI_CMD_SET_FORCE_3D:
           {
                cVector3d *force;
                force = (cVector3d *) a_data;
                SetForcePhantom(m_phantomHandle, force->y, force->z, force->x);
           }
           break;

           // set force and torque to phantom stylus
           case CHAI_CMD_SET_FORCE_TORQUE_3D:
           {
                cVector3d *genforce;
                genforce = (cVector3d *) a_data;
                SetForceTorquePhantom(m_phantomHandle,
                             genforce[0].y, genforce[0].z, genforce[0].x,
                             genforce[1].y, genforce[1].z, genforce[1].x);
           }
           break;

           // read user switch from phantom stylus
           case CHAI_CMD_GET_SWITCH_0:
           {
                int* result = (int *) a_data;
                *result = ReadSwitchPhantom(m_phantomHandle);
           }
           break;

           // function is not implemented for phantom devices
           default:
                result = CHAI_MSG_NOT_IMPLEMENTED;
        }
    }
    else
    {
        result = CHAI_MSG_SYSTEM_NOT_READY;
    }

    return result;
}


