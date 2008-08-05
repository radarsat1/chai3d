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
    \author:    Chris Sewell
    \version    1.0
    \date       05/2006
*/
//===========================================================================

#include "stdafx.h"
#include "CImplicitCone.h"
#include "glut.h"
#include "CDraw3D.h"

#pragma unmanaged


//===========================================================================
/*!
    Constructor of cImplicitCone

    \fn       cImplicitCone::cImplicitCone(const double& a_c)
    \param    a_c   Ratio of cone radius to height
*/
//===========================================================================
cImplicitCone::cImplicitCone(const double& a_c) : cImplicitShape()
{
    m_c = cAbs(a_c);
    m_draw_height = 3.0;
};


//===========================================================================
/*!
    Evaluate the cone's implicit function, x^2/c^2 + y^2/c^2 - z^2, with c
    being the radius to height ratio, at given a_point

    \fn       float cImplicitCone::S(cVector3d a_point)
    \param    a_point   Point at which to evaluate the implicit function
    \return   The value of the implicit function at the given point
*/
//===========================================================================
float cImplicitCone::S(cVector3d a_point)
{
    return (a_point.x*a_point.x)/(m_c*m_c) + (a_point.y*a_point.y)/(m_c*m_c) - a_point.z*a_point.z;
}


//===========================================================================
/*!
    Evaluate the gradient of the cone's implicit function, [2x/c^2, 2y/c^2, -2z], 
    at given a_point

    \fn       cVector3d cImplicitCone::dS(cVector3d a_point)
    \param    a_point  Point at which to evalute the gradient
    \return   The value of the gradient at the given point
*/
//===========================================================================
cVector3d cImplicitCone::dS(cVector3d a_point)
{
    return (cVector3d(2*a_point.x/(m_c*m_c), 2*a_point.y/(m_c*m_c), -2*a_point.z));
}


//===========================================================================
/*!
    Render the cone graphically

    \fn       void cImplicitCone::render(const int a_renderMode)
    \param    a_renderMode  Ignored in this implementation
*/
//===========================================================================
void cImplicitCone::render(const int a_renderMode)
{
    m_material.render();

    // Since glut renders a finite height single cone, we need to make it match
    // the implicit formula, which describes an infinite double-cone, we need 
    // to render two cones, and make them tall enough to go off the edge of
    // the screen
    glPushMatrix();
    glTranslated(0,0,-m_draw_height);
    glutSolidCone(m_c*m_draw_height, m_draw_height, 36, 36);
    glPopMatrix();

    // Rotate the second cone so that it is "upside down" for the second part
    // of the double cone
    glPushMatrix();
    glTranslated(0,0,m_draw_height);
    glRotated(180,1,0,0);
    glutSolidCone(m_c*m_draw_height, m_draw_height, 36, 36);
    glPopMatrix();

	if (m_state == 1)
	{
	    glPushMatrix();
	    glTranslated(m_surface_point.x, m_surface_point.y, m_surface_point.z);
	    cDrawSphere(0.05, 8, 8);
	    glPopMatrix();
	}
}

