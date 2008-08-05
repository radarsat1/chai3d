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
#include "CWorld.h"
#include "CLight.h"
//---------------------------------------------------------------------------

#ifndef _MSVC
#include <float.h>
#endif


//==========================================================================
/*!
      Constructor of cWorld.

      \fn       cWorld::cWorld()
*/
//===========================================================================
cWorld::cWorld()
{
    #ifndef _MSVC
    _control87(MCW_EM,MCW_EM);
    #endif

    // set background properties
    m_backgroundColor.set(0.0f, 0.0f, 0.0f, 1.0f);

    m_renderLightSources = 1;    
}


//===========================================================================
/*!
      Destructor of cWorld

      \fn       cWorld::~cWorld()
*/
//===========================================================================
cWorld::~cWorld()
{
    // delete all children
    deleteAllChildren();

    // clear textures list
    deleteAllTextures();
}


//===========================================================================
/*!
    Create new texture and add it to textures list.

    \fn         cTexture2D* cWorld::newTexture()
    \return     Return pointer to new texture entity.
*/
//===========================================================================
cTexture2D* cWorld::newTexture()
{
    // create new texture entity
    cTexture2D* newTexture = new cTexture2D();

    // add texture to list
    m_textures.push_back(newTexture);

    // return pointer to new texture
    return (newTexture);
}


//===========================================================================
/*!
    Add texture to texture list.

    \fn         void cWorld::addTexture(cTexture2D* a_texture)
    \param      a_texture  Texture to be added to the textures list.
*/
//===========================================================================
void cWorld::addTexture(cTexture2D* a_texture)
{
    // add texture to list
    m_textures.push_back(a_texture);
}


//===========================================================================
/*!
    Remove texture from textures list. Texture is not deleted from memory.

    \fn         bool cWorld::removeTexture(cTexture2D* a_texture)
    \param      a_texture  Texture to be removed from textures list.
    \return     Return \b true if operation succeeded
*/
//===========================================================================
bool cWorld::removeTexture(cTexture2D* a_texture)
{
    // set iterator
    std::vector<cTexture2D*>::iterator nextTexture;
    nextTexture = m_textures.begin();

    // search texture in  list and remove it
    for (unsigned int i=0; i<m_textures.size(); i++)
    {
        if ((*nextTexture) == a_texture)
        {
            // remove object from list
            m_textures.erase(nextTexture);

            // return success
            return (true);
        }
    }

    // operation failed
    return (false);
}


//===========================================================================
/*!
    Delete texture from textures list and erase it from memory.

    \fn         bool cWorld::deleteTexture(cTexture2D* a_texture)
    \param      a_texture  Texture to be deleted.
    \return     Return \b true if operation suceeded
*/
//===========================================================================
bool cWorld::deleteTexture(cTexture2D* a_texture)
{
    // remove texture from list
    bool result = removeTexture(a_texture);

    // if operation succeeds, delete object
    if (result)
    {
        delete a_texture;
    }

    // return result
    return (result);
}


//===========================================================================
/*!
    Delete all texture from memory.

    \fn         void cWorld::deleteAllTextures()
*/
//===========================================================================
void cWorld::deleteAllTextures()
{
    // delete all textures
    for (unsigned int i=0; i<m_textures.size(); i++)
    {
        cTexture2D* nextTexture = m_textures[i];
        delete nextTexture;
    }

    // clear textures list
    m_textures.clear();
}


//===========================================================================
/*!
    Set the background color used when rendering.  This really belongs in
    cCamera or cViewport; it's a historical artifact that it lives here.

    \fn         void cWorld::setBackgroundColor(const GLfloat a_red,
                const GLfloat a_green, const GLfloat a_blue)
    \param      a_red  Red component.
    \param      a_green  Green component.
    \param      a_blue  Blue component.
*/
//===========================================================================
void cWorld::setBackgroundColor(const GLfloat a_red, const GLfloat a_green,
                               const GLfloat a_blue)
{
    m_backgroundColor.set(a_red, a_green, a_blue);
}


