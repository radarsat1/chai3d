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
#include "CGenericTool3dof.h"
//---------------------------------------------------------------------------

//==========================================================================
/*!
      Constructor of cGenericTool3dof.

      \fn       cGenericTool3dof::cGenericTool3dof()
*/
//===========================================================================
cGenericTool3dof::cGenericTool3dof()
{
    // set workspace size
    m_halfWorkspaceAxisX  = 0.1;
    m_halfWorkspaceAxisY = 0.1;
    m_halfWorkspaceAxisZ  = 0.1;

    // init device related variables
    m_deviceGlobalPos.zero();
    m_deviceLocalPos.zero();
    m_lastComputedLocalForce.zero();
    m_lastComputedGlobalForce.zero();
    m_deviceLocalVel.zero();
    m_deviceGlobalVel.zero();
    m_button = false;
}


//==========================================================================
/*!
      The virtual workspace defines the volume in which the virtual tool
      will be moving.

      \fn       void cGenericTool3dof::setWorkspace(double a_width,
                double a_height, double a_depth)
      \param    a_width   Width of workspace.
      \param    a_height  Height of workspace.
      \param    a_depth   Depth of workspace.
*/
//===========================================================================
void cGenericTool3dof::setWorkspace(double a_workspaceAxisX,
    double a_workspaceAxisY, double a_workspaceAxisZ)
{
    m_halfWorkspaceAxisX = a_workspaceAxisX / 2.0;
    m_halfWorkspaceAxisY = a_workspaceAxisY / 2.0;
    m_halfWorkspaceAxisZ = a_workspaceAxisZ / 2.0;    
}

