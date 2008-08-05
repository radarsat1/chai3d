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
#ifndef CGenericPotentialFieldH
#define CGenericPotentialFieldH
//---------------------------------------------------------------------------
#include "CGenericObject.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class      cGenericPotentialField
      \brief      cGenericPotentialField describes a generic class to create
                  objects which are describe by implicit functions.
*/
//===========================================================================
class cGenericPotentialField : public cGenericObject
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cGenericPotentialField.
    cGenericPotentialField() {};
    //! Destructor of cGenericPotentialField.
    virtual ~cGenericPotentialField() {};

    // METHODS:
    //! compute interaction forces between finger and object.
    cVector3d computeForces(cVector3d& a_fingerPosition);

    // VIRTUAL METHODS:
    //! Render object in OpenGL.
    virtual void render(const int a_renderMode=0) {};
    //! Update global positions.
    virtual void updateGlobalPositions(const bool a_frameOnly) {};
    //! Update bounding box of current object
    virtual void updateBoundaryBox() {};
    //! Scale object of defined scale factor
    virtual void scaleObject(double a_scaleFactor) {};
    //! Compute interaction force for current object in local frame
    virtual cVector3d computeLocalForce(const cVector3d& a_localPosition) { return (cVector3d(0,0,0)); };
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
