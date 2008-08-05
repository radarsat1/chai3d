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
#include "CImplicitShape.h"

#define FREE 0
#define TOUCHED 1
#define LARGE_FLOAT 9999.9

#pragma unmanaged


//===========================================================================
/*!
    Constructor of cImplicitShape

    \fn       cImplicitShape::cImplicitShape()
*/
//===========================================================================
cImplicitShape::cImplicitShape()
{
    // Set up a default yellow, shiny material
    m_material.setShininess(100);
    m_material.m_ambient.set((float)0.8, (float)0.8, (float)0.0);
    m_material.m_diffuse.set((float)0.8, (float)0.8, (float)0.0);
    m_material.m_specular.set((float)1.0, (float)1.0, (float)1.0);

    // Set default stiffness and friction coefficients
    m_material.setStiffness(20.0);
    m_material.setStaticFriction(0.6);
    m_material.setDynamicFriction(0.3);
};


//===========================================================================
/*!
    "Algorithm 1" from the paper; finds the closest point on the surface to 
    given a_seed

    \fn     cVector3d cImplicitShape::findSurfacePoint(cVector3d a_seed)
    \param  a_seed  Point from which to begin search for a surface point
    \return The surface point that is (approximately) nearest to the given point 
*/
//===========================================================================
cVector3d cImplicitShape::findSurfacePoint(cVector3d a_seed)
{
    cVector3d p, delta_p;
    p = a_seed;
    delta_p.set(LARGE_FLOAT, LARGE_FLOAT, LARGE_FLOAT);
    while (delta_p.lengthsq() > 0.01)
    {
      delta_p = cMul(-(S(p)/(cDot(dS(p), dS(p)))), dS(p));
      p.add(delta_p);
    }
    return p;
}


//===========================================================================
/*!
    Compute a force given the current device position, according to the 
    algorithm

    \fn     cVector3d cImplicitShape::computeLocalForce(const cVector3d& 
            a_localPosition)
    \param  a_localPosition  Current position of the haptic device, in local
                             coordinates
    \return The force vector generated based on device's position
*/
//===========================================================================
cVector3d cImplicitShape::computeLocalForce(const cVector3d& a_localPosition)
{
    cVector3d localForce;
    localForce.set(0,0,0);

    // If S(x,y,z) < 0, we are inside the object...
    if (S(a_localPosition) < 0.0)
    {
      // Set the "in contact" color (red)
      m_material.m_ambient.set(0.8, 0.0, 0.0);

      // If this is a first contact, just find the nearest surface point
      if (m_state == FREE)
      {
        m_surface_point = findSurfacePoint(a_localPosition);
      }

      // If we're already in contact...
      else
      {
        // Project the device position onto the constraint plane from previous iteration
        cVector3d p0p = cProjectPointOnPlane(a_localPosition, m_surface_point, m_surface_normal);

        // Calculate the radius of the static friction cone: tan(alpha) = us = (radius)/(adjacent_side)
        float static_friction_radius = m_material.getStaticFriction()*cDistance(p0p, a_localPosition);

        // If the distance between the previous surface point and the new device position, projected onto
        // the plane defined by the previous surface point and its the normal (gradient), is greater than 
        // the static friction cone radius, we need to "slip"
        if (cDistance(m_surface_point, p0p) > static_friction_radius)
        {
          // We need to move the surface point along the line between the previous surface point and
          // the current projected point
          cVector3d desired_direction = cSub(m_surface_point, p0p);
          desired_direction.normalize();

          // Calculate the radius of the dynamic friction cone: tan(alpha) = ud = (radius)/(adjacent_side)
          float dynamic_friction_radius = m_material.getDynamicFriction()*cDistance(p0p, a_localPosition);

          // Modify the projected point to be at the edge of the dynamic friction radius 
          p0p = cAdd(p0p, cMul(dynamic_friction_radius, desired_direction));

          // Now find the closest surface point to this point
          m_surface_point = findSurfacePoint(p0p);
        }
      }

      // Compute the surface normal (that defines the constraint plane) as the gradient at the surface point
      m_surface_normal = dS(m_surface_point);
      m_surface_normal.normalize();

      // Render a spring force between the device position and the surface point
      localForce = cMul(m_material.getStiffness(), cSub(m_surface_point, a_localPosition));
      m_state = TOUCHED;
    }

    // If we are outside the object, force stays 0 and "not in contact" color (yellow) is set
    else
    {
      m_material.m_ambient.set(0.8, 0.8, 0.0);
      m_state = FREE;
    }

    return (localForce);
}
