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

    \author     Chris Sewell
    \file       CCollisionSpheresGeometry.cpp
    \version    1.0
    \date       01/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CVertex.h"
#include "CTriangle.h"
#include "CCollisionSpheresGeometry.h"
#include "CCollisionSpheres.h"
//---------------------------------------------------------------------------

//! STATIC VARIABLES:
// Initialize m_split, axis on which to sort triangle primitives.
int cCollisionSpheresGenericShape::m_split = 0;

//===========================================================================
/*!
    Return the edge that connects this point to the given point.  If no such
    edge exists, create a new edge between the two points, and return this
    new edge.

    \fn       cCollisionSpheresEdge *cCollisionSpheresPoint::getEdgeTo(
              cCollisionSpheresPoint *a_other)
    \param    a_other  The other endpoint of an edge connecting to this point.
    \return   Return a pointer to edge connecting this point and given point.
*/
//===========================================================================
cCollisionSpheresEdge *cCollisionSpheresPoint::getEdgeTo(cCollisionSpheresPoint *a_other)
{
    // check if this edge to the given point already exists in this point's edge map
    PtEmap::iterator edgeIter;
    edgeIter = m_edgeMap.find(a_other);

    // if the edge does not yet exist, create it
    if (edgeIter == m_edgeMap.end())
      return new cCollisionSpheresEdge(this, a_other);

    // return the (new or previously existing) edge
    return edgeIter->second;
}


//===========================================================================
/*!
    Constructor of cCollisionSpheresEdge.

    \fn       cCollisionSpheresEdge::cCollisionSpheresEdge(
              cCollisionSpheresPoint *a_a, cCollisionSpheresPoint *a_b)
    \param    a_a  First vertex of the edge.
    \param    a_b  Second vertex of the edge.
*/
//===========================================================================
cCollisionSpheresEdge::cCollisionSpheresEdge(cCollisionSpheresPoint *a_a,
                                             cCollisionSpheresPoint *a_b)
{
    // set the endpoints of the new edge
    m_end[0] = a_a;
    m_end[1] = a_b;

    // insert the edge into the edge maps of both endpoints
    m_end[0]->m_edgeMap.insert(PtEmap::value_type(m_end[1], this));
    m_end[1]->m_edgeMap.insert(PtEmap::value_type(m_end[0], this));

    // calculate the vector between the endpoints
    m_d = cSub((*m_end[1]).m_pos, (*m_end[0]).m_pos);

    // calculate the squared distance of the edge
    m_D = m_d.x*m_d.x + m_d.y*m_d.y + m_d.z*m_d.z;

    // calculate the center of the edge
    double lambda = 0.5;
    m_center.x = (*m_end[0]).m_pos.x + lambda*((*m_end[1]).m_pos.x - (*m_end[0]).m_pos.x);
    m_center.y = (*m_end[0]).m_pos.y + lambda*((*m_end[1]).m_pos.y - (*m_end[0]).m_pos.y);
    m_center.z = (*m_end[0]).m_pos.z + lambda*((*m_end[1]).m_pos.z - (*m_end[0]).m_pos.z);
}


//===========================================================================
/*!
    Constructor of cCollisionSpheresTri.

    \fn       cCollisionSpheresTri::cCollisionSpheresTri(
              cCollisionSpheresPoint *a, cCollisionSpheresPoint *b,
              cCollisionSpheresPoint *c)
    \param    a_a     First vertex of the triangle.
    \param    a_b     Second vertex of the triangle.
    \param    a_c     Third vertex of the triangle.
    \return   Return a pointer to new cCollisionSpheresTri instance.
*/
//===========================================================================
cCollisionSpheresTri::cCollisionSpheresTri(cCollisionSpheresPoint *a_a,
                                           cCollisionSpheresPoint *a_b,
                                           cCollisionSpheresPoint *a_c)
{
    // set the vertices (corners) of the triangle
    m_corner[0] = a_a;
    m_corner[1] = a_b;
    m_corner[2] = a_c;

    // set the edges (sides) of the triangle
    int i;
    for (i = 0; i < 3; i++)
      m_side[i] = m_corner[i]->getEdgeTo(m_corner[(i + 1) % 3]);

    // calculate a center point for the bounding sphere halfway between the
    // center of the first edge and the first vertex
    double lambda = 0.5;
    m_center.x = (*m_side[0]->getCenter()).x +
            lambda*((*m_corner[0]).m_pos.x - (*m_side[0]->getCenter()).x);
    m_center.y = (*m_side[0]->getCenter()).y +
            lambda*((*m_corner[0]).m_pos.y - (*m_side[0]->getCenter()).y);
    m_center.z = (*m_side[0]->getCenter()).z +
            lambda*((*m_corner[0]).m_pos.z - (*m_side[0]->getCenter()).z);

    // calculate a radius of the bounding sphere as the largest distance between
    // the sphere center calculated above and any vertex of the triangle
    m_radius = 0;
    for (i = 0; i < 3; i++)
    {
        double curRadius = m_corner[i]->m_pos.distance(m_center);
        if (curRadius > m_radius)
            m_radius = curRadius;
    }
}


