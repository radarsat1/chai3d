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
    \date       06/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CShapeSphere.h"
#include "CDraw3D.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cShapeSphere.

    \fn     cShapeSphere::cShapeSphere()
    \param  a_radius    Radius of sphere
*/
//===========================================================================
cShapeSphere::cShapeSphere(double a_radius)
{
    // initialize radius of sphere
    m_radius = cAbs(a_radius);

    // set material properties
    m_material.setShininess(100);
    m_material.m_ambient.set((float)0.3, (float)0.3, (float)0.3);
    m_material.m_diffuse.set((float)0.1, (float)0.7, (float)0.8);
    m_material.m_specular.set((float)1.0, (float)1.0, (float)1.0);
};


//===========================================================================
/*!
    Render sphere in OpenGL

    \fn       void cShapeSphere::render()
*/
//===========================================================================
void cShapeSphere::render(const int a_renderMode)
{
    // render material properties
    m_material.render();

    cDrawSphere(m_radius,36,36);
    
}


//===========================================================================
/*!
    Compute forces between tool and sphere shape

    \fn       cVector3d cShapeSphere::computeLocalForce(const cVector3d& a_localPosition)
    \param    a_localPosition    position of tool in world coordinates
    \return   return reaction force if tool is located inside sphere
*/
//===========================================================================
cVector3d cShapeSphere::computeLocalForce(const cVector3d& a_localPosition)
{
	
    // In the following we compute the reaction forces between the tool and the
    // sphere.
    cVector3d localForce;

    if (a_localPosition.length() > m_radius)
    {
        // Here the tool is located outside the sphere
        localForce.set(0, 0, 0);
    }
    else
    {
        // Here the tool is located inside the sphere; we need to compute
        // the reaction force.

        // compute penetration distance between tool and surface of sphere
        double penetrationDistance = m_radius - a_localPosition.length();

        // get the material stiffness defined by the material properties of the object
        double materialStiffness = m_material.getStiffness();

        // compute the direction of the reaction force. For the sphere its pretty
        // simple since it can be described by a vector going from the center of
        // the sphere towards the position tool.
        cVector3d forceDirection = cNormalize(a_localPosition);

        // compute a reaction force propotional to the penetration distance
        localForce = cMul( penetrationDistance * materialStiffness, forceDirection);
    }

    return (localForce);
}


//===========================================================================
/*!
    Update bounding box of current object.

    \fn       void updateBoundaryBox()
*/
//===========================================================================
void cShapeSphere::updateBoundaryBox()
{
    m_boundaryBoxMin.set(-m_radius, -m_radius, -m_radius);
    m_boundaryBoxMax.set( m_radius,  m_radius,  m_radius);
}


//===========================================================================
/*!
    Scale object of defined scale factor

    \fn       void cShapeSphere::scaleObject(double a_scaleFactor)
    \param    a_scaleFactor Scale factor
*/
//===========================================================================
void cShapeSphere::scaleObject(const double& a_scaleFactor)
{
    m_radius = a_scaleFactor * m_radius;
}
