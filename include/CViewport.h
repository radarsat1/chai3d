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
#ifndef CViewportH
#define CViewportH
//---------------------------------------------------------------------------
#include "windows.h"
#include "gl/gl.h"
#include "CWorld.h"
#include "CCamera.h"
#include "CMatrix3d.h"
#include "CVector3d.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class      cViewport
      \brief

      cViewport describes a two dimensional window display for rendering an
      OpenGL scene. You can image the viewport as being a television connected
      to a camera (cCamera) located inside a world (cWorld).
*/
//===========================================================================
class cViewport
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cViewport.
    cViewport(HWND a_winHandle, cCamera *a_camera, const bool a_stereoEnabled = false);
    //! Destructor of cViewport.
    ~cViewport();

    // METHODS:
    //! Get width of viewport.
    unsigned int getHeight() { return (m_height); }
    //! Get height of viewport.
    unsigned int getWidth() { return (m_width); }

    //! Set camera of viewport.
    void setCamera(cCamera *a_camera);
    //! Get camera of viewport.
    cCamera* getCamera() { return (m_camera); }

    //! Set status of viewport.
    void setEnabled( const bool a_enabled ) { m_enabled = a_enabled; }
    //! Get status of viewport.
    bool getEnabled() { return (m_enabled); }

    //! Set display mode
    //!
    //! Note that it is not possible to change the pixel format of a window
    //! in Windows, so it is the user's responsibility to create a _new_
    //! window and a _new_ viewport before enabling stereo.  Therefore calling this
    //! function on a window that _didn't_ initially have stereo enabled
    //! will not work.
    void setStereoOn(bool a_stereoEnabled);

    //! Get current display mode.
    bool getStereoOn() { return (m_stereoEnabled); }

    //! Render the scene in OpenGL.
    bool render();

    //! Select an object inside the viewport.
    bool select(const unsigned int a_windowPosX, const unsigned int a_windowPosY,
                const bool a_selectVisibleObjectsOnly);
    //! Get last selected mesh.
    cGenericObject* getLastSelectedObject() { return (m_lastSelectedObject); }
    //! Get last selected triangle.
    cTriangle* getLastSelectedTriangle() { return (m_lastSelectedTriangle); }
    //! Get last selected point position.
    cVector3d getLastSelectedPoint(void) { return(m_lastSelectedPoint); }
    //! Get distance to last selected object.
    double getLastSelectedDistance(void) { return (m_lastSelectedDistance); }

  private:
    // PROPERTIES:
    //!  Virtual camera connected to this viewport.
    cCamera* m_camera;
    //! Status of viewport.
    bool m_enabled;
    //! Stereo status.
    bool m_stereoEnabled;
    //! Last selected mesh.
    cGenericObject* m_lastSelectedObject;
    //! Last selected triangle.
    cTriangle* m_lastSelectedTriangle;
    //! Last selected point.
    cVector3d m_lastSelectedPoint;
    //! Distance between camera and last selected point.
    double m_lastSelectedDistance;
    //! Handle to window display.
    HWND m_winHandle;
    //! OpenGL display context
    HGLRC m_glContext;
    //! display context
    HDC m_glDC;
    //! GL Status
    bool m_glReady;
    //! Width of viewport
    unsigned int m_width;
    //! Height of viewport
    unsigned int m_height;

    // METHODS:
    //! Update display context.
    bool update();
    //! Clean up the current rendering context
    bool cleanup();
    //! Render the scene in OpenGL.
    bool renderView(const int a_imageIndex);
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

