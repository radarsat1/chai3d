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
#include "CMesh.h"
#include "CVertex.h"
#include "CTriangle.h"
#include "CMeshLoader.h"
#include "CCollisionBrute.h"
#include "CCollisionAABB.h"
#include "CCollisionSpheres.h"
#include <algorithm>
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cMesh

    \fn       cMesh::cMesh(cWorld* a_parent)
    \param    a_parent  Pointer to parent world.
*/
//===========================================================================
cMesh::cMesh(cWorld* a_parent)
{
    // initialize parent object of mesh. Not yet a child on an other object.
    m_parent = NULL;

    // set parent world of mesh
    m_parentWorld = a_parent;

    // should normals be displayed?
    m_showNormals = false;

    // if normals are displayed, this value defines their length.
    m_showNormalsLength = 0.1;

    // if normals are displayed, this defines their color
    m_showNormalsColor.set(1.0, 0.0, 0.0);

    // should the frame (X-Y-Z) be displayed?
    //
    // Not by default... this is really a debugging tool.
    m_showFrame = false;

    // should we use the material property?
    m_useMaterialProperty = true;

    // Are vertex colors used during rendering process?
    m_useVertexColors = false;

    // Should transparency be used?
    m_useTransparency = false;

    // Should texture mapping be used if a texture is defined?
    m_useTextureMapping = true;

    // How are triangles displayed; FILL or LINE ?
    m_triangleMode = GL_FILL;
    
    // initialize texture
    m_texture = NULL;

    // set default collision detector
    m_collisionDetector = new cCollisionBrute(&m_triangles);
}


//===========================================================================
/*!
    Destructor of cMesh.

    \fn     cMesh::~cMesh()
*/
//===========================================================================
cMesh::~cMesh()
{
    // clear vertex array
    m_vertices.clear();

    // clear list of free vertices
    m_freeVertices.clear();

    // clear triangle array
    m_triangles.clear();

    // clear list of free triangle.
    m_freeTriangles.clear();
}


//===========================================================================
/*!
     Read the number of vertices contained in this mesh, and its children
     if specified.

     \fn        unsigned int cMesh::getNumVertices(bool a_includeChildren) const
     \param     a_includeChildren  If \b true, then children are also included.
*/
//===========================================================================
unsigned int cMesh::getNumVertices(bool a_includeChildren) const
{
    // get number of vertices of current object
    unsigned int numVertices = m_vertices.size();

    // apply computation to children if specified
    if (a_includeChildren)
    {
        unsigned int i, numItems;
        numItems = m_children.size();
        for (i=0; i<numItems; i++)
        {
            cGenericObject *nextObject = m_children[i];

            // check if nextobject is a mesh.
            if (typeid(*nextObject) == typeid(cMesh))
            {
                cMesh* nextMesh = (cMesh*)nextObject;
                numVertices = numVertices + nextMesh->getNumVertices(a_includeChildren);
            }
        }
    }

    // return result
    return (numVertices);
}


//===========================================================================
/*!
     Read the number of triangle contained in this mesh, and its children
     if specified.

     \fn        unsigned int cMesh::getNumTriangles(bool a_includeChildren) const
     \param     a_affectChildren  If \b true, then children are also included.
*/
//===========================================================================
unsigned int cMesh::getNumTriangles(bool a_includeChildren) const
{
    // get number of triangle of current object
    unsigned int numTriangles = m_triangles.size();

    // apply computation to children if specified
    if (a_includeChildren)
    {
        unsigned int i, numItems;
        numItems = m_children.size();
        for (i=0; i<numItems; i++)
        {
            cGenericObject *nextObject = m_children[i];

            // check if nextobject is a mesh.
            if (typeid(*nextObject) == typeid(cMesh))
            {
                cMesh* nextMesh = (cMesh*)nextObject;
                numTriangles = numTriangles + nextMesh->getNumTriangles(a_includeChildren);
            }
        }
    }

    // return result
    return (numTriangles);
}


//===========================================================================
/*!
    Create a new vertex and add it to the vertex list.

    \fn         unsigned int cMesh::newVertex(const double a_x, const double
                a_y, const double a_z)
    \param      a_x  X coordinate of vertex.
    \param      a_y  Y coordinate of vertex.
    \param      a_z  Z coordinate of vertex.
    \return     Return index position of new vertex.
*/
//===========================================================================
unsigned int cMesh::newVertex(const double a_x, const double a_y, const double a_z)
{
    unsigned int index;

    // check if there is any available vertex on the free list
    if (m_freeVertices.size() > 0)
    {
        index = m_freeVertices.front();
        m_freeVertices.erase(0);
    }

    // No vertex is available on the free list so create a new one from the array
    else
    {
        // allocate new vertex
        index = m_vertices.size();
        cVertex newVertex(a_x, a_y, a_z);
        newVertex.m_index = index;
        m_vertices.push_back(newVertex);
    }

    // return index position in array
    return (index);
}


