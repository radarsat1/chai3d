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
    \author:    Christopher Sewell
    \version    1.1
    \date       01/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CGenericCollisionH
#define CGenericCollisionH
//---------------------------------------------------------------------------
#include "CVector3d.h"
#include "CMatrix3d.h"
#include "CVertex.h"
#include "CTriangle.h"
#include "CMaterial.h"
#include <vector>
//---------------------------------------------------------------------------
using std::vector;
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class    cCollision
      \brief    cCollision is an abstract class for collision-detection
                algorithms for meshes.
*/
//===========================================================================
class cGenericCollision
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cGenericCollision
    cGenericCollision();
    //! Destructor of cGenericCollision
    virtual ~cGenericCollision() {};

    // VIRTUAL METHODS:
    //! Do any necessary initialization, such as building trees.
    virtual void initialize() {};
    //! Maintain the collision tree in any topological changes have occured in the mesh
    virtual void maintain() {};
    //! Provide a visual representation of the method.
    virtual void render() {};
    //! Return the nearest triangle intersected by the given line.
    virtual bool computeCollision(cVector3d& a_segmentPointA,
        cVector3d& a_segmentPointB, cGenericObject*& a_colObject,
        cTriangle*& a_colTriangle, cVector3d& a_colPoint, double& a_colSquareDistance, int a_proxyCall = -1)
        { return (false); }

    // METHODS:
    //! Set Level
    void setDisplayDepth(unsigned int a_depth) { m_displayDepth = a_depth; }
    //! Read level
    double getDisplayDepth() const { return (m_displayDepth); } 
    //! Color properties of object
    cMaterial m_material;

  protected:
    // MEMBERS:
    //! Depth inside tree to render
    unsigned int m_displayDepth;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

