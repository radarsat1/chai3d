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
#ifndef CDeltaDevicesH
#define CDeltaDevicesH
//---------------------------------------------------------------------------
#include "CGenericDevice.h"
//---------------------------------------------------------------------------

class cDeltaDevice : public cGenericDevice
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cDeltaDevices.
    cDeltaDevice(unsigned int a_deviceNumber = 0);
    //! Destructor of cGenericDevice.
    virtual ~cDeltaDevice();

    // METHODS:
    //! Open connection to delta device.
    virtual int open();
    //! Close connection to delta device
    virtual int close();
    //! Calibrate delta device.
    virtual int initialize();
    //! Set a command to the delta device
    virtual int command(int a_command, void* a_data);

  private:
    //! Device ID number
    int m_deviceID;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