//===========================================================================
/*!
    Remove a vertex from the vertex array by passing its index number.

    \fn       bool cMesh::removeVertex(const unsigned int a_index)
    \param    a_index  Index number of vertex.
    \return   Return \b true if operation succeded.
*/
//===========================================================================
bool cMesh::removeVertex(const unsigned int a_index)
{
    // get vertex to be removed
    cVertex* vertex = &m_vertices[a_index];

    // check if it has not already been removed
    if (vertex->m_allocated == false) { return (false); }

    // desactivate vertex
    vertex->m_allocated = false;

    // add vertex to free list
    m_freeVertices.push_back(a_index);

    // return success
    return (true);
}


//===========================================================================
/*!
    Create a new triangle by passing vertex, normal and texture
    coordinate indices.

    \fn         unsigned int cMesh::newTriangle(const unsigned int a_indexVertex0,
                const unsigned int a_indexVertex1,
                const unsigned int a_indexVertex2)
    \param      a_indexVertex0   index position of vertex 0.
    \param      a_indexVertex1   index position of vertex 1.
    \param      a_indexVertex2   index position of vertex 2.
    \return     Return index position of new triangle.
*/
//===========================================================================
unsigned int cMesh::newTriangle(const unsigned int a_indexVertex0, const unsigned int a_indexVertex1,
             const unsigned int a_indexVertex2)
{
    unsigned int index;



    // check if there is an available triangle on the free list
    if (m_freeTriangles.size() > 0)
    {
        index = m_freeTriangles.front();
        m_triangles[index].m_allocated = true;
        m_freeTriangles.erase(0);
        m_triangles[index].setVertices(a_indexVertex0,a_indexVertex1,a_indexVertex2);
        m_triangles[index].setParent(this);
    }

    // No triangle is available from the free list so create a new one from the array
    else
    {
        // allocate new triangle
        index = m_triangles.size();
        cTriangle newTriangle(this, a_indexVertex0, a_indexVertex1, a_indexVertex2);
        newTriangle.m_index = index;
        newTriangle.m_allocated = true;
        m_triangles.push_back(newTriangle);
    }

    // return index position in array
    return (index);
}


//===========================================================================
/*!
     Create a new triangle with three new vertices by passing the position
     of each vertex.

     \fn       unsigned int cMesh::newTriangle(const cVector3d& a_vertex0,
               const cVector3d& a_vertex1, const cVector3d& a_vertex2);
     \param    a_vertex0   Position of vertex 0.
     \param    a_vertex1   Position of vertex 1.
     \param    a_vertex2   Position of vertex 2.
     \return   Return index position of new triangle.
*/
//===========================================================================
unsigned int cMesh::newTriangle(const cVector3d& a_vertex0, const cVector3d& a_vertex1,
                                const cVector3d& a_vertex2)
{
    // create three new vertices
    unsigned int indexVertex0 = newVertex(a_vertex0);
    unsigned int indexVertex1 = newVertex(a_vertex1);
    unsigned int indexVertex2 = newVertex(a_vertex2);

    // create new triangle
    unsigned int indexTriangle = newTriangle(indexVertex0, indexVertex1, indexVertex2);

    // return index of new triangle.
    return (indexTriangle);
}


//===========================================================================
/*!
     Remove a vertex from the vertex array by passing its index number.

     \fn       bool cMesh::removeTriangle(const unsigned int a_index)
     \param    a_index  Index number of vertex.
     \return   Return \b true if operation succeded.
*/
//===========================================================================
bool cMesh::removeTriangle(const unsigned int a_index)
{
    // get triangle to be removed
    cTriangle* triangle = &m_triangles[a_index];

    // check if it has not already been removed
    if (triangle->m_allocated == false) { return (false); }

    // desactivate triangle
    triangle->m_allocated = false;

    // add triangle to free list
    m_freeTriangles.push_back(a_index);

    // return success
    return (true);
}


//===========================================================================
/*!
     Clear all triangles and vertices.

     \fn       void cMesh::clear()
*/
//===========================================================================
void cMesh::clear()
{
    // clear all triangles
    m_triangles.clear();

    // clear all vertices
    m_vertices.clear();

    // clear free lists
    m_freeTriangles.clear();
    m_freeVertices.clear();
}


//===========================================================================
/*!
     Load a 3D graphic file.

     \fn       bool cMesh::loadFromFile(const string& a_fileName)
     \param    a_fileName  Filename of 3d image.
     \return   Return \b true is file loaded correctly. Otherwize return FALSE.
*/
//===========================================================================
bool cMesh::loadFromFile(const string& a_fileName)
{
    return ( cLoadMeshFromFile(this, a_fileName) );
}


