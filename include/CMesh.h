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
#ifdef _MSVC
#pragma warning (disable : 4786)
#endif
//---------------------------------------------------------------------------
#ifndef CMeshH
#define CMeshH
//---------------------------------------------------------------------------
#include "CGenericObject.h"
#include "CMaterial.h"
#include "CTexture2D.h"
#include "CColor.h"
//---------------------------------------------------------------------------
#include "gl/glu.h"
#include <vector>
#include <list>
//---------------------------------------------------------------------------
using std::list;
using std::vector;
//---------------------------------------------------------------------------
class cWorld;
class cTriangle;
class cVertex;
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class      cMesh
      \brief      cMesh hold a collection of vertices, triangles, material
                  and texture properties to graphically describe and render
                  virtual objects.
*/
//===========================================================================
class cMesh : public cGenericObject
{
  friend cTriangle;

  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cMesh.
    cMesh(cWorld* a_parent);
    //! Destructor of cMesh.
    virtual ~cMesh();

    // METHODS - GENERAL
    //! Get parent world.
    cWorld* getParentWorld() { return (m_parentWorld); }
    //! Load an 3D object file.
    bool loadFromFile(const string& a_fileName);

    // METHODS - VERTICES
    //! Create a new vertex and add it to the vertex list.
    unsigned int newVertex(const double a_x, const double a_y, const double a_z);
    //! Create a new vertex and add it to the vertex list.
    unsigned int newVertex(const cVector3d& a_pos) { return( newVertex(a_pos.x, a_pos.y, a_pos.z) ); }
    //! Remove vertex from array.
    bool removeVertex(const unsigned int a_index);
    //! Read vertex by passing its index position in array.
    inline cVertex* getVertex(unsigned int a_index) { return (&m_vertices[a_index]); }
    //! Read the number of vertices.
    unsigned int getNumVertices(bool a_includeChildren = false) const;
    //! Return pointer to vertex list
    vector<cVertex>* pVertices() { return (&m_vertices); }

    // METHODS - TRIANGLES
    //! Create a new triangle by passing vertex indices.
    unsigned int newTriangle(const unsigned int a_indexVertex0,
                             const unsigned int a_indexVertex1, const unsigned int a_indexVertex2);
    //! Create a new triangle and vertices by passing vertex positions.
    unsigned int newTriangle(const cVector3d& a_vertex0, const cVector3d& a_vertex1,
                             const cVector3d& a_vertex2);
    //! Remove triangle from array.
    bool removeTriangle(const unsigned int a_index);
    //! Read triangle by passing its index position in array.
    inline cTriangle* getTriangle(unsigned int a_index)
    {
        if (a_index >= m_triangles.size()) return 0;
        else return (&m_triangles[a_index]);
    }
    //! Read the number of triangles.
    unsigned int getNumTriangles(bool a_includeChildren = false) const;
    //! Clear all triangles and vertices of mesh.
    void clear();
    //! Return pointer to triangles list
    vector<cTriangle>* pTriangles() { return (&m_triangles); }

    // METHODS - GRAPHIC RENDERING
    //! Compute all normals of triangles.
    void computeAllNormals(const bool a_affectChildren=0);
    //! Set the level of transparency.
    void setTransparencyLevel(const float a_level, const bool a_applyToTextures, const bool a_affectChildren);
    //! Set the triangle in full surface or wireframe.
    void setWireMode(const bool a_showWireMode, const bool a_affectChildren);
    //! Set color of each vertex.
    void setVertexColor(const cColorb& a_color, const bool a_affectChildren);
    //! Use color arrays
    void useColors(const bool a_useColors, const bool a_affectChildren);
    //! Use material property
    void useMaterial(const bool a_useMaterial, const bool a_affectChildren);
    //! Use texture mapping
    void useTexture(const bool a_useTexture, const bool a_affectChildren);
    //! Set the way normals are displayed
    void setNormalsProperties(const double a_length, const cColorf& a_color, const bool a_affectChildren);
    //! Show normals
    void showNormals(const bool a_showNormals, const bool a_affectChildren);
    //! Set the texture, possibly recursively affecting children
    void setTexture(cTexture2D* a_texture, const bool a_affectChildren=0);
    //! Set the haptic stiffness, possibly recursively affecting children
    void setStiffness(double a_stiffness, const bool a_affectChildren=0);
    //! Set the haptic friction, possibly recursively affecting children
    void setFriction(double a_staticFriction, double a_dynamicFriction, const bool a_affectChildren=0);
    //! Read the current texture.
    cTexture2D* getTexture() { return(m_texture); }

    // METHODS - COLLISION DETECTION
    //! Setup an AABB collision detector to current mesh and children
    void createAABBCollisionDetector(bool a_affectChildren, bool a_useNeighbors);
    //! Setup a sphere tree collision detector to current mesh and children.
    void createSphereTreeCollisionDetector(bool a_affectChildren, bool a_useNeighbors);
    //! Help create neighbor lists for triangles to speed up collision detection.
    void findNeighbors(std::vector<cTriangle*>* search1,
                             std::vector<cTriangle*>* search2, int v1, int v2);
    //! Set the material for this mesh, and optionally pass it on to children
    void setMaterial(cMaterial& a_mat, const bool a_affectChildren=0);

    // MEMBERS:
    // material property of mesh
    cMaterial m_material;

  protected:
   // METHODS:
    //! Render the mesh object
    virtual void render(const int a_renderMode=0);
    //! Draw a small line for each vertex.
    void renderNormals();
    //! Render triangles, material and texture properties.
    void renderMesh(const int a_renderMode=0);

    // VIRTUAL METHODS
    //! Update global positions
    virtual void updateGlobalPositions(const bool a_frameOnly);
    //! Update boundary box dimensions
    virtual void updateBoundaryBox();
    //! Scale mesh of defined scale factor
    virtual void scaleObject(double a_scaleFactor);

    // MEMBERS - DISPLAY PROPERTIES:
    //! World parent
    cWorld *m_parentWorld;
    //! If \b true, then normals are displayed.
    bool m_showNormals;
    //! Color used to render lines representing normals.
    cColorf m_showNormalsColor;
    //! Length of each normal for graphic rendering purpose.
    double m_showNormalsLength;
    
    //! If TRUE, transparency mode is used.
    bool m_useTransparency;
    //! Defines the triangle mode.
    int m_triangleMode;
    //! Should color be used?
    bool m_useVertexColors;
    //! Should texture mapping be used?
    bool m_useTextureMapping;
    //! Should material be used?
    bool m_useMaterialProperty;
    //! texture
    cTexture2D* m_texture;

  private:
    // MEMBERS - ARRAYS:
    //! Array of vertices.
    vector<cVertex> m_vertices;
    //! List of free vertices in the array.
    list<unsigned int> m_freeVertices;
    //! Array of triangles.
    vector<cTriangle> m_triangles;
    //! List of free triangles
    list<unsigned int> m_freeTriangles;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

