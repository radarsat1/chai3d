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
#ifndef CCollisionAABBTreeH
#define CCollisionAABBTreeH
//---------------------------------------------------------------------------
#include "CMaths.h"
#include "CVertex.h"
#include "CTriangle.h"
#include "CCollisionAABBBox.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class    cCollisionAABBNode
      \brief    cCollisionAABBNode is an abstract class that contains methods
                to set up and maintain internal and leaf nodes of an AABB tree
                and to use them to detect for collision with a line.
*/
//===========================================================================
class cCollisionAABBNode
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cCollisionAABBNode.
    cCollisionAABBNode() {}
    //! Destructor of cCollisionAABBNode.
    virtual ~cCollisionAABBNode() {}

    // METHODS:
    virtual void fitBBox() {}
    //! Draw the edges of the bounding box for a tree node.
    virtual void render(unsigned int a_depth = -1) = 0;
    //! Determine whether given lines intersect the mesh bounded by the subtree
    //! rooted at the node.
    virtual bool computeCollision(
    cVector3d& a_segmentPointA, cVector3d& a_segmentPointB, cCollisionAABBBox &a_lineBox,
    cTriangle*& a_colTriangle, cVector3d& a_colPoint, double& a_colSquareDistance) = 0;

    // MEMBERS:
    //! The bounding box for the node.
    cCollisionAABBBox m_bbox;
    //! The depth of the node in the collision tree.
    unsigned int m_depth;

    // Returns true if this node contains the specified triangle tag
    virtual bool contains_triangle(int tag) = 0;
    
    cCollisionAABBNode* parent;
};


//===========================================================================
/*!
      \class    cCollisionAABBLeaf
      \brief    cCollisionAABBLeaf contains methods to set up and maintain leaf
                nodes of an AABB tree and to use them to detect for collision
                with a line.
*/
//===========================================================================
class cCollisionAABBLeaf : public cCollisionAABBNode
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Default constructor of cBBoxLeaf.
    cCollisionAABBLeaf() {}
    //! Constructor of cBBoxLeaf.
    cCollisionAABBLeaf(cTriangle *a_triangle) : m_triangle(a_triangle)
    {
		  fitBBox();
    }

    // METHODS:
    //! Create a bounding box to enclose triangle belonging to the leaf node.
    void fitBBox();
    //! Draw the edges of the bounding box for a leaf.
    void render(unsigned int a_depth = -1);
    //! Determine whether the given line intersects the leaf's triangle.
    bool computeCollision(cVector3d& a_segmentPointA,
    cVector3d& a_segmentPointB, cCollisionAABBBox &a_lineBox,
    cTriangle*& a_colTriangle, cVector3d& a_colPoint, double& a_colSquareDistance);

    // Returns true if this node contains the specified triangle tag
    virtual bool contains_triangle(int tag) {
      return (m_triangle != 0 && m_triangle->m_tag == tag);
    }

  protected:
    // MEMBERS:
    //! The triangle bounded by the leaf.
    cTriangle *m_triangle;
};


//===========================================================================
/*!
      \class    cCollisionAABBInternal
      \brief    cBBoxInternal contains methods to set up and maintain internal
                nodes of an AABB tree and to use them to detect for collision
                with a line.
*/
//===========================================================================
class cCollisionAABBInternal : public cCollisionAABBNode
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Default constructor of cBBoxInternal.
    cCollisionAABBInternal() { m_depth = 0; }
    //! Constructor of cBBoxInternal.
    cCollisionAABBInternal(unsigned int a_numLeaves, cCollisionAABBLeaf *a_leaves,
                           unsigned int a_depth = -1);

    // METHODS:
    //! Resize the bounding box to account for changes in the mesh.
    void fitBBox() {m_bbox.enclose(m_leftSubTree->m_bbox, m_rightSubTree->m_bbox);}
    //! Draw the edges of the bounding box for an internal node.
    void render(unsigned int a_depth = -1);
    //! Determine whether given line intersects the tree rooted at this node.
    bool computeCollision(cVector3d& a_segmentPointA,
    cVector3d& a_segmentPointB, cCollisionAABBBox &a_lineBox,
    cTriangle*& a_colTriangle, cVector3d& a_colPoint, double& a_colSquareDistance);

    virtual bool contains_triangle(int tag) {
      return (
        m_leftSubTree->contains_triangle(tag)
        ||
        m_rightSubTree->contains_triangle(tag)
        );
    }

  protected:
    // MEMBERS:
    //! The root of the node's left subtree.
    cCollisionAABBNode *m_leftSubTree;
    //! The root of the node's right subtree.
    cCollisionAABBNode *m_rightSubTree;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

