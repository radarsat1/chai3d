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
#include "CCollisionAABBTree.h"
//---------------------------------------------------------------------------

// pointer for creating new AABB tree nodes
extern cCollisionAABBInternal* g_nextFreeNode;

//===========================================================================
/*!
      Determine whether the two given boxes intersect each other.

      \fn         inline bool intersect(cBBox a_0, cBBox a_1)
      \param      a_0   First box; may intersect with second box.
      \param      a_1   Second box; may intersect with first box.
      \return     Returns whether there is any overlap in the two boxes.
*/
//===========================================================================
inline bool intersect(cCollisionAABBBox a_0, cCollisionAABBBox a_1)
{
    if (a_0.getLowerX() > a_1.getUpperX()) return false;
    if (a_0.getLowerY() > a_1.getUpperY()) return false;
    if (a_0.getLowerZ() > a_1.getUpperZ()) return false;
    if (a_1.getLowerX() > a_0.getUpperX()) return false;
    if (a_1.getLowerY() > a_0.getUpperY()) return false;
    if (a_1.getLowerZ() > a_0.getUpperZ()) return false;

    // collision has occured.
    return (true);
}


//===========================================================================
/*!
      Render bounding box of leaf node.

      \fn         void cCollisionAABBLeaf::render(unsigned int a_depth)
      \param      a_depth   Only draw nodes at this depth in the tree.
                            a_depth = -1 renders the complete tree.
*/
//===========================================================================
void cCollisionAABBLeaf::render(unsigned int a_depth)
{
    if (a_depth == m_depth)
    {
        m_bbox.render();
    }
}


//===========================================================================
/*!
      Create a bounding box to enclose the three vertices in the triangle
      belonging to the leaf node.

      \fn         void cCollisionAABBLeaf::fitBBox()
*/
//===========================================================================
void cCollisionAABBLeaf::fitBBox()
{
    // empty box
    m_bbox.setEmpty();

    // enclose all three vertices of triangle
    if (m_triangle != NULL)
    {
      m_bbox.enclose(m_triangle->getVertex0()->getPos());
      m_bbox.enclose(m_triangle->getVertex1()->getPos());
      m_bbox.enclose(m_triangle->getVertex2()->getPos());
    }
}


//===========================================================================
/*!
      Determine whether the given line intersects the triangle belonging to
      the leaf node.

      \fn       bool cCollisionAABBLeaf::computeCollision(cVector3d&
                a_segmentPointA, cVector3d& a_segmentPointB, cCollisionAABBBox
                &a_lineBox, cTriangle*& a_colTriangle, cVector3d& a_colPoint,
                double& a_colSquareDistance)
    \param      a_segmentPointA  Initial point of segment.
    \param      a_segmentPointB  End point of segment.
    \param      a_colObject  Pointer to nearest collided object.
    \param      a_colTriangle Pointer to nearest colided triangle.
    \param      a_colPoint  Position of nearest collision.
    \param      a_colSquareDistance  Distance between ray origin and nearest
                collision point.
    \return     Returns \b true if a triangles was hit.
*/
//===========================================================================
bool cCollisionAABBLeaf::computeCollision(cVector3d& a_segmentPointA,
    cVector3d& a_segmentPointB, cCollisionAABBBox &a_lineBox,
    cTriangle*& a_colTriangle, cVector3d& a_colPoint, double& a_colSquareDistance)
{
    cGenericObject* colObject;
   
    // compute collision detection
    bool result = m_triangle->computeCollision(
                                    a_segmentPointA,

                                    // This is really a ray direction at this point.
                                    // Should be renamed...
                                    a_segmentPointB,
                                    colObject,
                                    a_colTriangle,
                                    a_colPoint,
                                    a_colSquareDistance
                                  );

    // return result
    return (result);
}


//===========================================================================
/*!
      Draw the edges of the bounding box for an internal tree node, and
      call the draw function for its children.

      \fn         void cCollisionAABBInternal::draw(int depth)
      \param      depth   Only draw nodes at this level in the tree.
                          a_depth = -1 renders the complete tree.
*/
//===========================================================================
void cCollisionAABBInternal::render(unsigned int a_depth)
{
    // render current node
    if (m_depth == a_depth)
    {
        m_bbox.render();
    }

    // render left sub tree
    m_leftSubTree->render(a_depth);

    // render right sub tree
    m_rightSubTree->render(a_depth);
}


