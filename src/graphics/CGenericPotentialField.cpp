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
    \date       01/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CGenericPotentialField.h"
//---------------------------------------------------------------------------

//==========================================================================
/*!
      Compute interaction forces between finger and object.

      \fn       cVector3d cGenericPotentialField::computeForces
                (cVector3d& a_fingerPosition)
      \param    a_fingerPosition   Position of finger
      \return   Return computed force
*/
//===========================================================================
cVector3d cGenericPotentialField::computeForces(cVector3d& a_fingerPosition)
{
    // compute the position of the finger in local coordinates.
    cVector3d fingerPositionLocal;
    fingerPositionLocal = cMul(cTrans(m_localRot), cSub(a_fingerPosition, m_localPos));

    // compute interaction forces with this object
    cVector3d localForce;
    localForce = computeForces(fingerPositionLocal);

    // compute interaction forces with children
    for (unsigned int i=0; i<m_children.size(); i++)
    {
        cGenericObject *nextObject = m_children[i];

        if (typeid(*nextObject) == typeid(cGenericPotentialField))
        {
            cGenericPotentialField* nextField = (cGenericPotentialField*)nextObject;
            cVector3d force = nextField->computeForces(fingerPositionLocal);
            localForce.add(force);
        }
    }

    // convert reaction force in parent coodinates
    cVector3d parentForce;
    parentForce = cMul(m_localRot, localForce);
    return (parentForce);
}
