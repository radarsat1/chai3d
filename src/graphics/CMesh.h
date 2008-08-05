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
    \author:    Dan Morris
    \author:    Chris Sewell
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
      \brief      cMesh represents a collection of vertices, triangles, materials,
                  and texture properties that can be rendered graphically and
                  haptically.
*/
//===========================================================================
class cMesh : public cGenericObject
{

  // cTriangle accesses cMesh's vertex arrays directly
  friend cTriangle;

  public:
    // CONSTRUCTOR & DESTRUCTOR:

    //! Constructor of cMesh.
    cMesh(cWorld* a_world);
    //! Destructor of cMesh.
    virtual ~cMesh();

    // METHODS - GENERAL
    //! Get parent world
    cWorld* getParentWorld() { return (m_parentWorld); }

    //! Load a 3D object file (CHAI currently supports .obj and .3ds files)
    virtual bool loadFromFile(const string& a_fileName);


    // METHODS - VERTICES
    
    //! Create a new vertex and add it to the vertex list.
    unsigned int newVertex(const double a_x, const double a_y, const double a_z);
    //! Create a new vertex and add it to the vertex list.
    unsigned int newVertex(const cVector3d& a_pos) { return( newVertex(a_pos.x, a_pos.y, a_pos.z) ); }

    //! Remove the vertex at the specified position in my vertex array
    bool removeVertex(const unsigned int a_index);
    
    //! Access the vertex at the specified position in my vertex array (and maybe my childrens' arrays)
    inline cVertex* getVertex(unsigned int a_index, bool a_includeChildren = false);

    //! Read the number of stored vertices, optionally including those of my children
    unsigned int getNumVertices(bool a_includeChildren = false) const;
    
    //! Access my vertex list directly (use carefully)
    vector<cVertex>* pVertices() { return (&m_vertices); }


    // METHODS - TRIANGLES

    //! Create a new triangle by passing vertex indices.
    unsigned int newTriangle(const unsigned int a_indexVertex0,
                             const unsigned int a_indexVertex1, const unsigned int a_indexVertex2);
    
    //! Create a new triangle and three new vertices by passing vertex positions
    unsigned int newTriangle(const cVector3d& a_vertex0, const cVector3d& a_vertex1,
                             const cVector3d& a_vertex2);

    //! Remove a triangle from my triangle array
    bool removeTriangle(const unsigned int a_index);

    //! Access the triangle at the specified position in my triangle array
    cTriangle* getTriangle(unsigned int a_index, bool a_includeChildren = false);
    
    //! Read the number of stored triangles, optionally including those of my children
    unsigned int getNumTriangles(bool a_includeChildren = false) const;

    //! Clear all triangles and vertices of mesh.
    void clear();

    //! Access my triangle array directly (use carefully)
    vector<cTriangle>* pTriangles() { return (&m_triangles); }


    // METHODS - GRAPHIC RENDERING

    //! Set the material for this mesh, and optionally pass it on to my children
    void setMaterial(cMaterial& a_mat, const bool a_affectChildren=false);
    
    //! Compute all triangle normals, optionally propagating the operation to my children
    void computeAllNormals(const bool a_affectChildren=0);

    //! Set the alpha value at each vertex and in all of my material colors
    void setTransparencyLevel(const float a_level, const bool a_applyToTextures=false, const bool a_affectChildren=true);    
    //! Specify whether this mesh should use multipass transparency (see cCamera)
    void setTransparencyRenderMode(const bool a_useMultipassTransparency, const bool a_affectChildren=true);      
    //! Enable or disable transparency (also see setTransparencyRenderMode)... turns the depth mask _off_!
    void enableTransparency(const bool a_useTransparency, const bool a_affectChildren=true);
    // ! Is transparency enabled for this mesh?
    bool getTransparencyEnabled() const { return m_useTransparency; }

    //! Enable or disable wireframe rendering, optionally propagating the operation to my children
    void setWireMode(const bool a_showWireMode, const bool a_affectChildren=true);

    //! Set color of each vertex, optionally propagating the operation to my children
    void setVertexColor(const cColorb& a_color, const bool a_affectChildren=true);

    //! Enable or disable the use of a display list for rendering, optionally propagating the operation to my children
    void useDisplayList(const bool a_useDisplayList, const bool a_affectChildren=true);
    //! Ask whether I'm currently rendering with a display list
    bool getDisplayListEnabled() const { return m_useDisplayList; }
    //! Invalidate any existing display lists
    void invalidateDisplayList(const bool a_affectChildren=true);

    //! Enable or disabling face-culling, optionally propagating the operation to my children
    void useCulling(const bool a_useCulling, const bool a_affectChildren=true);
    //! Is face-culling currently enabled?
    bool getCullingEnabled() const { return m_cullingEnabled; }

    //! Enable or disable the use of per-vertex colors, optionally propagating the operation to my children
    void useColors(const bool a_useColors, const bool a_affectChildren=true);
    // ! Are vertex colors currently enabled?
    bool getColorsEnabled() const { return m_useVertexColors; }

    //! Enable or disable the use of material properties, optionally propagating the operation to my children
    void useMaterial(const bool a_useMaterial, const bool a_affectChildren=true);
    // ! Are material properties currently enabled?
    bool getMaterialEnabled() const { return m_useMaterialProperty; }
    
