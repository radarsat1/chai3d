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

#include "CImplicitShape.h"
#include "CMaterial.h"

//===========================================================================
/*!
      \class      cImplicitSphere
      \brief      cImplicitShape extends cImplicitShape for rendering 
                  spheres according Salisbury and Tarr's algorithm, with
                  S(x,y,z) = x^2 + y^2 + z^2 - r^2
*/
//===========================================================================
class cImplicitSphere : public cImplicitShape
{
  public:
    // CONSTRUCTOR AND DESTRUCTOR:
    //! Constructor of cImplicitSphere
    cImplicitSphere(const double& a_radius);
    //! Destructor of cImplicitSphere
    virtual ~cImplicitSphere() {};

    // METHODS:
    //! Evaluate the sphere's implicit function at given a_point
    float S(cVector3d a_point);
    //! Evaluate the gradient of the sphere's implicit function at given a_point
    cVector3d dS(cVector3d a_point);
    //! Render the sphere graphically
    virtual void render(const int a_renderMode=0);
	
    // PROPERTIES:
    //! Radius of the sphere
    double m_radius;
};
