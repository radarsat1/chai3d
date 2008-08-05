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
      Create a camera by passing the parent world as a parameter.

      \fn         cCamera::cCamera(cWorld* a_parentWorld)
      \param      a_parentWorld  Parent world camera.
*/
//===========================================================================
cCamera::cCamera(cWorld* a_parentWorld)
{
    // set default values for clipping planes
    setClippingPlanes(0.1, 1000.0);

    // set default field of view angle
    setFieldViewAngle(45);

    // set parent world
    m_parentWorld = a_parentWorld;

    // position and orient camera, looking down the negative x-axis
    // (the robotics convention)
    set(
          cVector3d(0,0,0),       // Local Position of camera.
          cVector3d(-1,0,0),      // Local Look At position
          cVector3d(0,0,1)        // Local Up Vector
        );

    // set default stereo parameters
    m_stereoFocalLength = 5.0;
    m_stereoEyeSeparation = 0.5;

    // disable multipass transparency rendering by default
    m_useMultipassTransparency = 0;
}


//===========================================================================
/*!
      Set the position and orientation of the camera. Three vectors are
      required:
      
      [iPosition] which describes the position in local coordinates
      of the camera

      [iLookAt] which describes a point at which the camera is looking

      [iUp] to orient the camera around its rolling axis. [iUp] always points
      to the top of the image.

      These vectors are used in the usual gluLookAt sense.

      \fn         bool cCamera::set(const cVector3d& a_localPosition,
                  const cVector3d& a_localLookAt, const cVector3d& a_localUp)

      \param      a_localPosition  The position of the camera in local coordinates
      \param      a_localLookAt  The Point in local space at which the camera looks
      \param      a_localUp  A vector giving the rolling orientation (points toward
                  the top of the image)
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
      Set the field of view angle in \e degrees

      \fn         void setFieldViewAngle(double a_fieldViewAngle)
      \param      a_fieldViewAngle  Field of view angle in \e degrees
                  (should be between 0 and 180)
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
    m_stereoFocalLength = a_stereoFocalLength;

    // Prevent 0 or negative focal lengths
    if (m_stereoFocalLength < CHAI_SMALL) 
      m_stereoFocalLength = CHAI_SMALL;

    return 0;    
}


//===========================================================================
/*!
      Set stereo eye separation

      \fn         int cCamera::setStereoEyeSeparation(double a_stereoEyeSeparation)
      \param      a_stereoEyeSeparation  Distance between the left and right eyes.
                  
      Note that the stereo pair can be reversed by supplying a negative
      eye separation.
*/
//===========================================================================
int cCamera::setStereoEyeSeparation(double a_stereoEyeSeparation)
{
    
    m_stereoEyeSeparation = a_stereoEyeSeparation;
  
    return 0;
}