//===========================================================================
/*!
     Compute all surface normals for every triangle composing the mesh.
     Call this method when the topology of the mesh changes, which means
     when the position of the vertices have been modified.

     \fn       void cMesh::computeAllNormals()
     \param    a_affectChildren  If \b true, then children are also updated.
*/
//===========================================================================
void cMesh::computeAllNormals(const bool a_affectChildren)
{
    unsigned int i;

    // set all normal to zero
    for(i=0; i<m_vertices.size(); i++)
    {
        cVertex *nextVertex = getVertex(i);
        nextVertex->m_normal.zero();
    }

    // compute normals for all triangles.
    for(i=0; i<m_triangles.size(); i++)
    {
        // get next triangle
        cTriangle* nextriangle = &m_triangles[i];

        // extract positions of vertices
        cVector3d vertex0 = m_vertices[nextriangle->m_indexVertex0].getPos();
        cVector3d vertex1 = m_vertices[nextriangle->m_indexVertex1].getPos();
        cVector3d vertex2 = m_vertices[nextriangle->m_indexVertex2].getPos();

        // compute normal vector
        cVector3d normal, v01, v02;
        vertex1.subr(vertex0, v01);
        vertex2.subr(vertex0, v02);
        v01.crossr(v02, normal);
        double length = normal.length();
        if (length > 0.0000001)
        {
            normal.div(length);
            m_vertices[nextriangle->m_indexVertex0].m_normal.add(normal);
            m_vertices[nextriangle->m_indexVertex1].m_normal.add(normal);
            m_vertices[nextriangle->m_indexVertex2].m_normal.add(normal);
        }
    }

    // normalize all normals
    for(i=0; i<m_vertices.size(); i++)
    {
        cVertex *nextVertex = getVertex(i);
        if (nextVertex->m_normal.lengthsq() > 0.00000001)
        {
            nextVertex->m_normal.normalize();
        }
    }

    // update changes to children
    if (a_affectChildren)
    {
        unsigned int i, numItems;
        numItems = m_children.size();
        for (i=0; i<numItems; i++)
        {
            cGenericObject *nextObject = m_children[i];

            // check if nextobject is a mesh. if yes, apply changes
            if (dynamic_cast<cMesh*>(nextObject))
            {
                cMesh* nextMesh = (cMesh*)nextObject;
                nextMesh->computeAllNormals(a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Compute the global position of all vertices.

     \fn       void cMesh::updateGlobalPositions(const bool a_frameOnly)
     \param    a_frameOnly  If \b false then global position of all vertices
               are computed.
*/
//===========================================================================
void cMesh::updateGlobalPositions(const bool a_frameOnly)
{
    if (!a_frameOnly)
    {
        unsigned int i,numItems;
        numItems = m_vertices.size();
        for (i=0; i<numItems; i++)
        {
            m_vertices[i].computeGlobalPosition(m_globalPos,m_globalRot);
        }
    }
}


//===========================================================================
/*!
     Set if the triangles should be displayed in wireframe mode or in full.

     \fn        void cMesh::setWireMode(const bool a_showWireMode,
                const bool a_affectChildren)
     \param     a_showWireMode  If \b true, wireframe mode is used.
     \param     a_affectChildren  If \b true, then children are also updated.
*/
//===========================================================================
void cMesh::setWireMode(const bool a_showWireMode, const bool a_affectChildren)
{
    // update changes to object
    if (a_showWireMode)  { m_triangleMode = GL_LINE; }
    else { m_triangleMode = GL_FILL; }

    // update changes to children
    if (a_affectChildren)
    {
        unsigned int i, numItems;
        numItems = m_children.size();
        for (i=0; i<numItems; i++)
        {
            cGenericObject *nextObject = m_children[i];

            // check if nextobject is a mesh. if yes, apply changes
            if (dynamic_cast<cMesh*>(nextObject))
            {
                cMesh* nextMesh = (cMesh*)nextObject;
                nextMesh->setWireMode(a_showWireMode, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Set the transparency level of each triangle of the solid.

     \fn        void cMesh::setTransparencyLevel(const float a_level,
     \          const bool a_applyToTextures,, const bool a_affectChildren)
     \param     a_level  Level of transparency ranging from 0.0 to 1.0.
     \param     a_applyToTextures  If \b true, then apply changes to texture
     \param     a_affectChildren  If true, then children also modified.
*/
//===========================================================================
void cMesh::setTransparencyLevel(const float a_level, const bool a_applyToTextures,
                                 const bool a_affectChildren)
{
    // turn off transparency mode if level is equal to 100% opacity.
    if (a_level == 1.0)
    {
        m_useTransparency = false;
    }
    else
    {
        m_useTransparency = true;
    }

    // apply new value to material
    m_material.setTransparencyLevel(a_level);

    // convert transparency level for cColorb format
    GLubyte level = GLubyte(255.0f * a_level);

    // apply new value to all vertex colors
    unsigned int i, numItems;
    numItems = m_vertices.size();
    for(i=0; i<numItems; i++)
    {
        m_vertices[i].m_color.setA(level);
    }

    // apply changes to texture if required
    if (a_applyToTextures && (m_texture != NULL))
    {
        // m_texture->setTransparency(a_level);
    }

    // set transparency level to children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            // check if nextobject is a mesh. if yes, apply changes.
            if (typeid(*nextObject) == typeid(cMesh))
            {
                cMesh *nextMesh = (cMesh*)nextObject;
                nextMesh->setTransparencyLevel(a_level, a_applyToTextures,
                                               a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Set color of each vertex.

     \fn        void cMesh::setVertexColor(const cColor& a_color,
                const bool a_affectChildren)
     \param     a_color   New color to be applied to each vertex
     \param     a_affectChildren  If \b true, then children also modified.
*/
//===========================================================================
void cMesh::setVertexColor(const cColorb& a_color, const bool a_affectChildren)
{
    // apply color to all vertex colors
    unsigned int i, numItems;
    numItems = m_vertices.size();
    for(i=0; i<numItems; i++)
    {
        m_vertices[i].m_color = a_color;
    }

    // update changes to children
    if (a_affectChildren)
    {
        numItems = m_children.size();
        for (i=0; i<numItems; i++)
        {
            cGenericObject *nextObject = m_children[i];

            if (typeid(*nextObject) == typeid(cGenericObject))
            {
                cMesh *nextMesh = (cMesh*)nextObject;
                nextMesh->setVertexColor(a_color, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Use color information of vertices when rendering the triangles. If this
     feature is not activated, then material property is used

     \fn       void cMesh::useColors(const bool a_useColors, const bool a_affectChildren)
     \param    a_useColors   If \b true, then vertex color information is applied.
     \param    a_affectChildren  If \b true, then children are also modified.
*/
//===========================================================================
void cMesh::useColors(const bool a_useColors, const bool a_affectChildren)
{
    // update changes to object
    m_useVertexColors = a_useColors;

    // update changes to children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            if (typeid(*nextObject) == typeid(cMesh))
            {
                cMesh *nextMesh = (cMesh*)nextObject;
                nextMesh->useColors(a_useColors, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Use material property information.

     \fn        void cMesh::useMaterial(const bool a_useMaterial,
                const bool a_affectChildren)
     \param     a_useMaterial If \b true, then material prperty is used.
     \param     a_affectChildren  If \b true, then children are also modified.
*/
//===========================================================================
void cMesh::useMaterial(const bool a_useMaterial, const bool a_affectChildren)
{
    // update changes to object
    m_useMaterialProperty = a_useMaterial;

    // apply changes to children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            if (typeid(*nextObject) == typeid(cMesh))
            {
                cMesh *nextMesh = (cMesh*)nextObject;
                nextMesh->useMaterial(a_useMaterial, a_affectChildren);
            }
        }
    }
}




//===========================================================================
/*!
     Set the haptic frictionfor this mesh.

     \fn        void cMesh::setFriction(double a_staticFriction, double a_dynamicFriction, const bool a_affectChildren=0)
     \param     a_staticFriction The static friction to apply to this object
     \param     a_dynamicFriction The dynamic friction to apply to this object
     \param     a_affectChildren  If \b true, then children are also modified.
     
*/
//===========================================================================
void cMesh::setFriction(double a_staticFriction, double a_dynamicFriction, const bool a_affectChildren)
{

  m_material.setStaticFriction(a_staticFriction);
  m_material.setDynamicFriction(a_dynamicFriction);

  // update changes to children
  if (a_affectChildren)
  {
      for (unsigned int i=0; i<m_children.size(); i++)
      {
          cGenericObject *nextObject = m_children[i];

          if (typeid(*nextObject) == typeid(cMesh))
          {
              cMesh *nextMesh = (cMesh*)nextObject;
              nextMesh->setFriction(a_staticFriction,a_dynamicFriction,a_affectChildren);
          }
      }
  }
}


//===========================================================================
/*!
     Set the haptic stiffness for this mesh.

     \fn        void cMesh::setStiffness(double a_stiffness, const bool a_affectChildren=0);
     \param     a_stiffness  The stiffness to apply to this object
     \param     a_affectChildren  If \b true, then children are also modified.
     
*/
//===========================================================================
void cMesh::setStiffness(double a_stiffness, const bool a_affectChildren)
{

  m_material.setStiffness(a_stiffness);

  // update changes to children
  if (a_affectChildren)
  {
      for (unsigned int i=0; i<m_children.size(); i++)
      {
          cGenericObject *nextObject = m_children[i];

          if (typeid(*nextObject) == typeid(cMesh))
          {
              cMesh *nextMesh = (cMesh*)nextObject;
              nextMesh->setStiffness(a_stiffness, a_affectChildren);
          }
      }
  }
}


//===========================================================================
/*!
     Set the current texture for this mesh.

     \fn        void cMesh::setTexture(cTexture2D* a_texture,
                const bool a_affectChildren)                
     \param     a_texture  The texture to apply to this object
     \param     a_affectChildren  If \b true, then children are also modified.

     Note that this does not affect whether texturing is enabled; it sets
     the texture that will be rendered _if_ texturing is enabled.  Call
     useTexture to enable / disable texturing.
*/
//===========================================================================
void cMesh::setTexture(cTexture2D* a_texture, const bool a_affectChildren)
{

  m_texture = a_texture;

  // update changes to children
  if (a_affectChildren)
  {
      for (unsigned int i=0; i<m_children.size(); i++)
      {
          cGenericObject *nextObject = m_children[i];

          if (typeid(*nextObject) == typeid(cMesh))
          {
              cMesh *nextMesh = (cMesh*)nextObject;
              nextMesh->setTexture(a_texture, a_affectChildren);
          }
      }
  }
}


//===========================================================================
/*!
     Set the current material for this mesh.

     \fn        void cMesh::setMaterial(cMaterial& a_mat,
                const bool a_affectChildren)                
     \param     a_mat The material to apply to this object
     \param     a_affectChildren  If \b true, then children are also modified.

     Note that this does not affect whether material rendering is enabled;
     it sets the maetrial that will be rendered _if_ material rendering is
     enabled.  Call useMaterial to enable / disable material rendering.
*/
//===========================================================================
void cMesh::setMaterial(cMaterial& a_mat, const bool a_affectChildren)
{

  m_material = a_mat;

  // update changes to children
  if (a_affectChildren)
  {
      for (unsigned int i=0; i<m_children.size(); i++)
      {
          cGenericObject *nextObject = m_children[i];

          if (typeid(*nextObject) == typeid(cMesh))
          {
              cMesh *nextMesh = (cMesh*)nextObject;
              nextMesh->setMaterial(a_mat, a_affectChildren);
          }
      }
  }
}

//===========================================================================
/*!
     Use texture mapping if texture is defined

     \fn        void cMesh::useTexture(const bool a_useTexture,
                const bool a_affectChildren)
     \param     a_useTexture If \b true, then texture mapping is used.
     \param     a_affectChildren  If \b true, then children are also modified.
*/
//===========================================================================
void cMesh::useTexture(const bool a_useTexture, const bool a_affectChildren)
{
    // update changes to object
    m_useTextureMapping = a_useTexture;

    // update changes to children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            if (typeid(*nextObject) == typeid(cMesh))
            {
                cMesh *nextMesh = (cMesh*)nextObject;
                nextMesh->useTexture(a_useTexture, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Define the way normals are graphically rendered.

     \fn        void cMesh::setNormalsProperties(const double a_length,
                const cColorf& a_color, const bool a_affectChildren)
     \param     a_length  Length of normals.
     \param     a_color  Color of normals.
     \param     a_affectChildren  If \b true, then children also modified.
*/
//===========================================================================
void cMesh::setNormalsProperties(const double a_length, const cColorf& a_color,
                                 const bool a_affectChildren)
{
    // update changes to object
    m_showNormalsLength = a_length;
    m_showNormalsColor = a_color;

    // update changes to children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            if (typeid(*nextObject) == typeid(cMesh))
            {
                cMesh *nextMesh = (cMesh*)nextObject;
                nextMesh->setNormalsProperties(a_length, a_color, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Show normals.

     \fn        void cMesh::showNormals(const bool a_showNormals,
                const bool a_affectChildren)
     \param     a_showNormals If \b true, small normals are rendered graphicaly.
     \param     a_affectChildren  If \b true, then children also modified.
*/
//===========================================================================
void cMesh::showNormals(const bool a_showNormals, const bool a_affectChildren)
{
    // update changes to object
    m_showNormals = a_showNormals;

    // apply changes to children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            if (typeid(*nextObject) == typeid(cMesh))
            {
                cMesh *nextMesh = (cMesh*)nextObject;
                nextMesh->showNormals(a_showNormals, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Compute boudary box by parsing all vertices indexed by a triangle

     \fn       void cMesh::updateBoundaryBox()
*/
//===========================================================================
void cMesh::updateBoundaryBox()
{
    double xMin = CHAI_LARGE;
    double yMin = CHAI_LARGE;
    double zMin = CHAI_LARGE;
    double xMax = -CHAI_LARGE;
    double yMax = -CHAI_LARGE;
    double zMax = -CHAI_LARGE;;

    for(unsigned int i=0; i<m_triangles.size(); i++)
    {
        // get next triangle
        cTriangle* nextTriangle = &m_triangles[i];

        if (nextTriangle->m_allocated)
        {
            cVector3d tVertex0 = m_vertices[nextTriangle->m_indexVertex0].m_localPos;
            xMin = cMin(tVertex0.x, xMin);
            yMin = cMin(tVertex0.y, yMin);
            zMin = cMin(tVertex0.z, zMin);
            xMax = cMax(tVertex0.x, xMax);
            yMax = cMax(tVertex0.y, yMax);
            zMax = cMax(tVertex0.z, zMax);

            cVector3d tVertex1 = m_vertices[nextTriangle->m_indexVertex1].m_localPos;
            xMin = cMin(tVertex1.x, xMin);
            yMin = cMin(tVertex1.y, yMin);
            zMin = cMin(tVertex1.z, zMin);
            xMax = cMax(tVertex1.x, xMax);
            yMax = cMax(tVertex1.y, yMax);
            zMax = cMax(tVertex1.z, zMax);

            cVector3d tVertex2 = m_vertices[nextTriangle->m_indexVertex2].m_localPos;
            xMin = cMin(tVertex2.x, xMin);
            yMin = cMin(tVertex2.y, yMin);
            zMin = cMin(tVertex2.z, zMin);
            xMax = cMax(tVertex2.x, xMax);
            yMax = cMax(tVertex2.y, yMax);
            zMax = cMax(tVertex2.z, zMax);
        }
    }


    if (m_triangles.size() > 0)
    {
        m_boundaryBoxMin.set(xMin, yMin, zMin);
        m_boundaryBoxMax.set(xMax, yMax, zMax);
    }
    else
    {
        m_boundaryBoxMin.zero();
        m_boundaryBoxMax.zero();
    }
}


//===========================================================================
/*!
     Resize vertices of current mesh

     \fn        void scaleObject(double a_scaleFactor)
     \param     a_scaleFactor   Scale factor.
*/
//===========================================================================
void cMesh::scaleObject(double a_scaleFactor)
{
    unsigned int i, numItems;
    numItems = m_vertices.size();
    for(i=0; i<numItems; i++)
    {
        m_vertices[i].m_localPos.mul(a_scaleFactor);
    }
}


//===========================================================================
/*!
     Sorting functions to help in findNeighbors method.

     \fn       bool TriangleSort(cTrianlge* t1, cTrianlge* t2)
     \param    t1  First triangle.
     \param    t2  Second triangle.
     \return   Return whether the first triangle is "less than" the second.
*/
//===========================================================================
bool TriangleSort0(cTriangle* t1, cTriangle* t2)
{
    return (t1->getVertex0()->getPos().x < t2->getVertex0()->getPos().x);
}
bool TriangleSort1(cTriangle* t1, cTriangle* t2)
{
    return (t1->getVertex1()->getPos().x < t2->getVertex1()->getPos().x);
}
bool TriangleSort2(cTriangle* t1, cTriangle* t2)
{
    return (t1->getVertex2()->getPos().x < t2->getVertex2()->getPos().x);
}


//===========================================================================
/*!
     Find pairs of neighboring triangles that share specific vertices,
     given vectors of those triangles sorted by those vertices (first, second,
     or third), using a merge join.

     \fn       void findNeighbors(std::vector<cTriangle*>* search1,
                            std::vector<cTriangle*>* search2, int v1, int v2)
     \param    search1   First vector of triangles, sorted by some vertex.
     \param    search2   Second vector of triangles, sorted by some vertex.
     \param    v1        Which vertex first vector is sorted by (0, 1, or 2).
     \param    v2        Which vertex second vector is sorted by (0, 1, or 2).
*/
//===========================================================================
void cMesh::findNeighbors(std::vector<cTriangle*>* search1,
                             std::vector<cTriangle*>* search2, int v1, int v2)
{
    unsigned int i = 0;
    unsigned int j = 0;

    while ((i < search1->size()) && (j < search2->size()))
    {
      if (fabs( (*search1)[i]->getVertex(v1)->getPos().x -
                (*search2)[j]->getVertex(v2)->getPos().x) < 0.0000001)
      {
        while ((i<search1->size()) &&
               (fabs( (*search1)[i]->getVertex(v1)->getPos().x -
                      (*search2)[j]->getVertex(v2)->getPos().x) < 0.0000001))
        {
          unsigned int jj = j;
          while ((jj<search2->size()) &&
                 (fabs( (*search1)[i]->getVertex(v1)->getPos().x -
                        (*search2)[jj]->getVertex(v2)->getPos().x) < 0.0000001))
          {
            if (((*search1)[i] != (*search2)[jj]) &&
                (cEqualPoints((*search1)[i]->getVertex(v1)->getPos(),
                             (*search2)[jj]->getVertex(v2)->getPos())))
            {
              bool found = false;
              unsigned int ii;

              for (ii=0; (!found) &&
                                   (ii<(*search1)[i]->m_neighbors->size()); ii++)
                if ((*((*search1)[i]->m_neighbors))[ii] == (*search2)[jj])
                  found = true;
              if (!found)
                (*search1)[i]->m_neighbors->push_back((*search2)[jj]);
              found = false;
              for (ii=0; (!found) &&
                            (ii<(*search2)[jj]->m_neighbors->size()); ii++)
                if ((*((*search2)[jj]->m_neighbors))[ii] == (*search1)[i])
                  found = true;
              if (!found)
                (*search2)[jj]->m_neighbors->push_back((*search1)[i]);
            }
            jj++;
          }
          i++;
        }
      }
      else if ( (*search1)[i]->getVertex(v1)->getPos().x >
                (*search2)[j]->getVertex(v2)->getPos().x )
        j++;
      else if ( (*search1)[i]->getVertex(v1)->getPos().x <
                (*search2)[j]->getVertex(v2)->getPos().x )
        i++;
    }
}


//===========================================================================
/*!
     Setup an AABB collision detector to current mesh and children

     \fn       void cMesh::createAABBCollisionDetector(bool a_affectChildren,
                                                       bool a_useNeighbors)
     \param    a_affectChildren   Create collision detectors for children?
     \param    a_useNeighbors     Create neighbor lists?
*/
//===========================================================================
void cMesh::createAABBCollisionDetector(bool a_affectChildren,
                                        bool a_useNeighbors)
{
    // delete previous collision detector
    if (m_collisionDetector != NULL)
    {
        delete m_collisionDetector;
    }

    // create AABB collision detector
    cCollisionAABB* collisionDetectorAABB =
                         new cCollisionAABB(pTriangles(), a_useNeighbors);
    collisionDetectorAABB->initialize();
    m_collisionDetector = collisionDetectorAABB;

    // create neighbor lists
    if (a_useNeighbors)
    {
      std::vector<cTriangle*>* sort0 = new std::vector<cTriangle*>;
      unsigned int i;
      for (i=0; i<m_triangles.size(); i++)
        sort0->push_back(&(m_triangles[i]));
      std::vector<cTriangle*>* sort1 = new std::vector<cTriangle*>;
      for (i=0; i<m_triangles.size(); i++)
        sort1->push_back(&(m_triangles[i]));
      std::vector<cTriangle*>* sort2 = new std::vector<cTriangle*>;
      for (i=0; i<m_triangles.size(); i++)
        sort2->push_back(&(m_triangles[i]));
      std::sort(sort0->begin(), sort0->end(), TriangleSort0);
      std::sort(sort1->begin(), sort1->end(), TriangleSort1);
      std::sort(sort2->begin(), sort2->end(), TriangleSort2);
      for (i=0; i<m_triangles.size(); i++)
      {
        m_triangles[i].m_neighbors = new std::vector<cTriangle*>;
        m_triangles[i].m_neighbors->push_back(&(m_triangles[i]));
      }
      findNeighbors(sort0, sort0, 0, 0);
      findNeighbors(sort0, sort1, 0, 1);
      findNeighbors(sort0, sort2, 0, 2);
      findNeighbors(sort1, sort1, 1, 1);
      findNeighbors(sort1, sort2, 1, 2);
      findNeighbors(sort2, sort2, 2, 2);
      delete sort0;
      delete sort1;
      delete sort2;
    }

    // update children if required
    if (a_affectChildren)
    {
        unsigned int i;
        for (i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            if (typeid(*nextObject) == typeid(cMesh))
            {
                cMesh *nextMesh = (cMesh*)nextObject;
                nextMesh->createAABBCollisionDetector(a_affectChildren,
                                                      a_useNeighbors);
            }
        }
    }
}


//===========================================================================
/*!
     Setup a sphere tree collision detector to current mesh and children

     \fn     void cMesh::createSphereTreeCollisionDetector(bool a_affectChildren,
                                                           bool a_useNeighbors)
     \param  a_affectChildren   Create collision detectors for children?
     \param  a_useNeighbors     Create neighbor lists?
*/
//===========================================================================
void cMesh::createSphereTreeCollisionDetector(bool a_affectChildren,
                                              bool a_useNeighbors)
{
    // delete previous collision detector
    if (m_collisionDetector != NULL)
    {
      delete m_collisionDetector;
    }

    // create sphere tree collision detector
    cCollisionSpheres* collisionDetectorSphereTree =
                           new cCollisionSpheres(pTriangles(), a_useNeighbors);
    collisionDetectorSphereTree->initialize();
    m_collisionDetector = collisionDetectorSphereTree;

    // create list of neighbors
    if (a_useNeighbors)
    {
      unsigned int i;
      std::vector<cTriangle*>* sort0 = new std::vector<cTriangle*>;
      for (i=0; i<m_triangles.size(); i++)
        sort0->push_back(&(m_triangles[i]));
      std::vector<cTriangle*>* sort1 = new std::vector<cTriangle*>;
      for (i=0; i<m_triangles.size(); i++)
        sort1->push_back(&(m_triangles[i]));
      std::vector<cTriangle*>* sort2 = new std::vector<cTriangle*>;
      for (i=0; i<m_triangles.size(); i++)
        sort2->push_back(&(m_triangles[i]));
      std::sort(sort0->begin(), sort0->end(), TriangleSort0);
      std::sort(sort1->begin(), sort1->end(), TriangleSort1);
      std::sort(sort2->begin(), sort2->end(), TriangleSort2);
      for (i=0; i<m_triangles.size(); i++)
      {
        m_triangles[i].m_neighbors = new std::vector<cTriangle*>;
        m_triangles[i].m_neighbors->push_back(&(m_triangles[i]));
      }
      findNeighbors(sort0, sort0, 0, 0);
      findNeighbors(sort0, sort1, 0, 1);
      findNeighbors(sort0, sort2, 0, 2);
      findNeighbors(sort1, sort1, 1, 1);
      findNeighbors(sort1, sort2, 1, 2);
      findNeighbors(sort2, sort2, 2, 2);
      delete sort0;
      delete sort1;
      delete sort2;
    }

    // update children if required
    if (a_affectChildren)
    {
      for (unsigned int i=0; i<m_children.size(); i++)
      {
        cGenericObject *nextObject = m_children[i];
        if (typeid(*nextObject) == typeid(cMesh))
        {
          cMesh *nextMesh = (cMesh*)nextObject;
          nextMesh->createSphereTreeCollisionDetector(a_affectChildren,
                                                      a_useNeighbors);
        }
      }
    }
}


//===========================================================================
/*!
     Render the solid in OpenGL. Do not call direclty.
     \fn       void cMesh::render(const int a_renderMode)
     \param    a_renderMode  Rendering mode.
*/
//===========================================================================
void cMesh::render(const int a_renderMode)
{
    // render triangle mesh
    if (m_useTransparency)
    {
        // render transparent front triangles
        if (a_renderMode & CHAI_RENDER_TRANSPARENT_FRONT)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            renderMesh(a_renderMode);
        }

        // render transparent back triangles
        if (a_renderMode & CHAI_RENDER_TRANSPARENT_BACK)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            renderMesh(a_renderMode);
        }
    }
    else
    {
         if (a_renderMode & CHAI_RENDER_NON_TRANSPARENT)
         {
            // render non transparent mesh
            glDisable(GL_CULL_FACE);
            renderMesh(a_renderMode);
         }
    }


    if (a_renderMode & CHAI_RENDER_NON_TRANSPARENT)
    {
        // render normals
        if (m_showNormals)
        {
            renderNormals();
        }
    }
}


//===========================================================================
/*!
     Render a graphic representation of each normal of the mesh.

     \fn       void cMesh::renderNormals()
*/
//===========================================================================
void cMesh::renderNormals()
{
    // disable lighting
    glDisable(GL_LIGHTING);

    // set line width
    glLineWidth(1.0);

    // set color
    glColor4fv( (const float *)&m_showNormalsColor);

    // render vertex normals
    for (unsigned int i=0; i<m_triangles.size(); i++)
    {
        cTriangle* nextTriangle = &m_triangles[i];
        cVector3d* vertex0 = &m_vertices[nextTriangle->m_indexVertex0].m_localPos;
        cVector3d* vertex1 = &m_vertices[nextTriangle->m_indexVertex1].m_localPos;
        cVector3d* vertex2 = &m_vertices[nextTriangle->m_indexVertex2].m_localPos;
        cVector3d* normal0 = &m_vertices[nextTriangle->m_indexVertex0].m_normal;
        cVector3d* normal1 = &m_vertices[nextTriangle->m_indexVertex1].m_normal;
        cVector3d* normal2 = &m_vertices[nextTriangle->m_indexVertex2].m_normal;
        cVector3d normalPos, normal;

        // render normal 0 of triangle
        glBegin(GL_LINES);
            glVertex3dv((const double *)vertex0);
            normal0->mulr(m_showNormalsLength, normal);
            vertex0->addr(normal, normalPos);
            glVertex3dv((const double *)&normalPos);
        glEnd();

        // render normal 1 of triangle
        glBegin(GL_LINES);
            glVertex3dv((const double *)vertex1);
            normal1->mulr(m_showNormalsLength, normal);
            vertex1->addr(normal, normalPos);
            glVertex3dv((const double *)&normalPos);
        glEnd();

        // render normal 2 of triangle
        glBegin(GL_LINES);
            glVertex3dv((const double *)vertex2);
            normal2->mulr(m_showNormalsLength, normal);
            vertex2->addr(normal, normalPos);
            glVertex3dv((const double *)&normalPos);
        glEnd();
    }

    // enable lighting
    glEnable(GL_LIGHTING);
}


//===========================================================================
/*!
     Render all the triangles, material and texture properties of the mesh

     \fn       void cMesh::renderMesh(const int a_renderMode)
*/
//===========================================================================
void cMesh::renderMesh(const int a_renderMode)
{
    // initialize arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);
    glDisableClientState(GL_EDGE_FLAG_ARRAY);

    // specify pointer to arrays
    glVertexPointer(3, GL_DOUBLE, sizeof(cVertex), &m_vertices[0].m_localPos);
    glNormalPointer(GL_DOUBLE, sizeof(cVertex), &m_vertices[0].m_normal);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(cVertex), m_vertices[0].m_color.pColor());
    glTexCoordPointer(2, GL_DOUBLE, sizeof(cVertex), &m_vertices[0].m_texCoord);

    // set polygon and face mode
    glPolygonMode(GL_FRONT_AND_BACK, m_triangleMode);

    // check transparency level
    if (m_useTransparency)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
    }

    // transparency is not used
    else
    {
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }

    // enable lighting
    glEnable(GL_LIGHTING);

    // if material property exists, render it
    if (m_useMaterialProperty)
    {
        m_material.render();
    }
    else
    {
        glColor3f(1.0, 1.0, 1.0);
    }

    // if texture property exists, render it
    if ((m_texture != NULL) && m_useTextureMapping)
    {
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        m_texture->render();
    }

    // should we use vertex colors?
    if (m_useVertexColors & m_useMaterialProperty)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    }
    else
    {
        glDisable(GL_COLOR_MATERIAL);
    }

    // render all active triangles
    glBegin(GL_TRIANGLES);
    unsigned int i;
    unsigned int numItems = m_triangles.size();
    for(i=0; i<numItems; i++)
    {
        bool allocated = m_triangles[i].m_allocated;
        if (allocated)
        {
          unsigned int index0 = m_triangles[i].m_indexVertex0;
          unsigned int index1 = m_triangles[i].m_indexVertex1;
          unsigned int index2 = m_triangles[i].m_indexVertex2;
          glArrayElement(index0);
          glArrayElement(index1);
          glArrayElement(index2);
        }
    }
    glEnd();

    // restore OpenGL setings
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}