    //! Enable or disable the rendering of vertex normals, optionally propagating the operation to my children
    void showNormals(const bool a_showNormals, const bool a_affectChildren=true);
    //! Set graphic properties for normal-rendering, optionally propagating the operation to my children
    void setNormalsProperties(const double a_length, const cColorf& a_color, const bool a_affectChildren);

    //! Set the haptic stiffness, possibly recursively affecting children
    void setStiffness(double a_stiffness, const bool a_affectChildren=0);
    //! Set the static and dynamic friction for this mesh, possibly recursively affecting children
    void setFriction(double a_staticFriction, double a_dynamicFriction, const bool a_affectChildren=0);
    
    //! Enable or disable the use of texture-mapping, optionally propagating the operation to my children
    void useTexture(const bool a_useTexture, const bool a_affectChildren=true);
    //! Is texture-mapping enabled?
    bool getTextureEnabled() const { return m_useTextureMapping; }

    //! Set my texture, possibly recursively affecting children
    void setTexture(cTexture2D* a_texture, const bool a_affectChildren=0);
    //! Access my texture
    cTexture2D* getTexture() const { return(m_texture); }

    //! Re-initializes textures and display lists
    virtual void onDisplayReset(const bool a_affectChildren = true);


    // METHODS - COLLISION DETECTION:

    //! Set up an AABB collision detector for this mesh and (optionally) its children
    virtual void createAABBCollisionDetector(bool a_affectChildren, bool a_useNeighbors);
    //! Set up a sphere tree collision detector for this mesh and (optionally) its children
    virtual void createSphereTreeCollisionDetector(bool a_affectChildren, bool a_useNeighbors);

    //! Help create neighbor lists for triangles to speed up collision detection.
    void findNeighbors(std::vector<cTriangle*>* search1,
                             std::vector<cTriangle*>* search2, int v1, int v2);

    // METHODS - MESH MANIPULATION:

    //! Extrude each vertex of the mesh by some amount along its normal
    void extrude(const double a_extrudeDistance, const bool a_affectChildren=0,
      const bool a_updateCollisionDetector=0);

    //! Simple method used to create new meshes "just like me".
    virtual cMesh* createMesh() { return new cMesh(m_parentWorld); }

    //! Render triangles, material and texture properties.
    virtual void renderMesh(const int a_renderMode=0);

		// MEMBERS:
    // material property of mesh
    cMaterial m_material;

  protected:
   
    // METHODS:

    //! Render the mesh itself
    virtual void render(const int a_renderMode=0);
    //! Draw a small line for each vertex normal
    virtual void renderNormals();
    
    //! Update the global position of each of my vertices
    virtual void updateGlobalPositions(const bool a_frameOnly);
    //! Update my boundary box dimensions based on my vertices
    virtual void updateBoundaryBox();
    //! Scale the mesh by the specified scale factor
    virtual void scaleObject(double a_scaleFactor);

    // MEMBERS - DISPLAY PROPERTIES:

    //! Parent world
    cWorld *m_parentWorld;

    //! If \b true, then normals are displayed.
    bool m_showNormals;
    //! Color used to render lines representing normals
    cColorf m_showNormalsColor;
    //! Length of each normal (for graphic rendering of normals)
    double m_showNormalsLength;

    //===========================================================================
    /*! 
        If true, transparency is enabled... this turns alpha on when the mesh is
        rendered, and - if multipass transparency is enabled in the rendering camera -
        uses the camera's multiple rendering passes to approximate back-to-front
        sorting via culling
    */
    //===========================================================================
    bool m_useTransparency;

    //===========================================================================
    /*! 
        If true, multi-pass transparency is permitted for this mesh... this means
        that if the rendering camera is using multipass transparency, this mesh
        will render back and front faces separately.

        Note that m_useTransparency also has to be 'true' for this variable to
        be meaningful.
    */
    //===========================================================================
    bool m_useMultipassTransparency;

    //===========================================================================
    /*! 
        Should culling be used when rendering triangles?
    
        Note that this option only applies when multipass transparency is 
        disabled or during the non-transparent rendering pass when multipass
        transparency is enabled...
        
        Also note that currently only back-faces are culled during non-transparent
        rendering; you can't cull front-faces.
    */
    //===========================================================================    
    bool m_cullingEnabled;
    
    //! The polygon rendering mode (GL_FILL or GL_LINE)
    int m_triangleMode;
    //! Should per-vertex colors be used?
    bool m_useVertexColors;
    //! Should texture mapping be used?
    bool m_useTextureMapping;
    //! Should material properties be used?
    bool m_useMaterialProperty;
    //! My texture
    cTexture2D* m_texture;

    //! Should we use a display list to render this mesh?
    bool m_useDisplayList;
    //! The openGL display list used to draw this mesh, if display lists are enabled
    int m_displayList;


    // MEMBERS - ARRAYS:

    //! Array of vertices
    vector<cVertex> m_vertices;
    //! List of free slots in the vertex array
    list<unsigned int> m_freeVertices;
    //! Array of triangles
    vector<cTriangle> m_triangles;
    //! List of free slots in the triangle array
    list<unsigned int> m_freeTriangles;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

