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
#ifndef CTriangleH
#define CTriangleH
//---------------------------------------------------------------------------
#include "CMaths.h"
#include "CVertex.h"
#include "CMesh.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \struct   cTriangle
      \brief    cTriangle defines a triangle, typically bound to a mesh for
                graphic rendering.
*/
//===========================================================================
class cTriangle
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------
    /*!
        Constructor of cTriangle.

        \param      a_parent  Parent mesh.
        \param      a_indexVertex0  index position of vertex 0.
        \param      a_indexVertex1  index position of vertex 1.
        \param      a_indexVertex2  index position of vertex 2.
    */
    //-----------------------------------------------------------------------
    cTriangle(cMesh* a_parent, const unsigned int a_indexVertex0,
              const unsigned int a_indexVertex1, const unsigned int a_indexVertex2)
    {
        // set vertices of new triangle
        m_indexVertex0 = a_indexVertex0;
        m_indexVertex1 = a_indexVertex1;
        m_indexVertex2 = a_indexVertex2;

        // set parent of triangle
        m_parent = a_parent;

        // triangle is not yet allocated by a mesh
        m_allocated = false;

        m_tag = 0;
        m_neighbors = 0;
    }


    //-----------------------------------------------------------------------
    /*!
        Destructor of cTriangle.
    */
    //-----------------------------------------------------------------------
    ~cTriangle(){}


    // METHODS:
    //-----------------------------------------------------------------------
    /*!
        Set the vertices of the triangle by passing the index numbers of
        the corresponding vertices.

        \param      a_indexVertex0  index position of vertex 0.
        \param      a_indexVertex1  index position of vertex 1.
        \param      a_indexVertex2  index position of vertex 2.
    */
    //-----------------------------------------------------------------------
    inline void setVertices(const unsigned int a_indexVertex0,
                const unsigned int a_indexVertex1, const unsigned int a_indexVertex2)
    {
        m_indexVertex0 = a_indexVertex0;
        m_indexVertex1 = a_indexVertex1;
        m_indexVertex2 = a_indexVertex2;
    }

    
    //-----------------------------------------------------------------------
    /*!
        Read pointer to vertex 0 of triangle

        \return     Return pointer to vertex 0.
    */
    //-----------------------------------------------------------------------
    inline cVertex* getVertex0() const
    {
        return (&m_parent->m_vertices[m_indexVertex0]);
    };


    //-----------------------------------------------------------------------
    /*!
        Read pointer to vertex 1 of triangle

        \return     Return pointer to vertex 1.
    */
    //-----------------------------------------------------------------------
    inline cVertex* getVertex1() const
    {
        return (&m_parent->m_vertices[m_indexVertex1]);
    };


    //-----------------------------------------------------------------------
    /*!
        Read pointer to vertex 2 of triangle

        \return     Return pointer to vertex 2.
    */
    //-----------------------------------------------------------------------
    inline cVertex* getVertex2() const
    {
        return (&m_parent->m_vertices[m_indexVertex2]);
    };


    //-----------------------------------------------------------------------
    /*!
        Access a pointer to the specified vertex of this triangle

        \param      int vi  The triangle (0, 1, or 2) to access
        \return     Returns a pointer to the requested triangle, or 0 for an
                    illegal index
    */
    //-----------------------------------------------------------------------
    inline cVertex* getVertex(int vi) const
    {
        switch (vi)
        {
          case 0 : return (&m_parent->m_vertices[m_indexVertex0]);
          case 1 : return (&m_parent->m_vertices[m_indexVertex1]);
          case 2 : return (&m_parent->m_vertices[m_indexVertex2]);
        }
        return NULL;
    }



    //-----------------------------------------------------------------------
    /*!
        Read index number of vertex 0 (defines a location in my owning
        mesh's vertex array)

        \return     Return index number.
    */
    //-----------------------------------------------------------------------
    inline unsigned int getIndexVertex0() const
    {
        return (m_indexVertex0);
    };


    //-----------------------------------------------------------------------
    /*!
        Read index number of vertex 1 (defines a location in my owning
        mesh's vertex array)

        \return     Return index number.
    */
    //-----------------------------------------------------------------------
    inline unsigned int getIndexVertex1() const
    {
        return (m_indexVertex1);
    };


    //-----------------------------------------------------------------------
    /*!
        Read index number of vertex 2 (defines a location in my owning
        mesh's vertex array)

        \return     Return index number.
    */
    //-----------------------------------------------------------------------
    inline unsigned int getIndexVertex2() const
    {
        return (m_indexVertex2);
    };


    //-----------------------------------------------------------------------
    /*!
        Read the index of this triangle (defines a location in my owning
        mesh's triangle array)

        \return     Return index number.
    */
    //-----------------------------------------------------------------------
    inline unsigned int getIndex() const
    {
        return (m_index);
    };


    //-----------------------------------------------------------------------
    /*!
        Retrieve a pointer to the mesh that owns this triangle

        \return     Return pointer to parent mesh.
    */
    //-----------------------------------------------------------------------
    inline cMesh* getParent() const
    {
        return (m_parent);
    };

    //-----------------------------------------------------------------------
    /*!
        Set pointer to mesh parent of triangle.        
    */
    //-----------------------------------------------------------------------
    inline void setParent(cMesh* parent)
    {
      m_parent = parent;
    };


    //-----------------------------------------------------------------------
    /*!
        Is this triangle allocated to an existing mesh?

        \return     Return \b true if triangle is allocated to an existing
                    mesh, otherwise return \b false.
    */
    //-----------------------------------------------------------------------
    inline bool allocated() const
    {
        return (m_allocated);
    };



    //-----------------------------------------------------------------------
    /*!
        Check if a ray intersects this triangle. The ray is described
        by its origin (/e a_origin) and its direction (/e a_direction).

        If a collision occurs, the square distance between the ray origin
        and the collision point in measured and compared to any previous collision
        information stored in parameters \e a_colObject, \e a_colTriangle,
        \e a_colPoint, and \e a_colSquareDistance.
        
        If the new collision is located nearer to the ray origin than the previous
        collision point, it is stored in the corresponding parameters \e a_colObject,
        \e a_colTriangle, \e a_colPoint, and \e a_colSquareDistance.

        \param  a_rayOrigin  Point from where collision ray starts.
        \param  a_rayDir  Direction vector of collision ray.
        \param  a_colObject  Pointer to nearest collided object.
        \param  a_colTriangle Pointer to nearest collided triangle.
        \param  a_colPoint  Position of nearest collision.
        \param  a_colSquareDistance  Distance between ray origin and nearest
                collision point.
    */
    //-----------------------------------------------------------------------
    inline bool computeCollision(
                const cVector3d& a_rayOrigin, const cVector3d& a_rayDir,
                cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                cVector3d& a_colPoint, double& a_colSquareDistance)
    {

// This value controls how close rays can be to parallel to the triangle
// surface before we discard them
#define INTERSECT_EPSILON 10e-10f

 
        // Get the position of the triangle's vertices
        cVector3d t_vertex0 = m_parent->m_vertices[m_indexVertex0].getPos();
        cVector3d t_vertex1 = m_parent->m_vertices[m_indexVertex1].getPos();
        cVector3d t_vertex2 = m_parent->m_vertices[m_indexVertex2].getPos();
        
        cVector3d t_E0, t_E1, t_N;

        // Compute the triangle's normal
        t_vertex1.subr(t_vertex0, t_E0);
        t_vertex2.subr(t_vertex0, t_E1);
        t_E0.crossr(t_E1, t_N);

        // If the ray is parallel to the triangle (perpendicular to the
        // normal), there's no collision
        if (fabs(t_N.dot(a_rayDir))<10E-15f) return (false);
        
        double t_T = cDot(t_N, cSub(t_vertex0,a_rayOrigin)) / cDot(t_N, a_rayDir);

        //
        if (t_T + INTERSECT_EPSILON < 0) return (false);
        
        cVector3d t_Q = cSub(cAdd(a_rayOrigin, cMul(t_T, a_rayDir)), t_vertex0);
        double t_Q0 = cDot(t_E0,t_Q);
        double t_Q1 = cDot(t_E1,t_Q);
        double t_E00 = cDot(t_E0,t_E0);
        double t_E01 = cDot(t_E0,t_E1);
        double t_E11 = cDot(t_E1,t_E1);
        double t_D = (t_E00 * t_E11) - (t_E01 * t_E01);

        //
        if ((t_D > -10E-40f) && (t_D < 10E-40f)) return(false);        

        double t_S0 = ((t_E11 * t_Q0) - (t_E01 * t_Q1)) / t_D;
        double t_S1 = ((t_E00 * t_Q1) - (t_E01 * t_Q0)) / t_D;

        // 
        if (
          (t_S0 >= 0.0 - INTERSECT_EPSILON) &&
          (t_S1 >= 0.0 - INTERSECT_EPSILON) &&
          ((t_S0 + t_S1) <= 1.0 + INTERSECT_EPSILON)
          )
        {
            cVector3d t_I = cAdd(t_vertex0, cMul(t_S0,t_E0), cMul(t_S1, t_E1));
            double t_squareDistance = a_rayOrigin.distancesq(t_I);

            // Collision has occurred

            // If we've already seen a closer collision, don't report
            // this one
            if (t_squareDistance >= a_colSquareDistance) return(false);

            // Okay, we want to report this collision
            a_colObject = m_parent;
            a_colTriangle = this;
            a_colPoint = cAdd(a_rayOrigin, cMul(t_T, a_rayDir));
            a_colSquareDistance = t_squareDistance;

            // Tell the caller that we found a new collision
            return(true);            
            
        }
        else return(false);               
        
    }


    //-----------------------------------------------------------------------
    /*!
        Compute and return the area of this triangle

        \return     Returns the area of this triangle
    */
    //-----------------------------------------------------------------------
    double compute_area()
    {

      // A = 0.5 * | u x v |

      cVector3d u = cSub(getVertex(1)->getPos(),getVertex(0)->getPos());
      cVector3d v = cSub(getVertex(2)->getPos(),getVertex(0)->getPos());    

      return 0.5 * (cCross(u,v).length());

    }

    // MEMBERS:
    //! For custom use. No specific purpose.
    int m_tag;

    //! A mesh can be organized into a network of neighboring triangles, which are stored here...
    std::vector<cTriangle*>* m_neighbors;

  public:
    //! Index number of vertex 0 (defines a location in my owning mesh's vertex array)
    unsigned int m_indexVertex0;
    //! Index number of vertex 1 (defines a location in my owning mesh's vertex array)
    unsigned int m_indexVertex1;
    //! Index number of vertex 2 (defines a location in my owning mesh's vertex array)
    unsigned int m_indexVertex2;
    //! Index number of this triangle (defines a location in my owning mesh's triangle array)
    unsigned int m_index;
    //! The mesh that owns me
    cMesh* m_parent;
    //! Is this triangle still active?
    bool m_allocated;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------


