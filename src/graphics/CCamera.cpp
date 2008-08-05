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
#include "CCamera.h"
#include "CWorld.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      Create a camera by passing the parent world as parameter.

      \fn         cCamera::cCamera(cWorld* a_parentWorld)
      \param      a_parentWorld  Parent world camera.
*/
//===========================================================================
cCamera::cCamera(cWorld* a_parentWorld)
{
    // set default values for clipping planes
    setClippingPlanes(0.1, 1000.0);

    // set default field of view.
    setFieldViewAngle(45);

    // set parent world
    m_parentWorld = a_parentWorld;

    // position and orient camera
    set(
          cVector3d(0,0,0),       // Local Position of camera.
          cVector3d(-1,0,0),      // Local Look At position
          cVector3d(0,0,1)        // Local Up Vector
        );

    // set default stereo parameters
    m_stereoFocalLength = 5.0;
    m_stereoEyeSeparation = 0.5;
}


//===========================================================================
/*!
      Set the position and orientation of the camera. Three vectors are
      required: [iPosition] which describes the position in world coordinates
      of the camera.
      [iLookAt] which describes a point where the camera is looking at.
      Finally [iUp] to oriente the camera around its rolling axis. [iUp]
      always points to the top of the image.

      \fn         bool cCamera::set(const cVector3d& a_localPosition,
                  const cVector3d& a_localLookAt, const cVector3d& a_localUp)

      \param      a_localPosition  Position of the camera in world coordinates.
      \param      a_localLookAt  Center of image. Point in space focused by the camera.
      \param      a_localUp  Vector giving the rolling orientation. If camera
                  is set horizontal then iUp=(0,0,1)
*/
//===========================================================================
bool cCamera::set(const cVector3d& a_localPosition, const cVector3d& a_localLookAt,
                  const cVector3d& a_localUp)
{
    // copy new values to temp variables
    cVector3d pos = a_localPosition;
    cVector3d lookAt = a_localLookAt;
    cVector3d up = a_localUp;
    cVector3d Cy;

    // check validity of vectors
    if (pos.distancesq(lookAt) < CHAI_SMALL) { return (false); }
    if (up.lengthsq() < CHAI_SMALL) { return (false); }

    // compute new rotation matrix
    pos.sub(lookAt);
    pos.normalize();
    up.normalize();
    up.crossr(pos, Cy);
    if (Cy.lengthsq() < CHAI_SMALL) { return (false); }
    Cy.normalize();
    pos.crossr(Cy,up);

    // update frame with new values
    setPos(a_localPosition);
    cMatrix3d localRot;
    localRot.setCol(pos, Cy, up);
    setRot(localRot);

    // return success
    return (true);
}


//===========================================================================
/*!
      Set the field of view angle. Angle is defined in /e degrees.

      \fn         void setFieldViewAngle(double a_fieldViewAngle)
      \param      a_fieldViewAngle  Field of view angle in \e degrees.
*/
//===========================================================================
void cCamera::setFieldViewAngle(double a_fieldViewAngle)
{
    m_fieldViewAngle = cClamp(a_fieldViewAngle, 0.0, 180.0);
}


//===========================================================================
/*!
      Set stereo focal length

      \fn         int cCamera::setStereoFocalLength(double a_stereoFocalLength)
      \param      a_stereoFocalLength  Focal length.
*/
//===========================================================================
int cCamera::setStereoFocalLength(double a_stereoFocalLength)
{
    if (a_stereoFocalLength > 0)
    {
        m_stereoFocalLength = a_stereoFocalLength;
        return (0);
    }
    else
    {
        return (-1);
    }
}


//===========================================================================
/*!
      Set stereo eye separation

      \fn         int cCamera::setStereoEyeSeparation(double a_stereoEyeSeparation)
      \param      a_stereoEyeSeparation  Distance between both eyes
*/
//===========================================================================
int cCamera::setStereoEyeSeparation(double a_stereoEyeSeparation)
{
    if (a_stereoEyeSeparation >= 0)
    {
        m_stereoEyeSeparation = a_stereoEyeSeparation;
        return (0);
    }
    else
    {
        return (-1);
    }
}



//===========================================================================
/*!
      Set the clipping planes distances from camera.

      \fn       void setClippingPlanes(const double a_distanceNear,
                const double a_distanceFar)
      \param    a_distanceNear  Near clipping plane
      \param    a_distanceFar  Far clipping plane
*/
//===========================================================================
void cCamera::setClippingPlanes(const double a_distanceNear, const double a_distanceFar)
{
    // check values of near and far clipping planes
    if ((a_distanceNear > 0.0) &&
        (a_distanceFar > 0.0) &&
        (a_distanceFar > a_distanceNear))
    {
        m_distanceNear = a_distanceNear;
        m_distanceFar = a_distanceFar;
    }
}