//===========================================================================
/*!
      Create an internal AABB tree node.

      \fn         cCollisionAABBInternal::cCollisionAABBInternal(int n,
                                        cCollisionAABBLeaf *l)
      \param      a_numLeaves   Number of leaves in subtree rooted at this node.
      \param      a_leaves Pointer to the location in the array of leafs for the
                           first leaf under this internal node.
      \param      a_depth  Depth of the node in the collision tree.
      \return     Returns a pointer to a new cCollisionAABBInternal node.
*/
//===========================================================================
cCollisionAABBInternal::cCollisionAABBInternal(unsigned int a_numLeaves,
    cCollisionAABBLeaf *a_leaves, unsigned int a_depth)
{
    m_depth = a_depth;
    m_leftSubTree = NULL;
    m_rightSubTree = NULL;

    // create a box to enclose all the leafs below this internal node
    m_bbox.setEmpty();
    for (unsigned int j = 0; j < a_numLeaves; ++j)
    {
        m_bbox.enclose(a_leaves[j].m_bbox);
    }

    // move leafs with smaller coordinates towards the beginning of the array
    // and leaves with larger coordinates towards the end of the right
    int axis = m_bbox.longestAxis();
    unsigned int i = 0;
    unsigned int mid = a_numLeaves;
    while (i < mid)
    {
        if (a_leaves[i].m_bbox.getCenter().get(axis) < m_bbox.getCenter().get(axis))
        {
            ++i;
        }
        else
        {
            std::swap(a_leaves[i], a_leaves[--mid]);
        }
    }

    // assign leafs before midpoint to right child and create new internal node
    if (mid == 0 || mid == a_numLeaves)
    {
        mid = a_numLeaves / 2;
    }

    if (mid >= 2)
    {
        m_rightSubTree = g_nextFreeNode;
        new(g_nextFreeNode++) cCollisionAABBInternal(mid, &a_leaves[0], m_depth + 1);
    }
    else
    {
        m_rightSubTree = &a_leaves[0];

        if (m_rightSubTree != NULL)
        {
            m_rightSubTree->m_depth = m_depth + 1;
        }
    }

    // assign leafs after midpoint to left child and create new internal node
    if (a_numLeaves - mid >= 2)
    {
        m_leftSubTree = g_nextFreeNode;
        new(g_nextFreeNode++) cCollisionAABBInternal(a_numLeaves - mid, &a_leaves[mid], m_depth + 1);
    }
    else
    {
        m_leftSubTree = &a_leaves[mid];
        if (m_leftSubTree) m_leftSubTree->m_depth = m_depth + 1;
    }
}


//===========================================================================
/*!
    Determine whether the given line intersects the mesh covered by the
    AABB Tree rooted at this internal node.

    \fn       bool cCollisionAABBInternal::computeCollision(cVector3d&
                a_segmentPointA, cVector3d& a_segmentPointB, cCollisionAABBBox
                &a_lineBox, cTriangle*& a_colTriangle, cVector3d& a_colPoint,
                double& a_colSquareDistance)
    \param      a_segmentPointA  Initial point of segment.
    \param      a_segmentPointB  End point of segment.
    \param      a_colObject  Pointer to nearest collided object.
    \param      a_colTriangle Pointer to nearest colided triangle.
    \param      a_colPoint  Position of nearest collision.
    \param      a_colSquareDistance  Distance between ray origin and nearest
                collision point.
    \return     Returns \b true if a triangles was hit.
*/
//===========================================================================
bool cCollisionAABBInternal::computeCollision(cVector3d& a_segmentPointA,
    cVector3d& a_segmentPointB, cCollisionAABBBox &a_lineBox,
    cTriangle*& a_colTriangle, cVector3d& a_colPoint, double& a_colSquareDistance)
{
    // if a line's bounding box does not intersect the node's bounding box,
    // there can be no intersection
	if (!intersect(m_bbox, a_lineBox))
	{
		return (false);
	}

    // initialize objects for calls to left and right subtrees
    cTriangle *l_colTriangle, *r_colTriangle;
    cVector3d l_colPoint, r_colPoint;
    double l_colSquareDistance = a_colSquareDistance;
    double r_colSquareDistance = a_colSquareDistance;
    bool l_result = false;
    bool r_result = false;

    // check collision between line and left subtree node
    if ( m_leftSubTree->computeCollision(a_segmentPointA, a_segmentPointB,
            a_lineBox, l_colTriangle, l_colPoint, l_colSquareDistance) )
    {
        l_result = true;
    }

    // check collision between line and right subtree node
    if ( m_rightSubTree->computeCollision(a_segmentPointA, a_segmentPointB,
            a_lineBox, r_colTriangle, r_colPoint, r_colSquareDistance) )
    {
        r_result = true;
    }

    // if there is an intersection in either subtree, return the closest one
    if ((l_result && !r_result) || (l_result && r_result &&
         (l_colSquareDistance <= r_colSquareDistance)))
    {
      a_colTriangle = l_colTriangle;
      a_colPoint = l_colPoint;
      a_colSquareDistance = l_colSquareDistance;
    }
    else
    {
      a_colTriangle = r_colTriangle;
      a_colPoint = r_colPoint;
      a_colSquareDistance = r_colSquareDistance;
    }

    // return result
    return (l_result || r_result);
}

