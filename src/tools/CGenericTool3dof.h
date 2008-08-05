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
#ifndef CGenericTool3dofH
#define CGenericTool3dofH
//---------------------------------------------------------------------------
#include "CGenericTool.h"
#include "CMaths.h"
#include "CConstants.h"
#include "CGenericPointForceAlgo.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class      cGenericTool3dof
      \brief      cGenericTool3dof describes a generic class which describes
                  a three degree freedom tool.
*/
//===========================================================================
class cGenericTool3dof : public cGenericTool
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of CVirtualTool3dof.
    cGenericTool3dof();
    //! Destructor of CVirtualTool3dof.
    virtual ~cGenericTool3dof() {};

    // METHODS:
    //! Update Position, orientation, velocity and other degree of freedoms of haptic tool
    virtual void updatePose() {};
    //! Compute interaction forces with environment.
    virtual void computeForces() {};
    //! Apply latest computed forces to device.
    virtual void applyForces() {};
    //! Stop system. apply no forces to device.
    virtual void stop() {};
    //! Render the tool in OpenGL.
    virtual void render(const int a_renderMode) {};
    //! Start the haptic device connected to the tool
    virtual void start() {};
    //! Initializes encoders on device connected to the tool
    virtual void initialize() {};
    //! Set virtual workspace dimensions in which tool will be working.
    void setWorkspace(double a_workspaceAxisX, double a_workspaceAxisY,
                      double a_workspaceAxisZ);
    //! Toggles forces on
    virtual void ForcesON() {};
    //! Toggles forces off
    virtual void ForcesOFF() {};


    // PROPERTIES:
    //! Width of workspace.
    double m_halfWorkspaceAxisX;
    //! Height of workspace.
    double m_halfWorkspaceAxisY;
    //! Depth of workspace.
    double m_halfWorkspaceAxisZ;
    //! Position of device in device local coordinate system.
    cVector3d m_deviceLocalPos;
    //! Position of device in world global coordinate system
    cVector3d m_deviceGlobalPos;
    //! Velocity of device in world global coordinate system
    cVector3d m_deviceGlobalVel;
    //! Last computed force between tool and world in world coordinate system.
    cVector3d m_lastComputedGlobalForce;
    //! Last computed force in device coordinate system.
    cVector3d m_lastComputedLocalForce;
    //! user switch.
    bool m_button;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------



