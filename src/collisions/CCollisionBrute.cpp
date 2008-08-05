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
#include "CVertex.h"
#include "CTriangle.h"
#include "CCollisionBrute.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cCollisionBrute.

    \fn     cCollisionBrute::cCollisionBrute(vector<cTriangle> *a_triangles)
*/
//===========================================================================
cCollisionBrute::cCollisionBrute(vector<cTriangle> *a_triangles)
{
    m_triangles = a_triangles;
}


//===========================================================================
/*!
    Check if a segment intersects any triangle. This method is called "brute
    force" because all triangles are checked. This method is simple but not
    efficient at all.

    \fn     bool cCollisionBrute::computeCollision(cVector3d& a_segmentPointA,
            cVector3d& a_segmentPointB, cGenericObject*& a_colObject,
            cTriangle*& a_colTriangle, cVector3d& a_colPoint, double& a_colSquareDistance,
            int proxySequenceNumber)

    \param  a_segmentPointA  Initial point of segment.
    \param  a_segmentPointB  End point of segment.
    \param  a_colObject  Pointer to nearest collided object.
    \param  a_colTriangle Pointer to nearest colided triangle.
    \param  a_colPoint  Position of nearest collision.
    \param  a_colSquareDistance  Distance between ray origin and nearest
              collision point.
    \param  a_proxyCall  If this is > 0, this is a call from a proxy, and the value
                         of a_proxyCall specifies which call this is.  -1 for non-proxy
                         calls.

    \return  Returns \b true if a triangles was hit.
*/
//===========================================================================
bool cCollisionBrute::computeCollision(cVector3d& a_segmentPointA,
     cVector3d& a_segmentPointB, cGenericObject*& a_colObject,
     cTriangle*& a_colTriangle, cVector3d& a_colPoint,
     double& a_colSquareDistance, int a_proxyCall)
{
    // temp variable for storing result.
    cGenericObject* colObject;
    cTriangle* colTriangle;
    cVector3d colPoint;
    bool hit = false;

    // convert segment into a line.
    cVector3d dir;
    a_segmentPointB.subr(a_segmentPointA, dir);

    // compute the square length of the segment.
    double colSquareDistance = dir.lengthsq();

    // parse all triangles for collision and return the nearest one.
    for (unsigned int i=0; i<m_triangles->size(); i++)
    {

        if ((*m_triangles)[i].computeCollision(
            a_segmentPointA, dir, colObject, colTriangle, colPoint, colSquareDistance))
        {
            // collision was detected between triangle and segment.
            // check if segment intersects triangle.
            a_colObject = colObject;
            a_colTriangle = colTriangle;
            a_colPoint = colPoint;
            a_colSquareDistance = colSquareDistance;
            hit = true;
        }
    }

    // return result
    return (hit);
}



