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
#include "CPotentialFieldForceAlgo.h"
#include "CWorld.h"
#include "CGenericPotentialField.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Compute forces for all potential field based objects (cGenericPotentialField).

    \fn       cVector3d cPotentialFieldForceAlgo::computeForces(const
              cVector3d& a_nextDevicePos)
    \param    a_nextDevicePos  Next position of haptic device or virtual finger.
*/
//===========================================================================
cVector3d cPotentialFieldForceAlgo::computeForces(const cVector3d& a_nextDevicePos)
{
    // initialize force
    cVector3d force;
    force.zero();

    // compute force feedback for all potential field based objects
    if (m_world != NULL)
    {
			
        int numObjects = m_world->getNumChildren();
        for (int i=0; i<numObjects; i++)
        {
            cGenericPotentialField *nextField = dynamic_cast<cGenericPotentialField*>(m_world->getChild(i));

            // if this object is a cGenericPotentialField, calculate the force
            // by calling the object's computeForces method, and add this to
            // the cumulative total
            if (nextField)
            {
                cVector3d nextForce = nextField->computeForces(a_nextDevicePos);
                force.add(nextForce);
            }
        }
    }

    // return result
    return (force);
}