//===========================================================================
/*!
    Set the background color used when rendering.  This really belongs in
    cCamera or cViewport; it's a historical artfact that it lives here.

    \fn         void cWorld::setBackgroundColor(const cColorf& a_color)
    \param      a_color  new background color.
*/
//===========================================================================
void cWorld::setBackgroundColor(const cColorf& a_color)
{
    m_backgroundColor = a_color;
}


//===========================================================================
/*!
    Add an OpenGL light source to the world. A maximum of eight light
    sources can be registered. For each registered light source, an
    OpenGL lightID number is defined

    \fn         bool cWorld::addLightSource(cLight* a_light)
    \param      a_light light source to register.
    \return     return \b true if light source was registered, otherwise
                return \b false.
*/
//===========================================================================
bool cWorld::addLightSource(cLight* a_light)
{
    // check if number of lights already equal to 8.
    if (m_lights.size() >= MAXIMUM_OPENGL_LIGHT_COUNT)
    {
        return (false);
    }

    // search for a free ID number
    int lightID;
    bool found = false;
    int count = 0;

    while ((count < MAXIMUM_OPENGL_LIGHT_COUNT) && (!found))
    {
        // next possible ID:
        switch (count)
        {
            case 0: lightID = GL_LIGHT0; break;
            case 1: lightID = GL_LIGHT1; break;
            case 2: lightID = GL_LIGHT2; break;
            case 3: lightID = GL_LIGHT3; break;
            case 4: lightID = GL_LIGHT4; break;
            case 5: lightID = GL_LIGHT5; break;
            case 6: lightID = GL_LIGHT6; break;
            case 7: lightID = GL_LIGHT7; break;
        }

        // check if ID is not already used
        unsigned int i;
        bool free = true;
        for (i=0; i<m_lights.size(); i++)
        {
            cLight* nextLight = m_lights[i];

            if (nextLight->m_glLightNumber == lightID)
            {
                free = false;
            }
        }

        // check if a free ID was found
        if (free)
        {
            a_light->m_glLightNumber = lightID;
            found = true;
        }

        count++;
    }

    // finalize
    if (found)
    {
        m_lights.push_back(a_light);
        return (true);
    }
    else
    {
        return (false);
    }
}


//===========================================================================
/*!
    Remove a light source from world.

    \fn         bool cWorld::removeLightSource(cLight* a_light)
    \param      a_light light source to be removed.
    \return     return \b true if light source was removed, otherwise
                return \b false.
*/
//===========================================================================
bool cWorld::removeLightSource(cLight* a_light)
{
    // set iterator
    std::vector<cLight*>::iterator nextLight;

    for(nextLight = m_lights.begin();
        nextLight != m_lights.end();
        nextLight++ ) {


        if ((*nextLight) == a_light)
        {
            // remove object from list
            m_lights.erase(nextLight);

            // return success
            return (true);
        }

    }

    // operation failed
    return (false);
}


//===========================================================================
/*!
    Get access to a particular light source (between 0 and MAXIMUM_OPENGL_LIGHT_COUNT-1).
    Returns a pointer to the requested light, or zero if it's not available.

    \fn         cLight cWorld::getLightSource(int index)
    \param      index  Specifies the light (0 -> 7) that should be accessed
    \return     return \b A pointer to a valid light or 0 if that light doesn't exist                
*/
//===========================================================================
cLight* cWorld::getLightSource(int index) {

  // Make sure this is a valid index
  if (index < 0 || (unsigned int)(index) >= m_lights.size()) return 0;

  // Return the light that we were supplied with by the creator of the world
  return m_lights[index];

}


//===========================================================================
/*!
    Render the world in OpenGL.

    \fn         void cWorld::render(const int a_renderMode)
    \param      a_renderMode  Rendering Mode.
*/
//===========================================================================
void cWorld::render(const int a_renderMode)
{

    // Back up the "global" modelview matrix for future reference
    glGetDoublev(GL_MODELVIEW_MATRIX,m_worldModelView);

    if (m_renderLightSources) 
    {
      // enable lighting
      glEnable(GL_LIGHTING);

      // render light sources
      unsigned int i;
      for (i=0; i<m_lights.size(); i++)
      {
          m_lights[i]->renderLightSource();
      }    
    }
}


