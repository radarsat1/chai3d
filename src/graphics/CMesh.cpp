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

#include "CMesh.h"
#include "CVertex.h"
#include "CTriangle.h"
#include "CMeshLoader.h"
#include "CCollisionBrute.h"
#include "CCollisionAABB.h"
#include "CCollisionSpheres.h"
#include <algorithm>

#ifdef _MSVC
#include <conio.h>
#endif

//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cMesh

    \fn       cMesh::cMesh(cWorld* a_world)
    \param    a_parent  Pointer to parent world.
*/
//===========================================================================
cMesh::cMesh(cWorld* a_world)
{
    // initialize parent object of mesh. Not yet a child on an other object.
    m_parent = NULL;

    // set parent world of mesh
    m_parentWorld = a_world;

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

    // turn culling on by default
    m_cullingEnabled = true;

    // by default, if transparency is enabled, use the multi-pass approach
    m_useMultipassTransparency = true;

    // Display lists disabled by default
    m_useDisplayList = false;
    m_displayList = -1;
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

    // Delete any allocated display lists
    if (m_displayList != -1)
      glDeleteLists(m_displayList,1);
    
}


//===========================================================================
/*!
     Returns the specified triangle... if a_includeChildren is false, I just
     index into my triangle array (no boundary checking, since this is called
     often).
     
     If a_includeChildren is true, I start counting through my own triangle
     array, then each of my children... in the process, I'm going to call
     getNumTriangles(true) on each of my children, so this is a recursive
     and unbounded (though generally fast) version of this method.

     \fn        cTriangle* cMesh::getTriangle(unsigned int a_index, bool a_includeChildren = false);
     \param     a_index            The index of the requested triangle
     \param     a_includeChildren  If \b true, then children are also included.
*/
//===========================================================================
cTriangle* cMesh::getTriangle(unsigned int a_index, bool a_includeChildren)
{

    // The easy case...
    if (a_includeChildren == false) return &(m_triangles[a_index]);

    // Now we have to possibly search through children...
    
    // First do a sanity check to make sure this is a reasonable
    // triangle...
    if (a_index >= getNumTriangles(true)) return 0;

    // Get number of triangles of current object
    unsigned int numTriangles = m_triangles.size();

    if (a_index < numTriangles) return &(m_triangles[a_index]);

    // Okay, this triangle must live in a child mesh... subtract
    // my own triangles from the number we have to search through
    a_index -= numTriangles;

    unsigned int i, numChildren;
    numChildren = m_children.size();
    for (i=0; i<numChildren; i++)
    {
        cGenericObject *nextObject = m_children[i];

        // check if nextObject is a mesh.
        cMesh* nextMesh = dynamic_cast<cMesh*>(nextObject);            
        if (nextMesh)
        {
             // How many triangles does he have?
             numTriangles = nextMesh->getNumTriangles(true);

             // If this index lives in _this_ child...
             if (a_index < numTriangles)
             {
               return nextMesh->getTriangle(a_index,true);
             }

             // Otherwise keep going...
             else {
               a_index -= numTriangles;
             }

        } // ...if this child was a mesh
    } // ...for each child

    // We didn't find this triangle... this should never happen, since we
    // sanity-checked at the beginning of this method...
    return 0;
}


