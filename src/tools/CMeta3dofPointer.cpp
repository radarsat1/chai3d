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
    \version    1.1
    \date       01/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CMeta3dofPointer.h"
#include <process.h>
//---------------------------------------------------------------------------

//==========================================================================
/*!
      Constructor of cMeta3dofPointer. 

      \fn       cMeta3dofPointer::cMeta3dofPointer(cWorld* a_world,
								unsigned int a_deviceNumber, bool dio_access)
      \param    a_world  World in which the tool will operate.
      \param    a_deviceNumber  0-based index used to try to open a phantom
								device.
      \param    dio_access  Also used only if the tool ends up being a phantom.
				If this is 'true', the tool will use the Ghost API's direct-i/o
				model, if it's available.  Otherwise the gstEffect i/o model will
				be used.
*/
//===========================================================================
cMeta3dofPointer::cMeta3dofPointer(cWorld* a_world, unsigned int a_deviceNumber, bool a_dio_access)
    :cGeneric3dofPointer(a_world)
{
    bool systemAvailable;

    // try to open a connection to an Delta or Omega haptic device
    m_device = new cDeltaDevice(a_deviceNumber);
    systemAvailable = m_device->isSystemAvailable();

    if (systemAvailable)
    {
        m_device->open();
        return;
    }
    else
    {
        delete m_device;
        m_device = NULL;
    }

    // try to open phantom driver
    m_device = new cPhantomDevice(a_deviceNumber, a_dio_access);
    systemAvailable = m_device->isSystemAvailable();

    if (systemAvailable)
    {
        m_device->open();
        return;
    }
    else
    {
        delete m_device;
        m_device = NULL;
    }

    // try to open a virtual haptic device
    m_device = new cVirtualDevice();
    systemAvailable = m_device->isSystemAvailable();

    if (systemAvailable)
    {
        m_device->open();
        return;
    }
    else
    {
        // launch a virtual device
        m_device->close();
        delete m_device;
        m_device = NULL;

        // dvdVirtual.exe is expected to be in the system path
        spawnl(P_NOWAIT, "dhdVirtual.exe", "dhdVirtual.exe", NULL);

        // Wait for the virtual delta process to start up...
        Sleep(1000);
        m_device = new cVirtualDevice();
        systemAvailable = m_device->isSystemAvailable();

        if (!systemAvailable)
        {
            m_device->close();
            delete m_device;
            m_device = NULL;
            return;
        }
    }
}


//==========================================================================
/*!
      Destructor of cMeta3dofPointer.

      \fn       cMeta3dofPointer::~cMeta3dofPointer()
*/
//===========================================================================
cMeta3dofPointer::~cMeta3dofPointer()
{

}
