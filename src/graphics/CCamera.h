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
#ifndef CCameraH
#define CCameraH
//---------------------------------------------------------------------------
#include "cGenericObject.h"
#include "CMaths.h"
#include "windows.h"
#include "gl/gl.h"
#include "gl/glu.h"
//---------------------------------------------------------------------------
class cWorld;
//---------------------------------------------------------------------------
const int CHAI_MONO             =  0;
const int CHAI_STEREO_LEFT      = -1;
const int CHAI_STEREO_RIGHT     =  1;
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class      cCamera
      \brief      cCamera describes a virtual Camera located inside the world.
                  The output of image of a virtual camera can be displayed
                  on a Viewport. see cViewport.
*/
//===========================================================================
class cCamera : public cGenericObject
{
  friend class cWorld;

  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cCamera.
    cCamera(cWorld* iParent);
    //! Destructor of cCamera.
    virtual ~cCamera() {};

    // METHODS:
    //! Set the position and orientation of the camera.
    virtual bool set(const cVector3d& a_localPosition, const cVector3d& a_localLookAt,
             const cVector3d& a_localUp);

    //! Get pointer to parent world.
    cWorld* getParentWorld() { return (m_parentWorld); }

    //! Set clipping planes.
    void setClippingPlanes(const double a_distanceNear, const double a_distanceFar);

    //! Get near clipping plane
    double getNearClippingPlane() { return (m_distanceNear); }

    //! Get far clipping plane
    double getFarClippingPlane() { return (m_distanceFar); }

    //! Set field of view.
    void setFieldViewAngle(double a_fieldViewAngle);

    //! Read field of view
    double getFieldViewAngle() { return (m_fieldViewAngle); }

    //! Set stereo focal length
    int setStereoFocalLength(double a_stereoFocalLength);

    //! Get stereo focal length
    double getStereoFocalLength() { return (m_stereoFocalLength); }

    //! Set stereo eye separation
    int setStereoEyeSeparation(double a_stereoEyeSeparation);

    //! Get stereo eye separation
    double getStereoEyeSeparation() { return (m_stereoEyeSeparation); }

    //! Render the camera in OpenGL.
    virtual void renderView(const int a_windowWidth, const int a_windowHeight, const int a_imageIndex);

    //! Check any collision between mouse cursor and object in scenegraph
    virtual bool select(const int a_windowPosX, const int a_windowPosY,
                const int a_windowWidth, const int a_windowHeight, cGenericObject*& a_selectedObject,
                cTriangle*& a_selectedTriangle, cVector3d& a_selectedPoint, double a_selectedDistance,
                const bool a_visibleObjectsOnly);

  protected:

    // PROPERTIES:
    //! Parent world.
    cWorld *m_parentWorld;

    //! Distance to Near clipping plane.
    double m_distanceNear;
    //! Distance to Far clipping plane.
    double m_distanceFar;
    //! Field of view. Angle in degrees.
    double m_fieldViewAngle;

    // Stereo Parameters:
    //! Focal length.
    double m_stereoFocalLength;
    //! Eye separation
    double m_stereoEyeSeparation;

    // VIRTUAL METHODS:
    //! Update global positions. This method is specific to each object type.
    virtual void updateGlobalPositions(const bool a_frameOnly) {};
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

