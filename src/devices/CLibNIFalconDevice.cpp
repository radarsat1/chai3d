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

#ifndef _DISABLE_LIBNIFALCON_DEVICE_SUPPORT

//---------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#endif
#include "CVector3d.h"
//---------------------------------------------------------------------------
#include <falcon/core/FalconDevice.h>
#include <falcon/comm/FalconCommLibUSB.h>
#include <falcon/comm/FalconCommLibFTDI.h>
#include <falcon/firmware/FalconFirmwareNovintSDK.h>
#include <falcon/util/FalconFirmwareBinaryNvent.h>
#include <falcon/kinematic/FalconKinematicStamper.h>
//---------------------------------------------------------------------------

// Initialize reference count
int cLibNIFalconDevice::m_activeLibNIFalconDevices = 0;

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
    \return Return 0 is operation succeeds, -1 if an error
 occurs.
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
	m_device = new libnifalcon::FalconDevice();
	libnifalcon::FalconDevice *device = (libnifalcon::FalconDevice*)m_device;
    device->setFalconFirmware<libnifalcon::FalconFirmwareNovintSDK>();
    device->setFalconComm<libnifalcon::FalconCommLibFTDI>();
    if (!device->open(0)) {
        printf("libnifalcon: Couldn't open device 0 (libftdi).\n");
        return -1;
    }

    printf("libnifalcon: Uploading firmware.\n");
    int i;
    for (i=0; i<10 && !device->isFirmwareLoaded(); i++) {
        if (device->getFalconFirmware()->loadFirmware(
                true, libnifalcon::NOVINT_FALCON_NVENT_FIRMWARE_SIZE,
                const_cast<uint8_t*>(libnifalcon::NOVINT_FALCON_NVENT_FIRMWARE))
            && device->isFirmwareLoaded())
            break;
        printf(".");
        fflush(stdout);
    }

    if (i==10) {
        printf("libnifalcon: Couldn't upload device firmware.\n");

        printf("libnifalcon: Error Code: %d\n", device->getErrorCode());
        if (device->getErrorCode() == 2000)
            printf("libnifalcon: Device Error Code: %d\n",
                   device->getFalconComm()->getDeviceErrorCode());

        return -1;
    }

	device->close();

    device->setFalconComm<libnifalcon::FalconCommLibUSB>();
    if (!device->open(0)) {
        printf("libnifalcon: Couldn't open device 0 (libusb).\n");
        return -1;
    }

    device->setFalconKinematic<libnifalcon::FalconKinematicStamper>();

    return 0;
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
  libnifalcon::FalconDevice *device = (libnifalcon::FalconDevice*)m_device;

  if (!device->isOpen())
    return CHAI_MSG_SYSTEM_NOT_READY;

  double *pos=0;
  double force[3] = {0.0, 0.0, 0.0};
  cVector3d *v = (cVector3d*)a_data;

  switch (a_command)
  {
  case CHAI_CMD_GET_POS_3D:
  case CHAI_CMD_GET_POS_NORM_3D:
    device->runIOLoop();
    pos = device->getPosition();
    v->y = pos[0];
    v->z = pos[1];
    v->x = pos[2] - 0.1;
    if (a_command == CHAI_CMD_GET_POS_NORM_3D)
        *v *= 60.0;
    break;

  case CHAI_CMD_SET_FORCE_3D:
    force[0] = v->y;
    force[1] = v->z;
    force[2] = v->x;
    device->setForce(force);
    break;

  case CHAI_CMD_GET_VEL_3D:
    // TODO
    break;

  // read scale factor from normalized coords to mm
  case CHAI_CMD_GET_NORMALIZED_SCALE_FACTOR:   
    {
    double* scale = (double*)a_data;

    *scale = 60.0;
    }
    break;

  default:
    return CHAI_MSG_NOT_IMPLEMENTED;
  }

  return CHAI_MSG_OK;
}

#endif // _DISABLE_LIBNIFALCON_DEVICE_SUPPORT
