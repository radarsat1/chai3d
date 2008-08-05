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
    //! Constructor of cGenericTool3dof.
    cGenericTool3dof();
    //! Destructor of cGenericTool3dof.
    virtual ~cGenericTool3dof() {};

    // METHODS:
    //! Set virtual workspace dimensions in which tool will be working.
    void setWorkspace(double a_workspaceAxisX, double a_workspaceAxisY,
                      double a_workspaceAxisZ);
    
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
    //! Velocity of device in device local coordinate system
    cVector3d m_deviceLocalVel;
    //! Velocity of device in world global coordinate system
    cVector3d m_deviceGlobalVel;
    //! Last computed force between tool and world in world coordinate system.
    //! If you want to manually send forces to a device, you can modify this
    //! value before calling 'applyForces'.
    cVector3d m_lastComputedGlobalForce;
    //! Last computed force in device coordinate system.
    cVector3d m_lastComputedLocalForce;
    //! user switch.
    bool m_button;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------



