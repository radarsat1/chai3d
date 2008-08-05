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
#include "CCollisionAABBBox.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      Set the bounding box to bound the two given bounding boxes.

      \fn       void cCollisionAABBBox::enclose(const cCollisionAABBBox& a_boxA,
                const cCollisionAABBBox& a_boxB)
      \param    a_boxA   The first bounding box to be enclosed.
      \param    a_boxB   The other bounding box to be enclosed.
*/
//===========================================================================
void cCollisionAABBBox::enclose(const cCollisionAABBBox& a_boxA,
                        const cCollisionAABBBox& a_boxB)
{
    // find the minimum of each coordinates
    cVector3d lower(
        cMin(a_boxA.getLowerX(), a_boxB.getLowerX()),
        cMin(a_boxA.getLowerY(), a_boxB.getLowerY()),
        cMin(a_boxA.getLowerZ(), a_boxB.getLowerZ())
    );

    // find the maximum of each component
	cVector3d upper(
        cMax(a_boxA.getUpperX(), a_boxB.getUpperX()),
		cMax(a_boxA.getUpperY(), a_boxB.getUpperY()),
		cMax(a_boxA.getUpperZ(), a_boxB.getUpperZ())
    );

    // set the propertyies of this box to enclose the two given boxes
	setValue(lower, upper);
}


//===========================================================================
/*!
      Modify the bounding box as needed to bound the given point.

      \fn       void cCollisionAABBBox::enclose (const cVector3d& a_point)
      \param    a_point  The point to be bounded.
*/
//===========================================================================
void cCollisionAABBBox::enclose (const cVector3d& a_point)
{
    // decrease coordinates as needed to include given point
	cVector3d lower(
        cMin(getLowerX(), a_point.x),
		cMin(getLowerY(), a_point.y),
		cMin(getLowerZ(), a_point.z)
    );

    // increase coordinates as needed to include given point
	cVector3d upper(
        cMax(getUpperX(), a_point.x),
		cMax(getUpperY(), a_point.y),
		cMax(getUpperZ(), a_point.z)
    );

    // set the properties of this box to enclose the given point
	setValue(lower, upper);
}


//===========================================================================
/*!
      Return the index of the longest axis of the bounding box.

      \fn         int cCollisionAABBBox::longestAxis() const
      \return     Returns the index of the longest axis of the box.
*/
//===========================================================================
int cCollisionAABBBox::longestAxis() const
{
	if ((m_extent.x >= m_extent.y) && (m_extent.x >= m_extent.z))
    {
		return 0;
    }
	else if ((m_extent.y >= m_extent.x) && (m_extent.y >= m_extent.z))
    {
		return 1;
    }
    else
    {
	    return 2;
    }
}

