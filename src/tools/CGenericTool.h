//=========================================================================
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
#ifndef CGenericToolH
#define CGenericToolH
//---------------------------------------------------------------------------
#include "windows.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include "CGenericObject.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class      cGenericTool
      \brief      cGenericTool describes a generic class to create virtual
                  tools inside a virtual environment (cWorld) and connecting
                  them to haptic devices.
*/
//===========================================================================
class cGenericTool : public cGenericObject
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cGenericTool.
    cGenericTool() {};
    //! Destructor of cGenericTool.
    virtual ~cGenericTool() {};

    // METHODS:
    //! Update Position, orientation, velocity and other degree of freedoms of tool.
    virtual void updatePose() {};
    //! Compute interaction forces with environment.
    virtual void computeForces() {};
    //! Apply latest forces to device.
    virtual void applyForces() {};
    //! Stop system. apply no forces to device.
    virtual void stop() {};
    //! Render the object in OpenGL.
    virtual void render(const int a_renderMode=0) {};
    //! Start the device connected to the tool
    virtual void start() {};
    //! Initializes encoders on device connected to the tool
    virtual void initialize() {};
    //! Toggles forces on
    virtual void ForcesON() {};
    //! Toggles forces off
    virtual void ForcesOFF() {};
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