//===========================================================================
/*!
    Check for collision detection between a mouse click and an object
    displayed in the virtual scene

    \fn         bool cCamera::select(const int a_windowPosX, const int a_windowPosY,
                const int a_windowWidth, const int a_windowHeight, cGenericObject*& a_selectedObject,
                cTriangle*& a_selectedTriangle, cVector3d& a_selectedPoint, double a_selectedDistance,
                const bool a_visibleObjectsOnly);
     \param     a_windowPosX  X coodinate position of mouse click.
     \param     a_windowPosY  Y coodinate position of mouse click.
     \param     a_windowWidth  Width of window display.
     \param     a_windowHeight  Height of window display.
     \param     a_selectedObject  Pointer to selected object.
     \param     a_selectedTriangle  Pointer to selected triangle.
     \param     a_selectedPoint  Point is space where collision hit occured.
     \param     a_selectedDistance  Distance between camera and collision hit.
     \return    Return \b true if an object has been hit.
*/
//===========================================================================
bool cCamera::select(const int a_windowPosX, const int a_windowPosY,
     const int a_windowWidth, const int a_windowHeight, cGenericObject*& a_selectedObject,
     cTriangle*& a_selectedTriangle, cVector3d& a_selectedPoint, double a_selectedDistance,
     const bool a_visibleObjectsOnly)
{
    // initialize variables
    bool result;

    a_selectedObject = NULL;
    a_selectedTriangle = NULL;
    a_selectedPoint.set(0.0, 0.0, 0.0);
    a_selectedDistance = CHAI_LARGE;

    // update global frame of camera
    computeGlobalCurrentObjectOnly(true);

    // check field of view of camera
    if (fabs(m_fieldViewAngle) < 0.001f) { return (false); }

    // compute ray
    double distCam = (a_windowHeight / 2.0f) / cTanDeg(m_fieldViewAngle / 2.0f);

    cVector3d selectRay;
    selectRay.set(-distCam,
                   (a_windowPosX - (a_windowWidth / 2.0f)),
                   ((a_windowHeight / 2.0f) - a_windowPosY));
    selectRay.normalize();

    selectRay = cMul(m_globalRot, selectRay);
    cVector3d selectPoint = cAdd(m_globalPos, cMul(100000, selectRay));


    // search for intersection between ray and objects in world
    result = m_parentWorld->computeCollisionDetection(
                                m_globalPos,
                                selectPoint,
                                a_selectedObject,
                                a_selectedTriangle,
                                a_selectedPoint,
                                a_selectedDistance,
                                a_visibleObjectsOnly, 0);

    // return result.
    return(result);
}


//===========================================================================
/*!
      Render  camera in OpenGL.

      \fn         void cCamera::render(const int a_windowWidth,
                  const int a_windowHeight, const int a_imageIndex)
      \param      a_windowWidth  Width of Viewport.
      \param      a_windowHeight  Height of Viewport.
      \param      a_imageIndex  Stereo Left, Stereo Right or Mono image.
*/
//===========================================================================
void cCamera::renderView(const int a_windowWidth, const int a_windowHeight,
                         const int a_imageIndex)
{
    // compute global pose
    computeGlobalCurrentObjectOnly(true);

    // set background color
    cColorf color = m_parentWorld->getBackgroundColor();
    glClearColor(color.getR(), color.getG(), color.getB(), color.getA());

    // clear buffers (unless we're rendering the second image in a stereo
    // pair, which is the right image by convention)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set depth buffer writable
    glDepthMask(GL_TRUE);

    // enable depth test
    glEnable(GL_DEPTH_TEST);

    // set projection matrix mode and initialize matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // check window size
    if (a_windowHeight == 0) { return; }

    // setup perspective projection
    double glAspect = ((double)a_windowWidth / (double)a_windowHeight);

    // set viewpoint for a non stereoscopic rendering
    if (a_imageIndex == CHAI_MONO)
    {
        gluPerspective(
                m_fieldViewAngle,   // Field of View Angle.
                glAspect,           // Aspect ratio of viewing volume.
                m_distanceNear,     // Distance to Near clipping plane.
                m_distanceFar);     // Distance to Far clipping plane.


        // render pose
        cVector3d lookAt = m_globalRot.getCol0();
        cVector3d lookAtPos;
        m_globalPos.subr(lookAt, lookAtPos);
        cVector3d up = m_globalRot.getCol2();

        gluLookAt( m_globalPos.x,    m_globalPos.y,   m_globalPos.z,
                   lookAtPos.x,    lookAtPos.y,   lookAtPos.z,
                   up.x,           up.y,          up.z );
    }

    // set up viewport for stereoscopic rendering (left/right eye)
    else
    {
        
      double radians = ((CHAI_PI / 180.0) * m_fieldViewAngle / 2.0f);
      double wd2 = m_distanceNear * tan(radians);
      double ndfl = m_distanceNear / m_stereoFocalLength;

      cVector3d lookv = m_globalRot.getCol0();
      lookv.mul(-1.0);

      cVector3d upv = m_globalRot.getCol2();
      cVector3d offsetv = cCross(lookv,upv);

      offsetv.mul(m_stereoEyeSeparation / 2.0);

      if (a_imageIndex == CHAI_STEREO_LEFT) offsetv.mul(-1.0);

      double stereo_multiplier = (a_imageIndex == CHAI_STEREO_LEFT) ? 1.0f : -1.0f;

      double left   = -1.0 * glAspect * wd2 + stereo_multiplier * 0.5 * m_stereoEyeSeparation * ndfl;
      double right  =        glAspect * wd2 + stereo_multiplier * 0.5 * m_stereoEyeSeparation * ndfl;
      double top    =        wd2;
      double bottom = -1.0 * wd2;

      glFrustum(left,right,bottom,top,m_distanceNear,m_distanceFar);

      // Compute offset camera position
      cVector3d pos = cAdd(m_globalPos,offsetv);

      // Compute offset camera look position
      cVector3d lookAtPos;
      pos.addr(lookv, lookAtPos);

      gluLookAt(pos.x,       pos.y,       pos.z,
                lookAtPos.x, lookAtPos.y, lookAtPos.z,
                upv.x,       upv.y,       upv.z
                );

    }

    // render world
    m_parentWorld->renderSceneGraph(CHAI_RENDER_NON_TRANSPARENT);
    m_parentWorld->renderSceneGraph(CHAI_RENDER_TRANSPARENT_BACK);
    m_parentWorld->renderSceneGraph(CHAI_RENDER_TRANSPARENT_FRONT);
}

