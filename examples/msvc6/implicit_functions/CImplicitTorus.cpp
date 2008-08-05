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
#include "CImplicitTorus.h"
#include "CDraw3D.h"
#include "glut.h"


#pragma unmanaged


//===========================================================================
/*!
    Constructor of cImplicitTorus

    \fn       cImplicitTorus::cImplicitTorus(const double& a_outer_radius,
	                                         const double& a_inner_radius)
    \param    a_outer_radius   Outer radius of the torus
    \param    a_inner_radius   Inner radius of the torus
*/
//===========================================================================
cImplicitTorus::cImplicitTorus(const double& a_outer_radius, const double& a_inner_radius) : cImplicitShape()
{
    m_outer_radius = cAbs(a_outer_radius);
    m_inner_radius = cAbs(a_inner_radius);
};


//===========================================================================
/*!
    Evaluate the torus's implicit function, (sqrt(x^2+y^2)-R^2)^2+z^2-r^2,
    at given a_point

    \fn       float cImplicitTorus::S(cVector3d a_point)
    \param    a_point   Point at which to evaluate the implicit function
    \return   The value of the implicit function at the given point
*/
//===========================================================================
float cImplicitTorus::S(cVector3d a_point)
{
    return ((sqrt(a_point.x*a_point.x+a_point.y*a_point.y)-m_outer_radius)*
		    (sqrt(a_point.x*a_point.x+a_point.y*a_point.y)-m_outer_radius) +
            a_point.z*a_point.z-m_inner_radius*m_inner_radius);
}


//===========================================================================
/*!
    Evaluate the gradient of the torus's implicit function, 
    [2x-2xR/sqrt(x^2+y^2), 2y-2yR/sqrt(x^2+y^2), 2z], at given a_point

    \fn       cVector3d cImplicitTorus::dS(cVector3d a_point)
    \param    a_point  Point at which to evalute the gradient
    \return   The value of the gradient at the given point
*/
//===========================================================================
cVector3d cImplicitTorus::dS(cVector3d a_point)
{
    return (cVector3d(2*a_point.x - 2*a_point.x*m_outer_radius/sqrt(a_point.x*a_point.x+a_point.y*a_point.y),
          2*a_point.y - 2*a_point.y*m_outer_radius/sqrt(a_point.x*a_point.x+a_point.y*a_point.y),
          2*a_point.z ));
}


//===========================================================================
/*!
    Render the torus graphically

    \fn       void cImplicitTorus::render(const int a_renderMode)
    \param    a_renderMode  Ignored in this implementation
*/
//===========================================================================
void cImplicitTorus::render(const int a_renderMode)
{
    m_material.render();
    glutSolidTorus(m_inner_radius, m_outer_radius, 36, 36);

	if (m_state == 1)
	{
	    glPushMatrix();
	    glTranslated(m_surface_point.x, m_surface_point.y, m_surface_point.z);
	    cDrawSphere(0.05, 8, 8);
	    glPopMatrix();
	}
}