//===========================================================================
/*!
    Determine whether there is any intersection between the primitives
    (this triangle and the given line) by calling the collision detection
    method of the cTriangle object associated with this triangle primitive.

    \fn       bool cCollisionSpheresTri::computeCollision(
              cCollisionSpheresGenericShape *a_other,
              cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
              cVector3d& a_colPoint, double& a_colSquareDistance))
    \param    a_other  The line primitive to check for intersection.
    \param    a_colObject  Returns pointer to nearest collided object.
    \param    a_colTriangle  Returns pointer to nearest colided triangle.
    \param    a_colPoint  Returns position of nearest collision.
    \param    a_colSquareDistance  Returns distance between ray origin and
                                   collision point.
    \return   Return whether the given line intersects this triangle.
*/
//===========================================================================
bool cCollisionSpheresTri::computeCollision(cCollisionSpheresGenericShape *a_other,
                                            cGenericObject*& a_colObject,
                                            cTriangle*& a_colTriangle,
                                            cVector3d& a_colPoint,
                                            double& a_colSquareDistance)
{
    // cast the "other" shape to a line primitive; collision detection is
    // currently only set up to handle line segment - triangle intersections
    cCollisionSpheresLine* line = (cCollisionSpheresLine*)a_other;

    // check for a collision between the primitives (one a triangle and the
    // other a line segment, we assume) by calling the collision detection
    // method of the cTriangle object associated with this triangle primitive;
    // it will only return true if the distance between the segment origin and
    // the triangle is less than the current closest intersecting triangle
    // (whose distance squared is kept in a_colSquareDistance)
    return (m_original->computeCollision(line->getSegmentPointA(),
            line->getDir(), a_colObject, a_colTriangle, a_colPoint,
            a_colSquareDistance));
}


//===========================================================================
/*!
    Constructor of cCollisionSpheresLine.

    \fn       cCollisionSpheresLine::cCollisionSpheresLine(
              cVector3d& a_segmentPointA, cVector3d& a_segmentPointB)
    \param    a_segmentPointA   First endpoint of the line segment.
    \param    a_segmentPointB   Second endpoint of the line segment.
*/
//===========================================================================
cCollisionSpheresLine::cCollisionSpheresLine(cVector3d& a_segmentPointA,
                                             cVector3d& a_segmentPointB)
{
    // calculate the center of the line segment
    m_center = cAdd(a_segmentPointA, a_segmentPointB);
    m_center.x *= 0.5;
    m_center.y *= 0.5;
    m_center.z *= 0.5;

    // calculate the radius of the bounding sphere as the distance from the
    // center of the segment (calculated above) to an endpoint
    cVector3d rad = cSub(m_center, a_segmentPointA);
    m_radius = sqrt(rad.x*rad.x + rad.y*rad.y + rad.z*rad.z);

    // set origin and direction of the line segment; i.e., redefine the segment
    // as a ray from the first endpoint (presumably the proxy position when
    // the collision detection is being used with the proxy force algorithm) to
    // the second endpoint (presumably the goal position)
    m_segmentPointA = a_segmentPointA;
    a_segmentPointB.subr(a_segmentPointA, m_dir);
}


//===========================================================================
/*!
    Determine whether there is any intersection between the primitives
    (this line and the given triangle) by calling the collision detection
    method of the triangle primitive.

    \fn       bool cCollisionSpheresLine::computeCollision(
              cCollisionSpheresGenericShape *a_other,
              cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
              cVector3d& a_colPoint, double& a_colSquareDistance))
    \param    a_other  The triangle primitive to check for intersection.
    \param    a_colObject  Returns pointer to nearest collided object.
    \param    a_colTriangle  Returns pointer to nearest colided triangle.
    \param    a_colPoint  Returns position of nearest collision.
    \param    a_colSquareDistance  Returns distance between ray origin and
                                   collision point.
    \return   Return whether the given triangle intersects this line.
*/
//===========================================================================
bool cCollisionSpheresLine::computeCollision(cCollisionSpheresGenericShape *a_other,
          cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
          cVector3d& a_colPoint, double& a_colSquareDistance)
{
    // check for a collision between the primitives (one a triangle and the
    // other a line segment, we assume) by calling the collision detection
    // method of the triangle primitive; it will only return true if the
    // distance between the segment origin and the triangle is less than the
    // current closest intersecting triangle (whose distance squared is kept
    // in a_colSquareDistance)
    return a_other->computeCollision(this, a_colObject, a_colTriangle,
            a_colPoint, a_colSquareDistance);
}






