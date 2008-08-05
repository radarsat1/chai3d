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
#include "CCollisionAABB.h"
//---------------------------------------------------------------------------

// Pointer for creating new AABB tree nodes
cCollisionAABBInternal* g_nextFreeNode;
extern bool intersect(cCollisionAABBBox a_0, cCollisionAABBBox a_1);

//===========================================================================
/*!
    Constructor of cCollisionAABB.

    \fn     cCollisionAABB::cCollisionAABB(vector<cTriangle> *a_triangles,
                                           bool a_useNeighbors)
    \param  a_triangle      Pointer to array of triangles.
    \param  a_useNeighbors  Use neighbor lists to speed up collision detection?
*/
//===========================================================================
cCollisionAABB::cCollisionAABB(vector<cTriangle>* a_triangles,
                               bool a_useNeighbors)
{
    // list of triangles used when building the tree
    m_triangles = a_triangles;

    // initialize members
    m_root = NULL;
    m_leaves = NULL;
    m_numTriangles = 0;
    m_useNeighbors = a_useNeighbors;
}


//===========================================================================
/*!
    Destructor of cCollisionAABB.

    \fn     cCollisionAABB::~cCollisionAABB()
*/
//===========================================================================
cCollisionAABB::~cCollisionAABB()
{
    // clear collision tree
    if (m_root != NULL)
    {
        delete[] m_root;
    }
}


//===========================================================================
/*!
      Build the AABB Tree.

      \fn         void cCollisionAABB::initialize()
*/
//===========================================================================
void cCollisionAABB::initialize()
{
    unsigned int i;
    lastCollision = NULL;

    // if a previous tree was created, delete it
    if (m_root != NULL)
    {
        delete m_root;
    }

    // reset triangle counter
    m_numTriangles = 0;

    // count the number of allocated triangles that will be used to create
    // the tree.
    for (i = 0; i < m_triangles->size(); ++i)
    {
      cTriangle* nextTriangle = &(*m_triangles)[i];
      if (nextTriangle->allocated())
      {
        m_numTriangles++;
      }
    }

    // check if the number of triangles is equal to zero
    if (m_numTriangles == 0)
    {
        m_root = NULL;
        return;
    }

    // create a leaf node for each triangles
    m_leaves = new cCollisionAABBLeaf[m_numTriangles];
    for (i = 0; i < m_numTriangles; ++i)
    {
      cTriangle* nextTriangle = &(*m_triangles)[i];
      if (nextTriangle->allocated())
      {        
        new(&m_leaves[i]) cCollisionAABBLeaf(nextTriangle);
      }
    }

    // allocate an array to hold all internal nodes of the binary tree
    if (m_numTriangles >= 2)
    {
      g_nextFreeNode = new cCollisionAABBInternal[m_numTriangles-1];
      m_root = g_nextFreeNode;
      new(g_nextFreeNode++) cCollisionAABBInternal(m_numTriangles, m_leaves, 0);
    }

    // there is only one triangle, so the tree in only constituted of one leaf.
    else
    {
        m_root = &m_leaves[0];
    }
}


//===========================================================================
/*!
    Check if a segment intersects any triangle of the mesh. The method uses
    the pre computed AABB boxes.

    \fn     bool cCollisionBrute::computeCollision(cVector3d& a_segmentPointA,
            cVector3d& a_segmentPointB, cGenericObject*& a_colObject,
            cTriangle*& a_colTriangle, cVector3d& a_colPoint,
            double& a_colSquareDistance, int a_num)

    \param  a_rayOrigin  Point from where collision ray starts.
    \param  a_rayDir  Direction vector of collision ray.
    \param  a_colObject  Pointer to nearest collided object.
    \param  a_colTriangle Pointer to nearest colided triangle.
    \param  a_colPoint  Position of nearest collision.
    \param  a_colSquareDistance  Distance between ray origin and nearest
              collision point.
    \param  a_proxyCall  If this is > 0, this is a call from a proxy, and the value
                         of a_proxyCall specifies which call this is.  -1 for
                         non-proxy calls.

    \return  Returns \b true if a triangles was hit.
*/
//===========================================================================
bool cCollisionAABB::computeCollision(cVector3d& a_segmentPointA,
        cVector3d& a_segmentPointB, cGenericObject*& a_colObject,
        cTriangle*& a_colTriangle, cVector3d& a_colPoint,
        double& a_colSquareDistance, int a_proxyCall)
{
    // convert two point segment into a segment described by a point and
    // a directional vector.
    cVector3d dir;
    a_segmentPointB.subr(a_segmentPointA, dir);

    // create an axis-aligned bounding box for the line
    cCollisionAABBBox l_lineBox;
    l_lineBox.setEmpty();
    l_lineBox.enclose(a_segmentPointA);
    l_lineBox.enclose(a_segmentPointB);
 
    // if this is a subsequent call from the proxy algorithm after a detecting
    // an initial collision, only neighbors of the triangle from the first
    // collision detection need to be checked
    if ((m_useNeighbors) && (a_proxyCall > 1) && (m_root != NULL) &&
        (lastCollision != NULL) && (lastCollision->m_neighbors != NULL))
    {
      cGenericObject* colObject;
      cTriangle* colTriangle;
      cVector3d colPoint;
      double colSquareDistance = dir.lengthsq();
      bool firstHit = true;

      // check each neighbor, and find the closest for which there is a
      // collision, if any
      for (unsigned int i=0; i<lastCollision->m_neighbors->size(); i++)
      {
        if (((*(lastCollision->m_neighbors))[i])->computeCollision(
                                    a_segmentPointA,
                                    dir,
                                    colObject,
                                    colTriangle,
                                    colPoint,
                                    colSquareDistance
                                  ))
        {
          if (firstHit || (colSquareDistance < a_colSquareDistance))
          {
            lastCollision = colTriangle;
            a_colObject = colObject;
            a_colTriangle = colTriangle;
            a_colPoint = colPoint;
            a_colSquareDistance = colSquareDistance;
            firstHit = false;
          }
        }
      }
      if (!firstHit)
        return true;
      lastCollision = NULL;
      return false;
    }

    // otherwise, if this is the first call in an iteration of the proxy
    // algorithm (or a call from any other algorithm), check the sphere
    // tree

    // test for intersection between this box and the collision tree
    if (m_root == NULL)
    {
        lastCollision = NULL;
        return (false);
    }
    else
    {
      a_colSquareDistance = dir.lengthsq();
      bool result = m_root->computeCollision(a_segmentPointA, dir, l_lineBox,
               a_colTriangle, a_colPoint, a_colSquareDistance);
      if (result) {
        lastCollision = a_colTriangle;
        a_colObject = a_colTriangle->getParent();        
      }
      else
        lastCollision = NULL;
      return result;
    }
}


//===========================================================================
/*!
      Render the bounding boxes in OpenGL.

      \fn         void cCollisionAABB::render()
*/
//===========================================================================
void cCollisionAABB::render()
{
    if (m_root != NULL)
    {
        // set rendering settings
        glDisable(GL_LIGHTING);
        glLineWidth(1.0);
        glColor4fv(m_material.m_ambient.pColor());

        // render tree
        m_root->render(m_displayDepth);

        // restore settings
        glEnable(GL_LIGHTING);
    }
}



