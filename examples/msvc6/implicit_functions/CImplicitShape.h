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

#ifndef CImplicitShapeH
#define CImplicitShapeH

#include "CGenericPotentialField.h"
#include "CMaterial.h"


//===========================================================================
/*!
      \class      cImplicitShape
      \brief      cImplicitShape extends cGenericPotentialField and serves
                  as the base class for shapes implementing implicit rendering
                  algorithms, as described in Salisbury and Tarr's paper
                  "Haptic Rendering of Surfaces Defined by Implicit Functions".
*/
//===========================================================================
class cImplicitShape : public cGenericPotentialField
{
  public:
    // CONSTRUCTOR AND DESTRUCTOR:
    //! Constructor of cImplictShape
     cImplicitShape();
    //! Destructor of cImplicitShape
    virtual ~cImplicitShape() {};
    
    // METHODS:
    //! Compute a force given the current device position, according to the algorithm
    virtual cVector3d computeLocalForce(const cVector3d& a_localPosition);
    //! "Algorithm 1" from the paper; finds the closest point on the surface to given a_seed
    cVector3d findSurfacePoint(cVector3d a_seed);
    //! Subclasses should implement this function to evaluate the shape's implicit function at given a_point
    virtual float S(cVector3d a_point) = 0;
    //! Subclasses should implement this function to evalute the gradient of the shape's implicit function at given a_point
    virtual cVector3d dS(cVector3d a_point) = 0;
    //! Subclasses should implement this function to render the shape graphically
    virtual void render(const int a_renderMode=0) = 0;
    
    // PROPERTIES:
    //! A cMaterial object containing the object's graphic and haptic properties
    cMaterial m_material;

  protected:
    //! Is the device currently in contact with the shape (TOUCHED) or not (FREE)?
    int m_state;
    //! Avatar point on surface (p1 in the paper)
    cVector3d m_surface_point;
    //! The normal (gradient) at the surface point, used for the tangent plane
    cVector3d m_surface_normal;
};

#endif