//===========================================================================
/*!
    Check if a ray intersects this current triangle. The segment is described
    by a start point /e a_segmentPointA and end point /e a_segmentPointB. \n

    If a collision occurs, the square distance between the segment start point
    and collision point in measured and compared to any previous collision
    information stored in parameters \e a_colObject, \e a_colTriangle,
    \e a_colPoint, and \e a_colSquareDistance. If the new collision is located
    nearer to ray origin than previous collision point, it is stored
    in the corresponding parameters \e a_colObject, \e a_colTriangle,
    \e a_colPoint, and \e a_colSquareDistance.

    \param  a_segmentPointA  Start point of segment.
    \param  a_segmentPointB  End point of segment.
    \param  a_colObject      Pointer to nearest collided object.
    \param  a_colTriangle    Pointer to nearest collided triangle.
    \param  a_colPoint       Position of nearest collision.
    \param  a_colDistance    Distance between ray origin and nearest collision point.
    \param  a_proxyCall      If this is > 0, this is a call from a proxy, and the value
                             of a_proxyCall specifies which call this is.  -1 for
                             non-proxy calls.
*/
//===========================================================================
bool cWorld::computeCollisionDetection(
        cVector3d& a_segmentPointA, cVector3d& a_segmentPointB,
        cGenericObject*& a_colObject, cTriangle*& a_colTriangle, cVector3d& a_colPoint,
        double& a_colDistance, const bool a_visibleObjectsOnly, int a_proxyCall,
        cGenericPointForceAlgo* force_algo)        
{
	 
    // no collision found yet
    bool hit = false;
    double colSquareDistance = CHAI_LARGE;
    cVector3d a_bck_segmentPointA,bcklocalSegmentPointA;

    // convert collision segment into local coordinate system.
    cMatrix3d transLocalRot;
    m_localRot.transr(transLocalRot);

    cVector3d localSegmentPointA = a_segmentPointA;
    localSegmentPointA.sub(m_localPos);
    transLocalRot.mul(localSegmentPointA);
    a_bck_segmentPointA   = a_segmentPointA;
    bcklocalSegmentPointA = localSegmentPointA;

    cVector3d localSegmentPointB = a_segmentPointB;
    localSegmentPointB.sub(m_localPos);
    transLocalRot.mul(localSegmentPointB);

    // initialize objects for calls
    cGenericObject* t_colObject;
    cTriangle *t_colTriangle;
    cVector3d t_colPoint;
    double t_colSquareDistance = colSquareDistance;

    for (unsigned int i=0; i<m_children.size(); i++)
		{
        localSegmentPointA = a_segmentPointA;
        localSegmentPointA.sub(m_localPos);
				transLocalRot.mul(localSegmentPointA);

				cMesh *meshObject = dynamic_cast<cMesh*>(m_children[i]);
        if (a_proxyCall == 1) {       
            if ( meshObject != NULL && meshObject->m_historyValid)
                AdjustCollisionSegment(a_segmentPointA,a_segmentPointB,localSegmentPointA,meshObject,force_algo);
				}

				int coll = m_children[i]->computeCollisionDetection(localSegmentPointA, localSegmentPointB,
                      t_colObject, t_colTriangle, t_colPoint, t_colSquareDistance, a_visibleObjectsOnly, a_proxyCall);

        if(coll == 1)
				{
            // object was hit
            hit = true;

            if (t_colSquareDistance < colSquareDistance) 
						{
                a_colObject = t_colObject;
                a_colTriangle = t_colTriangle;
                a_colPoint = t_colPoint;
                colSquareDistance = t_colSquareDistance;

								a_segmentPointA = localSegmentPointA;
								a_segmentPointB = localSegmentPointB;

                // convert collision point into parent coordinate
                m_localRot.mul(a_colPoint);
                a_colPoint.add(m_localPos);

                a_bck_segmentPointA   = a_segmentPointA;
                bcklocalSegmentPointA = localSegmentPointA;
						}
				}
        else 
        {
            a_segmentPointA       = a_bck_segmentPointA;
            bcklocalSegmentPointA = localSegmentPointA;
        }
		}

    // for optimization reasons, the collision detectors only computes the
    // square distance between origin (a_segmentA) and collision point.
    // compute square root to obtain real distance.
    a_colDistance = sqrt(colSquareDistance);

    // return result
    return (hit);
}