//===========================================================================
/*!
      Set the positions of the near and far clip planes

      \fn       void setClippingPlanes(const double a_distanceNear,
                const double a_distanceFar)
      \param    a_distanceNear  Distance to near clipping plane
      \param    a_distanceFar   Distance to far clipping plane
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
    Check for collision detection between an x-y position (typically a mouse
    click) and an object in the scene

    \fn         bool cCamera::select(const int a_windowPosX, const int a_windowPosY,
                const int a_windowWidth, const int a_windowHeight, cGenericObject*& a_selectedObject,
                cTriangle*& a_selectedTriangle, cVector3d& a_selectedPoint, double a_selectedDistance,
                const bool a_visibleObjectsOnly);
     
     \param     a_windowPosX        X coodinate position of mouse click.
     \param     a_windowPosY        Y coodinate position of mouse click.
     \param     a_windowWidth       Width of window display (pixels)
     \param     a_windowHeight      Height of window display (pixels)
     \param     a_selectedObject    Returns a pointer to the selected object (if any)
     \param     a_selectedTriangle  Returns a pointer to selected triangle (if any)
     \param     a_selectedPoint     Returns the point is space where a collision was found (if any)
     \param     a_selectedDistance  Returns the distance between the camera and the collision point (if any)

     \return    Returns \b true if an object has been hit, else false
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

    // update my m_globalPos and m_globalRot variables
    computeGlobalCurrentObjectOnly(true);

    // make sure we have a legitimate field of view
    if (fabs(m_fieldViewAngle) < 0.001f) { return (false); }

    // compute the ray that leaves the eye point at the appropriate angle
    //
    // m_fieldViewAngle / 2.0 would correspond to the _top_ of the window
    double distCam = (a_windowHeight / 2.0f) / cTanDeg(m_fieldViewAngle / 2.0f);

    cVector3d selectRay;
    selectRay.set(-distCam,
                   (a_windowPosX - (a_windowWidth / 2.0f)),
                   ((a_windowHeight / 2.0f) - a_windowPosY));
    selectRay.normalize();

    selectRay = cMul(m_globalRot, selectRay);

    // create a point that's way out along that ray
    cVector3d selectPoint = cAdd(m_globalPos, cMul(100000, selectRay));

    // search for intersection between the ray and objects in the world
    result = m_parentWorld->computeCollisionDetection(
                                m_globalPos,
                                selectPoint,
                                a_selectedObject,
                                a_selectedTriangle,
                                a_selectedPoint,
                                a_selectedDistance,
                                a_visibleObjectsOnly, 0);
    
    return result;
}


//===========================================================================
/*!
      Set up the OpenGL perspective projection matrix, and nukes the contents
      of the GL buffers.  This function assumes the caller (typically cViewport)
      has set the appropriate buffer to be current.

      \fn         void cCamera::renderView(const int a_windowWidth,
                  const int a_windowHeight, const int a_imageIndex)
      \param      a_windowWidth  Width of Viewport.
      \param      a_windowHeight  Height of Viewport.
      \param      a_imageIndex  One of the following constants, identifying the frame
                  to be rendered:

                  CHAI_MONO, CHAI_STEREO_LEFT, CHAI_STEREO_RIGHT

                  Note that CHAI_STEREO_DEFAULT doesn't really make sense for
                  the camera; this is a constant to be sent to a _viewport_, 
                  which will then decide which frame(s) to ask the camera
                  to render.
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

    // clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // make the depth buffer writable
    glDepthMask(GL_TRUE);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // we want to modify the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // check window size
    if (a_windowHeight == 0) { return; }

    // set up perspective projection
    double glAspect = ((double)a_windowWidth / (double)a_windowHeight);

    // set the perspective up for monoscopic rendering
    if (a_imageIndex == CHAI_MONO || a_imageIndex == CHAI_STEREO_DEFAULT)
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

    // set the perspective up for stereoscopic rendering
    else
    {

      // Based on Paul Bourke's stereo rendering tutorial:
      //
      // http://astronomy.swin.edu.au/~pbourke/opengl/stereogl/

      double radians = ((CHAI_PI / 180.0) * m_fieldViewAngle / 2.0f);
      double wd2 = m_distanceNear * tan(radians);
      double ndfl = m_distanceNear / m_stereoFocalLength;

      // compute the look, up, and cross vectors
      cVector3d lookv = m_globalRot.getCol0();
      lookv.mul(-1.0);

      cVector3d upv = m_globalRot.getCol2();
      cVector3d offsetv = cCross(lookv,upv);

      offsetv.mul(m_stereoEyeSeparation / 2.0);

      if (a_imageIndex == CHAI_STEREO_LEFT) offsetv.mul(-1.0);

      // decide whether to offset left or right
      double stereo_multiplier = (a_imageIndex == CHAI_STEREO_LEFT) ? 1.0f : -1.0f;

      double left   = -1.0 * glAspect * wd2 + stereo_multiplier * 0.5 * m_stereoEyeSeparation * ndfl;
      double right  =        glAspect * wd2 + stereo_multiplier * 0.5 * m_stereoEyeSeparation * ndfl;
      double top    =        wd2;
      double bottom = -1.0 * wd2;

      glFrustum(left,right,bottom,top,m_distanceNear,m_distanceFar);

      // compute the offset we should apply to the current camera position
      cVector3d pos = cAdd(m_globalPos,offsetv);

      // compute the shifted camera position
      cVector3d lookAtPos;
      pos.addr(lookv, lookAtPos);

      // set up the projection matrix
      gluLookAt(pos.x,       pos.y,       pos.z,
                lookAtPos.x, lookAtPos.y, lookAtPos.z,
                upv.x,       upv.y,       upv.z
                );

    }

    // set the matrix mode back to modelview to be nice
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // render the world
    m_parentWorld->renderSceneGraph(CHAI_RENDER_NON_TRANSPARENT);
    
    // optionally perform additional rendering passes for transparency
    if (m_useMultipassTransparency) {
      m_parentWorld->renderSceneGraph(CHAI_RENDER_TRANSPARENT_BACK);
      m_parentWorld->renderSceneGraph(CHAI_RENDER_TRANSPARENT_FRONT);
    }
}


//===========================================================================
/*!
      Enable or disable multipass transparency... when this option is
      enabled (it's disabled by default), each time the camera is
      asked to render the scene, it will perform three rendering
      passes: a pass for non-transparent items, a pass for the backfaces
      of transparent items, and a pass for the frontfaces of transparent
      items.

      Objects being rendered are told which pass is current via the 
      parameter supplied to the render() function.

      Note that if you turn this option on, you need to make sure that
      your objects don't unnecessarily draw themselves three times...
      that is, if you have an object that doesn't care about multipass
      transparency, start your render() function with:
      
      if (a_renderMode != CHAI_RENDER_NON_TRANSPARENT) return;

      We will hopefully find a cleaner way to do this in the future, but
      for now be careful when you enable this feature...

      \fn         void cCamera::enableMultipassTransparency(bool enable)
*/
//===========================================================================
void cCamera::enableMultipassTransparency(bool enable) {
    m_useMultipassTransparency = enable;
}
