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
#include "CImplicitCylinder.h"
#include "CDraw3D.h"

#pragma unmanaged


//===========================================================================
/*!
    Constructor of cImplicitCylinder

    \fn       cImplicitCylinder::cImplicitCylinder(const double& a_radius)
    \param    a_radius   Radius of the cylinder
*/
//===========================================================================
cImplicitCylinder::cImplicitCylinder(const double& a_radius) : cImplicitShape()
{
    m_radius = cAbs(a_radius);
    m_draw_height = 6.0;
};


//===========================================================================
/*!
    Evaluate the cylinder's implicit function, x^2 + y^2 - r^2, at 
    given a_point

    \fn       float cImplicitCylinder::S(cVector3d a_point)
    \param    a_point   Point at which to evaluate the implicit function
    \return   The value of the implicit function at the given point
*/
//===========================================================================
float cImplicitCylinder::S(cVector3d a_point)
{
    return (a_point.x*a_point.x + a_point.y*a_point.y - m_radius*m_radius);
}


//===========================================================================
/*!
    Evaluate the gradient of the cylinder's implicit function, [2x, 2y, 0], 
    at given a_point

    \fn       cVector3d cImplicitCylinder::dS(cVector3d a_point)
    \param    a_point  Point at which to evaluate the gradient
    \return   The value of the gradient at the given point
*/
//===========================================================================
cVector3d cImplicitCylinder::dS(cVector3d a_point)
{
    return (cVector3d(2*a_point.x, 2*a_point.y, 0.0));
}


//===========================================================================
/*!
    Render the cylinder graphically

    \fn       void cImplicitCylinder::render(const int a_renderMode)
    \param    a_renderMode  Ignored in this implementation
*/
//===========================================================================
void cImplicitCylinder::render(const int a_renderMode)
{
    m_material.render();

    cMatrixGL frame;
    frame.set(cVector3d(0,0,-m_draw_height/2.0), cIdentity3d());
    frame.glMatrixPushMultiply();

    GLUquadricObj *cylinder;
    cylinder = gluNewQuadric();
    gluQuadricDrawStyle (cylinder, GLU_FILL);
    gluQuadricNormals (cylinder, GLU_SMOOTH);
    gluQuadricTexture(cylinder, GL_TRUE);
    gluCylinder(cylinder, m_radius, m_radius, m_draw_height, 36, 36);
    gluDeleteQuadric(cylinder);
    glDisable(GL_TEXTURE_2D);

    frame.glMatrixPop();

	if (m_state == 1)
	{
	    glPushMatrix();
	    glTranslated(m_surface_point.x, m_surface_point.y, m_surface_point.z);
	    cDrawSphere(0.05, 8, 8);
	    glPopMatrix();
	}
}


