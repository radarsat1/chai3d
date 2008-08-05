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
    \author     Based on code by Charity Lu
    \author     clu@cs.stanford.edu
    \version    1.1
    \date       01/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CCollisionAABBBoxH
#define CCollisionAABBBoxH
//---------------------------------------------------------------------------
#include "CMaths.h"
#include "CTriangle.h"
#include "CVertex.h"
#include <vector>
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class    cCollisionAABBox
      \brief    cCollisionAABBox contains the properties and methods of an
                axis-aligned bounding box, as used in the AABB collision
                detection algorithm.
*/
//===========================================================================
class cCollisionAABBBox
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Default constructor of cCollisionAABBBox.
    cCollisionAABBBox() { };
    //! Constructor of cCollisionAABBBox.
    cCollisionAABBBox(const cVector3d& a_min, const cVector3d& a_max)
        { setValue(a_min, a_max); }
    //! Destructor of cCollisionAABBBox.
    virtual ~cCollisionAABBBox() { };

    // METHODS:
    //! Return the center of the bounding box.
    inline cVector3d getCenter() const { return (m_center); }
    //! Return the extent (half the width) of the bounding box.
    inline cVector3d getExtent() const { return (m_extent); }
    //! Set the center of the bounding box.
    inline void setCenter(const cVector3d& a_center)  { m_center = a_center; }
    //! Set the extent (half the width) of the bounding box.
    inline void setExtent(const cVector3d& a_extent) { m_extent = a_extent; }
    //! Set the center and extent of the box based on two points.
    inline void setValue(const cVector3d& a_min, const cVector3d& a_max)
    {
        m_extent = cMul(0.5, cSub(a_max, a_min));
        m_center = cAdd(a_min, m_extent);
        m_min = a_min;
        m_max = a_max;
    }
    //! Test whether this box contains the given point.
    inline bool contains(const cVector3d& a_p) const;
    //! Set the bounding box to bound the two given bounding boxes.
    void enclose(const cCollisionAABBBox& a_boxA, const cCollisionAABBBox& a_boxB);
    //! Modify the bounding box as needed to bound the given point.
    void enclose (const cVector3d& a_point);
    //! Set the bounding box to bound the given bounding box.
    void enclose(const cCollisionAABBBox& a_box) { enclose(*this, a_box); }
    //! Initialize a bounding box to center at origin and infinite extent.
    inline void setEmpty()
    {
        const double INFINITY = 1.0e50;
        m_center.zero();
        m_extent = cVector3d(-INFINITY, -INFINITY, -INFINITY);
        m_min.set(INFINITY, INFINITY, INFINITY);
        m_max.set(-INFINITY, -INFINITY, -INFINITY);
    }
    //! Return the smallest coordinate along X axis.
    inline double getLowerX() const  { return (m_min.x); }
    //! Return the largest coordinate along X axis.
    inline double getUpperX() const  { return (m_max.x); }
    //! Return the smallest coordinate along Y axis.
    inline double getLowerY() const  { return (m_min.y); }
    //! Return the largest coordinate along Y axis.
    inline double getUpperY() const  { return (m_max.y); }
    //! Return the smallest coordinate along Z axis.
    inline double getLowerZ() const  { return (m_min.z); }
    //! Return the largest coordinate along Z axis.
    inline double getUpperZ() const  { return (m_max.z); }
    //! Return the length of the longest axis of the bounding box.
    double size() const;
    //! Return the index of the longest axis of the bounding box.
    int longestAxis() const;
    //! Draw the edges of the bounding box.
    inline void render()
    {
        cDrawWireBox(m_min.x, m_max.x, m_min.y, m_max.y, m_min.z, m_max.z);
    }

    // PROPERTIES:
    //! The center of the bounding box.
    cVector3d m_center;
    //! The extent (half the width) of the bounding box.
    cVector3d m_extent;
    //! The minimum point (along each axis) of the bounding box.
    cVector3d m_min;
    //! The maximum point (along each axis) of the bounding box.
    cVector3d m_max;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