//===========================================================================
/*!
     Returns the specified vertex... if a_includeChildren is false, I just
     index into my vertex array (no boundary checking, since this is called
     often).
     
     If a_includeChildren is true, I start counting through my own vertex
     array, then each of my children... in the process, I'm going to call
     getNumVertices(true) on each of my children, so this is a recursive
     and unbounded (though generally fast) version of this method.

     \fn        cVertex* cMesh::getVertex(unsigned int a_index, bool a_includeChildren = false);
     \param     a_index            The index of the requested vertex
     \param     a_includeChildren  If \b true, then children are also included.
*/
//===========================================================================
cVertex* cMesh::getVertex(unsigned int a_index, bool a_includeChildren)
{

    // The easy case...
    if (a_includeChildren == false) return &(m_vertices[a_index]);

    // Now we have to possibly search through children...
    
    // First do a sanity check to make sure this is a reasonable
    // vertex...
    if (a_index >= getNumVertices(true)) return 0;

    // Get number of vertices of current object
    unsigned int numVertices = m_vertices.size();

    if (a_index < numVertices) return &(m_vertices[a_index]);

    // Okay, this vertex must live in a child mesh... subtract
    // my own vertices from the number we have to search through
    a_index -= numVertices;

    unsigned int i, numChildren;
    numChildren = m_children.size();
    for (i=0; i<numChildren; i++)
    {
        cGenericObject *nextObject = m_children[i];

        // check if nextObject is a mesh.
        cMesh* nextMesh = dynamic_cast<cMesh*>(nextObject);            
        if (nextMesh)
        {
             // How many vertices does he have?
             numVertices = nextMesh->getNumVertices(true);

             // If this index lives in _this_ child...
             if (a_index < numVertices)
             {
               return nextMesh->getVertex(a_index,true);
             }

             // Otherwise keep going...
             else {
               a_index -= numVertices;
             }

        } // ...if this child was a mesh
    } // ...for each child

    // We didn't find this vertex... this should never happen, since we
    // sanity-checked at the beginning of this method...
    return 0;
}


//===========================================================================
/*!
     Returns the number of vertices contained in this mesh, optionally
     including its children.

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

            // check if nextObject is a mesh.
            cMesh* nextMesh = dynamic_cast<cMesh*>(nextObject);            
            if (nextMesh)
            {
                numVertices = numVertices + nextMesh->getNumVertices(a_includeChildren);
            }
        }
    }

    // return result
    return (numVertices);
}


//===========================================================================
/*!
     Enables or disables backface culling (rendering in GL is much faster
     with culling on)

     \fn       void cMesh::useCulling(const bool iUseCulling, const bool a_affectChildren)
     \param    a_useCulling  If \b true, backfaces are culled
     \param    a_affectChildren  If \b true, this operation is propagated to my children
*/
//===========================================================================
void cMesh::useCulling(const bool a_useCulling, const bool a_affectChildren)
{
    // apply changes to this object
    m_cullingEnabled = a_useCulling;
    
    // propagate changes to children
    if (a_affectChildren)
    {
        unsigned int i, numItems;
        numItems = m_children.size();
        for (i=0; i<numItems; i++)
        {
            cGenericObject *nextObject = m_children[i];

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->useCulling(a_useCulling, a_affectChildren);
            }
        }
    }
}



