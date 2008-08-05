//===========================================================================
/*
    This file is part of the CHAI 3D visualization and haptics libraries.
    Copyright (C) 2003-2008 by CHAI 3D. All rights reserved.

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
    \date       06/2007
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CFalconDeviceH
#define CFalconDeviceH

#ifndef _DISABLE_FALCON_DEVICE_SUPPORT
//---------------------------------------------------------------------------
#include "CGenericDevice.h"
#include "CPrecisionClock.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    \class cFalconDevice
    \brief
    Interface to the Falcon haptic device
*/
//===========================================================================
class cFalconDevice : public cGenericDevice
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cFalconDevics.
    cFalconDevice(unsigned int a_deviceNumber = 0);

    //! Destructor of cFalconDevics.
    virtual ~cFalconDevice();

    // METHODS:
    //! Open connection to Falcon device.
    virtual int open();

    //! Close connection to Falcon device
    virtual int close();

    //! Calibrate Falcon device.
    virtual int initialize(const bool a_resetEncoders=false);

    //! Set a command to the Falcon device
    virtual int command(int a_command, void* a_data);


  protected:

    //! Half size of the workspace (in meters)
    double m_halfSizeWorkspace;

    //! Maximum forces
    double m_maximumForces;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

#endif