//===========================================================================
/*!
     This enables the use of display lists for mesh rendering.  This should
     significantly speed up rendering for large meshes, but it means that
     any changes you make to any cMesh options or any vertex positions
     will not take effect until you invalidate the existing display list
     (by calling invalidateDisplayList()).

     In general, if you aren't having problems with rendering performance,
     don't bother with this; you don't want to worry about having to 
     invalidate display lists every time you change a tiny option.

     \fn       void cMesh::useDisplayList(const bool a_useDisplayList, const bool a_affectChildren)
     \param    a_useDisplayList  If \b true, this mesh will be rendered with a display list
     \param    a_affectChildren  If \b true, then children also modified.
*/
//===========================================================================
void cMesh::useDisplayList(const bool a_useDisplayList, const bool a_affectChildren)
{
    // update changes to object
    m_useDisplayList = a_useDisplayList;
    
    // propagate changes to children
    if (a_affectChildren)
    {
        unsigned int i, numItems;
        numItems = m_children.size();
        for (i=0; i<numItems; i++)
        {
            cGenericObject *nextObject = m_children[i];

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->useDisplayList(a_useDisplayList, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Returns the number of triangles contained in this mesh, optionally
     including its children.

     \fn        unsigned int cMesh::getNumTriangles(bool a_includeChildren) const
     \param     a_affectChildren  If \b true, then children are also included.
*/
//===========================================================================
unsigned int cMesh::getNumTriangles(bool a_includeChildren) const
{
    // get the number of triangle of current object
    unsigned int numTriangles = m_triangles.size();

    // optionally count the number of triangles in my children
    if (a_includeChildren)
    {
        unsigned int i, numItems;
        numItems = m_children.size();
        for (i=0; i<numItems; i++)
        {
            cGenericObject *nextObject = m_children[i];

            // check if nextobject is a mesh.
            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                numTriangles += nextMesh->getNumTriangles(a_includeChildren);
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

    // return the index at which I inserted this vertex in my vertex array
    return index;
}


//===========================================================================
/*!
    Remove the vertex at the specified position in my vertex array

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
    Create a new triangle and three new vertices by passing vertex indices

    \fn         unsigned int cMesh::newTriangle(const unsigned int a_indexVertex0,
                const unsigned int a_indexVertex1,
                const unsigned int a_indexVertex2)
    \param      a_indexVertex0   index position of vertex 0.
    \param      a_indexVertex1   index position of vertex 1.
    \param      a_indexVertex2   index position of vertex 2.
    \return     Return the index of the new triangle in my triangle array
*/
//===========================================================================
unsigned int cMesh::newTriangle(const unsigned int a_indexVertex0, const unsigned int a_indexVertex1,
             const unsigned int a_indexVertex2)
{
    unsigned int index;

    // check if there is an available slot on the free triangle list
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

    // return the index at which I inserted this triangle in my triangle array
    return (index);
}


//===========================================================================
/*!
     Create a new triangle and three new vertices by passing vertex positions

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
     Load a 3D mesh file.  CHAI currently supports .obj and .3ds files.

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
     Compute surface normals for every vertex in the mesh, by averaging
     the face normals of the triangle that include each vertex.
     
     \fn       void cMesh::computeAllNormals()
     \param    a_affectChildren  If \b true, then children are also updated.
*/
//===========================================================================
void cMesh::computeAllNormals(const bool a_affectChildren)
{
    unsigned int i;

    // set all normals to zero
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

    // optionally propagate changes to children
    if (a_affectChildren)
    {
        unsigned int i, numItems;
        numItems = m_children.size();
        for (i=0; i<numItems; i++)
        {
            cGenericObject *nextObject = m_children[i];

            // check if nextobject is a mesh. if yes, apply changes
            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->computeAllNormals(a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Compute the global position of all vertices

     \fn       void cMesh::updateGlobalPositions(const bool a_frameOnly)
     \param    a_frameOnly  If \b false, the global position of all vertices
               is computed, otherwise this function does nothing.
*/
//===========================================================================
void cMesh::updateGlobalPositions(const bool a_frameOnly)
{
    if (a_frameOnly) return;

    unsigned int i,numVertices;
    numVertices = m_vertices.size();
    for (i=0; i<numVertices; i++)
    {
        m_vertices[i].computeGlobalPosition(m_globalPos,m_globalRot);
    }
    
}


//===========================================================================
/*!
     Enable or disable wireframe rendering, optionally propagating the
     operation to my children

     \fn        void cMesh::setWireMode(const bool a_showWireMode,
                const bool a_affectChildren)
     \param     a_showWireMode  If \b true, wireframe mode is used.
     \param     a_affectChildren  If \b true, then children are also updated
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
            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->setWireMode(a_showWireMode, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Set the alpha value at each vertex, in all of my material colors, 
     optionally propagating the operation to my children.

     Using the 'apply to textures' option causes the actual texture
     alpha values to be over-written in my texture, if it exists.

     [Editor's note: the 'apply to textures' option is not currently
     enabled, since (a) it's a silly way to control transparency
     and (b) not all textures have an alpha channel.]

     \fn        void cMesh::setTransparencyLevel(const float a_level,
     \param     const bool a_applyToTextures,, const bool a_affectChildren)
     \param     a_level  Level of transparency ranging from 0.0 to 1.0.
     \param     a_applyToTextures  If \b true, then apply changes to texture
     \param     a_affectChildren  If true, then children also modified.
*/
//===========================================================================
void cMesh::setTransparencyLevel(const float a_level, const bool a_applyToTextures,
                                 const bool a_affectChildren)
{
    
    // apply new value to material
    m_material.setTransparencyLevel(a_level);

    // convert transparency level to cColorb format
    GLubyte level = GLubyte(255.0f * a_level);

    // apply the new value to all vertex colors
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

    // propagate the operation to my children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            // check if nextobject is a mesh. if yes, apply changes.
            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
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

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->setVertexColor(a_color, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Enable or disable the use of per-vertex color information of when rendering
     the mesh.

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

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->useColors(a_useColors, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Specify whether transparency should be enabled.  Note that this
     does not affect the transparency _mode_, which controls the use
     of simple transparency vs. multipass transparency.  

     \fn       void cMesh::enableTransparency(const bool a_useTransparency,
                 const bool a_affectChildren)
     \param    a_useTransparency   If \b true, transparency is enabled
     \param    a_affectChildren    If \b true, then children are also modified.
*/
//===========================================================================
void cMesh::enableTransparency(const bool a_useTransparency,
                                           const bool a_affectChildren)
{
    // update changes to object
    m_useTransparency = a_useTransparency;

    // propagate changes to my children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->enableTransparency(a_useTransparency, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Specify whether transparent rendering should use two passes (looks better)
     or one pass (runs faster).  See cCamera for more information about
     multipass transparency.

     \fn       void cMesh::setTransparencyRenderMode(const bool a_useMultipassTransparency,
                 const bool a_affectChildren)
     \param    a_useMultipassTransparency   If \b true, this mesh uses multipass rendering
     \param    a_affectChildren  If \b true, then children are also modified.
*/
//===========================================================================
void cMesh::setTransparencyRenderMode(const bool a_useMultipassTransparency,
                                           const bool a_affectChildren)
{
    // update changes to object
    m_useMultipassTransparency = a_useMultipassTransparency;

    // propagate changes to my children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->setTransparencyRenderMode(a_useMultipassTransparency, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Enable or disable the use of material properties.

     \fn        void cMesh::useMaterial(const bool a_useMaterial,
                const bool a_affectChildren)
     \param     a_useMaterial If \b true, then material properties are used for rendering.
     \param     a_affectChildren  If \b true, then children are also modified.
*/
//===========================================================================
void cMesh::useMaterial(const bool a_useMaterial, const bool a_affectChildren)
{
    // update changes to object
    m_useMaterialProperty = a_useMaterial;

    // propagate changes to my children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {                
                nextMesh->useMaterial(a_useMaterial, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
Extrude each vertex of the mesh by some amount along its normal

\fn        void cMesh::extrude(double a_extrudeDistance, bool a_affectChildren=0);
\param     a_extrudeDistance Distance to move each vertex
\param     a_affectChildren  If \b true, children are also modified.
\param     a_updateCollisionDetector  If \b true, this mesh's collision detector is
           re-initialized
*/
//===========================================================================
void cMesh::extrude(const double a_extrudeDistance, const bool a_affectChildren,
                    const bool a_updateCollisionDetector)
{
  // update changes to object
  int vertexcount = m_vertices.size();
  for(int i=0; i<vertexcount; i++)
  {
    m_vertices[i].m_localPos.add(cMul(a_extrudeDistance,m_vertices[i].m_normal));
  }  

  // This is an O(N) operation, as is the extrusion, so it seems okay to call
  // this by default...
  updateBoundaryBox();

  // propagate changes to my children
  if (a_affectChildren)
  {
    for (unsigned int i=0; i<m_children.size(); i++)
    {
      cGenericObject *nextObject = m_children[i];

      cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
      if (nextMesh)
      {                
        nextMesh->extrude(a_extrudeDistance, a_affectChildren);
      }
    }
  }

  if (a_updateCollisionDetector && m_collisionDetector)
    m_collisionDetector->initialize();
}



//===========================================================================
/*!
     Set the static and dynamic friction for this mesh, possibly recursively affecting children

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

  // propagate changes to children
  if (a_affectChildren)
  {
      for (unsigned int i=0; i<m_children.size(); i++)
      {
          cGenericObject *nextObject = m_children[i];

          cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
          if (nextMesh)
          {
              nextMesh->setFriction(a_staticFriction,a_dynamicFriction,a_affectChildren);
          }
      }
  }
}


//===========================================================================
/*!
     Set the haptic stiffness for this mesh, possibly recursively affecting children

     \fn        void cMesh::setStiffness(double a_stiffness, const bool a_affectChildren=0);
     \param     a_stiffness  The stiffness to apply to this object
     \param     a_affectChildren  If \b true, then children are also modified.
     
*/
//===========================================================================
void cMesh::setStiffness(double a_stiffness, const bool a_affectChildren)
{

  m_material.setStiffness(a_stiffness);

  // propagate changes to children
  if (a_affectChildren)
  {
      for (unsigned int i=0; i<m_children.size(); i++)
      {
          cGenericObject *nextObject = m_children[i];

          cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
          if (nextMesh)
          {
              nextMesh->setStiffness(a_stiffness, a_affectChildren);
          }
      }
  }
}


//===========================================================================
/*!
     Set the current texture for this mesh, possibly recursively affecting children

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

  // propagate changes to children
  if (a_affectChildren)
  {
      for (unsigned int i=0; i<m_children.size(); i++)
      {
          cGenericObject *nextObject = m_children[i];

          cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
          if (nextMesh)
          {
              nextMesh->setTexture(a_texture, a_affectChildren);
          }
      }
  }
}


//===========================================================================
/*!
     Set the current material for this mesh, possibly recursively affecting children

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

  // propagate changes to children
  if (a_affectChildren)
  {
      for (unsigned int i=0; i<m_children.size(); i++)
      {
          cGenericObject *nextObject = m_children[i];

          cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
          if (nextMesh)
          {
              nextMesh->setMaterial(a_mat, a_affectChildren);
          }
      }
  }
}

//===========================================================================
/*!
     Enable or disable texture-mapping, possibly recursively affecting children

     \fn        void cMesh::useTexture(const bool a_useTexture,
                const bool a_affectChildren)
     \param     a_useTexture If \b true, then texture mapping is used.
     \param     a_affectChildren  If \b true, then children are also modified.
*/
//===========================================================================
void cMesh::useTexture(const bool a_useTexture, const bool a_affectChildren)
{
    m_useTextureMapping = a_useTexture;

    // propagate changes to children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->useTexture(a_useTexture, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Define the way normals are graphically rendered, optionally propagating
     the operation to my children

     \fn        void cMesh::setNormalsProperties(const double a_length,
                const cColorf& a_color, const bool a_affectChildren)
     \param     a_length  Length of normals
     \param     a_color  Color of normals
     \param     a_affectChildren  If \b true, then children also modified.
*/
//===========================================================================
void cMesh::setNormalsProperties(const double a_length, const cColorf& a_color,
                                 const bool a_affectChildren)
{
    m_showNormalsLength = a_length;
    m_showNormalsColor = a_color;

    // propagate changes to children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->setNormalsProperties(a_length, a_color, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Enable or disable the graphic rendering of normal vectors at each vertex,
     optionally propagating the operation to my children

     \fn        void cMesh::showNormals(const bool a_showNormals,
                const bool a_affectChildren)
     \param     a_showNormals If \b true, normal vectors are rendered graphically
     \param     a_affectChildren  If \b true, then children also modified
*/
//===========================================================================
void cMesh::showNormals(const bool a_showNormals, const bool a_affectChildren)
{
    m_showNormals = a_showNormals;

    // propagate changes to children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            cGenericObject *nextObject = m_children[i];

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->showNormals(a_showNormals, a_affectChildren);
            }
        }
    }
}


//===========================================================================
/*!
     Compute the axis-aligned boundary box that encloses all triangles in this mesh

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

    // loop over all my triangles
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
     Resize the current mesh by scaling all my vertex positions

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
     Sorting functions to help in the findNeighbors method

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
     Set up an AABB collision detector for this mesh and (optionally) its children

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

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->createAABBCollisionDetector(a_affectChildren,
                                                      a_useNeighbors);
            }
        }
    }
}


//===========================================================================
/*!
     Set up a sphere tree collision detector for this mesh and (optionally) its children

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
        cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
        if (nextMesh)
        {
          nextMesh->createSphereTreeCollisionDetector(a_affectChildren,
                                                      a_useNeighbors);
        }
      }
    }
}


//===========================================================================
/*!
     Render this mesh in OpenGL.  This method actually just prepares some 
     OpenGL state, and uses renderMesh to actually do the rendering.
     
     \fn       void cMesh::render(const int a_renderMode)
     \param    a_renderMode  Rendering mode (see cGenericObject)
*/
//===========================================================================
void cMesh::render(const int a_renderMode)
{

    // if transparency is enabled, either via multipass rendering or via
    // standard alpha-blending...
    if (m_useTransparency)
    {

      // if we're using multipass transparency, render only on 
      // the front and back passes
      if (m_useMultipassTransparency)
      {

          // render transparent front triangles
          if (a_renderMode == CHAI_RENDER_MODE_TRANSPARENT_FRONT_ONLY)
          {
              glEnable(GL_CULL_FACE);
              glCullFace(GL_BACK);
              renderMesh(a_renderMode);
          }

          // render transparent back triangles
          else if (a_renderMode == CHAI_RENDER_MODE_TRANSPARENT_BACK_ONLY)
          {
              glEnable(GL_CULL_FACE);
              glCullFace(GL_FRONT);
              renderMesh(a_renderMode);
          }

          // Multipass is enabled for this object but not for the camera, so do 
          // a simple pass with transparency on...
          else if (a_renderMode == CHAI_RENDER_MODE_RENDER_ALL)
          {
              // Turn culling off for transparent objects...
              glDisable(GL_CULL_FACE);
              renderMesh(a_renderMode);
          }
      }

      // multipass transparency is disabled; render only on non-transparent passes
      else
      {
          if ( (a_renderMode == CHAI_RENDER_MODE_NON_TRANSPARENT_ONLY) || (a_renderMode == CHAI_RENDER_MODE_RENDER_ALL) )
          {
              // Turn culling off for transparent objects...
              glDisable(GL_CULL_FACE);
              renderMesh(a_renderMode);
          }
      }
    }

    // if transparency is disabled...
    else
    {
        // render only on non-transparent passes
        if ( (a_renderMode == CHAI_RENDER_MODE_NON_TRANSPARENT_ONLY) || (a_renderMode == CHAI_RENDER_MODE_RENDER_ALL) )
        {
          // render a non-transparent mesh
          if (m_cullingEnabled) 
          {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
          }
          
          else
          {
            glDisable(GL_CULL_FACE);
          }

          renderMesh(a_renderMode);
        }
    }


    // Only render normals on one pass, no matter what the transparency
    // options are...
    if ( (a_renderMode == CHAI_RENDER_MODE_NON_TRANSPARENT_ONLY) || (a_renderMode == CHAI_RENDER_MODE_RENDER_ALL) )
    {
        // render normals
        if (m_showNormals) renderNormals();        
    }
}


//===========================================================================
/*!
     Invalidate any existing display lists.  You should call this on if you're using
     display lists and you modify mesh options, vertex positions, etc.

     \fn       void cMesh::invalidateDisplayList()
     \param    a_affectChildren  If \b true all children are updated
*/
//===========================================================================
void cMesh::invalidateDisplayList(bool a_affectChildren)
{

    // Delete my display list if necessary
    if (m_displayList != -1)
    {
        glDeleteLists(m_displayList,1);
        m_displayList = -1;
    }

    // Propagate the operation to my children
    if (a_affectChildren)
    {
        unsigned int i, numItems;
        numItems = m_children.size();
        for (i=0; i<numItems; i++)
        {
            cGenericObject *nextObject = m_children[i];

            cMesh *nextMesh = dynamic_cast<cMesh*>(nextObject);
            if (nextMesh)
            {
                nextMesh->invalidateDisplayList(a_affectChildren);
            }
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

// Technically this is illegal; display lists aren't necessarily
// supposed to support vertex arrays.  But it works.  So far, I have
// not found it to be faster using immediate commands, so we do this
// for now.  If it turns out that some implementations actually don't
// allow this, we'll just change this #define later on.
#define USE_ARRAYS_INSIDE_DISPLAY_LISTS 1

// On some machines, GL_COMPILE_AND_EXECUTE totally blows for some reason,
// so even though it's more complex on the first rendering pass, we use
// GL_COMPILE (and _repeat_ the first rendering pass)
#define DISPLAY_LIST_GENERATION_MODE    GL_COMPILE

//===========================================================================
/*!
     Render the mesh itself.  This function is declared public to allow
     sharing of data among meshes, which is not possible given most
     implementations of 'protected'.  But it should only be accessed
     from within render() or derived versions of render().

     \fn       void cMesh::renderMesh(const int a_renderMode)
*/
//===========================================================================
void cMesh::renderMesh(const int a_renderMode)
{

    int creating_display_list = 0;

    // Should we render with a display list?
    if (m_useDisplayList)
    {
        // If the display list doesn't exist, create it
        if (m_displayList == -1)
        {
             m_displayList = glGenLists(1);
             if (m_displayList == -1) return;
             glNewList(m_displayList,DISPLAY_LIST_GENERATION_MODE);
             creating_display_list = 1;             

             // Go ahead and render; we'll create this list now...
             //
             // If we're not using compile_and_execute, we'll make another
             // (recursive) call to renderMesh() at the end of this function.
        }

        // Otherwise all we have to do is call the display list
        else
        {         
            glCallList(m_displayList);

            // All done...
            return;
        }
    }

    // initialize rendering arrays
    if (USE_ARRAYS_INSIDE_DISPLAY_LISTS || m_useDisplayList == false) {
      glEnableClientState(GL_NORMAL_ARRAY);
      glEnableClientState(GL_VERTEX_ARRAY);
    }    
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);
    glDisableClientState(GL_EDGE_FLAG_ARRAY);

    // set polygon and face mode
    glPolygonMode(GL_FRONT_AND_BACK, m_triangleMode);

    // set up useful rendering state
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    // enable or disable blending
    if (m_useTransparency)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Note that we have to disable depth-writing if we really want 
        // blending to work
        glDepthMask(GL_FALSE);
    }

    // transparency is not used
    else
    {
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }

    // if material properties exist, render them
    if (m_useMaterialProperty)
    {
        m_material.render();        
    }

    // should we use vertex colors?
    if (m_useVertexColors)
    {
        // Clear the effects of material properties...
        if (m_useMaterialProperty == 0)
        {
          float fnull[4] = {0,0,0,0};
          glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const float *)&fnull);
          glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (const float *)&fnull);
        }

        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);        
        glEnableClientState(GL_COLOR_ARRAY);
    }
    else
    {
        glDisable(GL_COLOR_MATERIAL);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    // A default color for objects that don't have vertex colors or
    // material properties (otherwise they're invisible)...
    if (m_useVertexColors == 0 && m_useMaterialProperty == 0)
    {
      glEnable(GL_COLOR_MATERIAL);        
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glColor4f(1,1,1,1);
    }

    // if we have a texture, enable it
    if ((m_texture != NULL) && m_useTextureMapping)
    {
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        m_texture->render();
    }

    if (USE_ARRAYS_INSIDE_DISPLAY_LISTS || m_useDisplayList == 0) {
      // specify pointers to rendering arrays
      glVertexPointer(3, GL_DOUBLE, sizeof(cVertex), &m_vertices[0].m_localPos);
      glNormalPointer(GL_DOUBLE, sizeof(cVertex), &m_vertices[0].m_normal);
      glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(cVertex), m_vertices[0].m_color.pColor());
      glTexCoordPointer(2, GL_DOUBLE, sizeof(cVertex), &m_vertices[0].m_texCoord);
    }

    // render all active triangles
    glBegin(GL_TRIANGLES);
    unsigned int i;
    unsigned int numItems = m_triangles.size();
    
    for(i=0; i<numItems; i++)
    {
        bool allocated = m_triangles[i].m_allocated;
        if (allocated==false) continue;

        // Render from vertex arrays if we're not in a display list
        if (USE_ARRAYS_INSIDE_DISPLAY_LISTS || m_useDisplayList == false) {
          unsigned int index0 = m_triangles[i].m_indexVertex0;
          unsigned int index1 = m_triangles[i].m_indexVertex1;
          unsigned int index2 = m_triangles[i].m_indexVertex2;
          glArrayElement(index0);
          glArrayElement(index1);
          glArrayElement(index2);
        }

        // Technically, we're supposed to use immediate commands if we're inside a display list.
        //
        // I'm still looking at the performance impacts of display lists and the generality of
        // vertex arrays inside display lists...
        else {
          for(int j=0; j<3; j++) {
// Suppress warnings here because we're consciously casting everything down...            
#pragma warning(push)
#pragma warning(disable:4244)  
            cVertex* v = m_triangles[i].getVertex(j);
            glNormal3f(v->m_normal.x,v->m_normal.y,v->m_normal.z);
            if (m_useTextureMapping) glTexCoord2f(v->m_texCoord.x,v->m_texCoord.y);
            if (m_useVertexColors)   glColor4b(v->m_color.m_color[0],v->m_color.m_color[1],v->m_color.m_color[2],v->m_color.m_color[3]);
            glVertex3f(v->m_localPos.x,v->m_localPos.y,v->m_localPos.z);            
#pragma warning(pop)
          }
        }         
    }
    glEnd();

    // restore OpenGL settings to reasonable defaults
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Turn off any array variables I might have turned on...
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    // If we've gotten this far and we're using a display list for rendering,
    // we must be capturing it right now...
    if (m_useDisplayList) {
      glEndList();

      // Recursively make a call to actually render this object if
      // we didn't use compile_and_execute
#if (DISPLAY_LIST_GENERATION_MODE == GL_COMPILE)
      if (m_useDisplayList && m_displayList != -1) renderMesh(a_renderMode);
#endif
    }

}



//===========================================================================
/*!
    Users can call this function when it's necessary to re-initialize the OpenGL
    context; e.g. re-initialize textures and display lists.  Subclasses should
    perform whatever re-initialization they need to do.

    \fn     void cMesh::onDisplayReset(const bool a_affectChildren)
    \param  a_affectChildren  If \b true all children are updated
*/
//===========================================================================
void cMesh::onDisplayReset(const bool a_affectChildren)
{
    invalidateDisplayList();
    if (m_texture != NULL) m_texture->markForUpdate();

    // Use the superclass method to call the same function on the rest of the
    // scene graph...
    cGenericObject::onDisplayReset();
}