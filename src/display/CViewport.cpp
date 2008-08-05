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
#include "CViewport.h"
#include "CWorld.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cViewport.

    \fn         cViewport::cViewport(HWND a_winHandle, cCamera *a_camera,
                const bool a_stereoEnabled)
    \param      a_winHandle    Handle to the graphical display
    \param      a_camera       Camera inside world.
    \param      a_useStereo    If \b true, use stereo; else use mono.
*/
//===========================================================================
cViewport::cViewport(HWND a_winHandle, cCamera *a_camera, const bool a_stereoEnabled)
{
    // set camera of viewport
    setCamera(a_camera);

    // stereo status
    m_stereoEnabled = a_stereoEnabled;

    // update wincontrol
    m_winHandle = a_winHandle;

    m_glDC = 0;

    if (m_winHandle != NULL)
    {
        update();
    }
}


//===========================================================================
/*!
        Destructor of cViewport.

        \fn     cViewport::~cViewport()
*/
//===========================================================================
cViewport::~cViewport()
{
    cleanup();
}


//===========================================================================
/*!
        Attempt to enable stereo on the current viewport.

        \fn     cViewport::setStereoOn(bool a_stereoEnabled)
*/
//===========================================================================
void cViewport::setStereoOn(bool a_stereoEnabled)
{
    // check if new mode is not already active
    if (a_stereoEnabled == m_stereoEnabled) { return; }

    // update new mode
    m_stereoEnabled = a_stereoEnabled;

    // See whether stereo is _really_ enabled
    PIXELFORMATDESCRIPTOR pfd;
    int formatIndex = GetPixelFormat(m_glDC);
    DescribePixelFormat(m_glDC, formatIndex, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    // if stereo was enabled but can not be displayed, switch over to mono.
    if (((pfd.dwFlags & PFD_STEREO) == 0) && m_stereoEnabled)
    {
        m_stereoEnabled = false;
    }
}


//===========================================================================
/*!
        Clean up the current rendering context

        \fn     bool cViewport::cleanup()
*/
//===========================================================================
bool cViewport::cleanup()
{
    bool status = true;

    // delete display context
    int result = ReleaseDC(m_winHandle, m_glDC);
    if (result == 0) status = false;

    result = wglDeleteContext(m_glContext);    
    if (result == 0) status = false;

    m_glContext = 0;
    m_glDC = 0;
    m_glReady = false;
    return status;
}


//===========================================================================
/*!
        If the winControl handle has been modified, call this function
        to update the OpenGL display context.

        \fn         bool cViewport::update()
        \return     Return true if operation succeded.
*/
//===========================================================================
bool cViewport::update()
{

    // Clean up the old rendering context if necessary
    if (m_glDC) cleanup();    

    // declare variables
    int formatIndex;
    
    // viewport is not yet enabled
    m_enabled = false;

    // gl display not yet ready
    m_glReady = false;

    // check display handle
    if (m_winHandle == NULL) { return (false); }

    // get width and height of display
    RECT sizeWin;
    if (GetWindowRect(m_winHandle, &sizeWin) == 0) { return (false); }
    m_width   = sizeWin.right  - sizeWin.left;
    m_height  = sizeWin.bottom - sizeWin.top;

    // set up pixel format descriptor
    PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),       // size of this pfd
      1,                                   // version number
      PFD_DRAW_TO_WINDOW |                 // support window
      PFD_SUPPORT_OPENGL |                 // support OpenGL
      (m_stereoEnabled ? PFD_STEREO : 0) | // optionally enable stereo
      PFD_DOUBLEBUFFER,               // double buffered
      PFD_TYPE_RGBA,                  // RGBA type
      32,                             // 24-bit color depth
      0, 0, 0, 0, 0, 0,               // color bits ignored
      0,                              // no alpha buffer
      0,                              // shift bit ignored
      0,                              // no accumulation buffer
      0, 0, 0, 0,                     // accum bits ignored
      32,                             // 32-bit z-buffer
      0,                              // no stencil buffer
      0,                              // no auxiliary buffer
      PFD_MAIN_PLANE,                 // main layer
      0,                              // reserved
      0, 0, 0                         // layer masks ignored
    };           

    // retrieve handle of the display device context
    m_glDC = GetDC(m_winHandle);

    if (m_glDC == 0)
    {
       return(false);
    }

    // find pixel format supported by the device context. If error return false.
    formatIndex = ChoosePixelFormat(m_glDC, &pfd);
    if (formatIndex == 0)
    {
        return(false);
    }

    // sets the specified device context's pixel format. If error return false
    if (!SetPixelFormat(m_glDC, formatIndex, &pfd))
    {
        return(false);
    }

    formatIndex = GetPixelFormat (m_glDC);
    DescribePixelFormat (m_glDC, formatIndex, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    // openGL is now ready for rendering
    m_glReady = true;

    // if stereo was enabled but can not be displayed, switch over to mono.
    if (((pfd.dwFlags & PFD_STEREO) == 0) && m_stereoEnabled)
    {
        m_stereoEnabled = false;
    }

    // create display context
    m_glContext = wglCreateContext(m_glDC);
    if (m_glContext == 0)
    {
        return(false);
    }

    // enable viewport
    m_enabled = true;

    // return success
    return(true);
}


//===========================================================================
/*!
    Call this method to render the OpenGL world inside the viewport.

    \fn         bool cViewport::render()
    \return     Return \b true if operation succeded.
*/
//===========================================================================
bool cViewport::render()
{
    bool result;

    // render mono mode
    if (m_stereoEnabled)
    {
        result = renderView(CHAI_STEREO_LEFT);
        if (!result) return (false);

        result = renderView(CHAI_STEREO_RIGHT);
        return (result);
    }
    // render stereo mode
    else
    {
        result = renderView(CHAI_MONO);
        return (result);
    }

}


//===========================================================================
/*!
    Renders the OpenGL image in the a_imageIndex related buffer

    \fn         void cViewport::render()
    \param      a_imageIndex  CHAI_MONO, CHAI_STEREO_LEFT or CHAI_STEREO_RIGHT
    \return     Return \b true if operation succeded.
*/
//===========================================================================
bool cViewport::renderView(const int a_imageIndex)
{
    // viewport enabled and camera exists, then render scene
    if (m_glReady && m_enabled && (m_camera != NULL))
    {
        // check if size of display has been modified
        RECT sizeWin;
        if (GetWindowRect(m_winHandle, &sizeWin) == 0) { return (false); }

        unsigned int width   = sizeWin.right  - sizeWin.left;
        unsigned int height  = sizeWin.bottom - sizeWin.top;

        if ((m_width != width) || (m_height != height))
        {
            update();
        }

        // Activate display context
        //
        // Note that in the general case, this is not strictly necessary,
        // but if a user is using multiple viewports, we don't want him
        // to worry about the current rendering context, so we incur a bit
        // of overhead here.
        if (!wglMakeCurrent(m_glDC, m_glContext))
        {
            // return operation failed
            return(false);
        }

        // Set up rendering to the appropriate buffer
        if (a_imageIndex == CHAI_STEREO_RIGHT)
        {
            glDrawBuffer(GL_BACK_RIGHT);
        }
        else if (a_imageIndex == CHAI_STEREO_LEFT)
        {
            glDrawBuffer(GL_BACK_LEFT);
        }
        else
        {
            glDrawBuffer(GL_BACK);
        }
        
        // set viewport size
        glViewport(0, 0, m_width, m_height);

        // render world
        m_camera->renderView(m_width, m_height, a_imageIndex);

        // Swap buffers
        // If stereo is enabled, we only swap after the _right_ image is drawn
        if (m_stereoEnabled == 0 || a_imageIndex == CHAI_STEREO_RIGHT)
        {
            SwapBuffers(m_glDC);
        }

        // deactivate display context
        // wglMakeCurrent(m_glDC, 0);

        // operation succeeded
        return (true);
    }

    else
    {
        // operation failed
        return (false);
    }
}


//===========================================================================
/*!
     Select an object on displayed in the viewport. By calling this method
     the computer searches for the selected object, triangle and exact
     intersection position point on that triangle.
     Use getLastSelectedObject(), getLastSelectedTriangle() and
     getLastSelectedPoint() to extract information about the objects

     \fn        bool cViewport::select(const unsigned int& a_windowPosX,
                const unsigned int& a_windowPosY, const bool a_selectVisibleObjectsOnly)
     \param     a_windowPosX  X coodinate position of mouse click.
     \param     a_windowPosY  Y coodinate position of mouse click.
     \return    Return \b true if an object has been hit.
*/
//===========================================================================
bool cViewport::select(const unsigned int a_windowPosX, const unsigned int a_windowPosY,
                       const bool a_selectVisibleObjectsOnly)
{

    if (m_camera == 0) return false;

    // search for intersection between ray and objects in world
    bool result = m_camera->select( a_windowPosX,
                                    a_windowPosY,
                                    m_width,
                                    m_height,
                                    m_lastSelectedObject,
                                    m_lastSelectedTriangle,
                                    m_lastSelectedPoint,
                                    m_lastSelectedDistance,
                                    a_selectVisibleObjectsOnly
                                );

    // return result. True if and object has been hit, else false.
    return(result);
}


//===========================================================================
/*!
    Set camera. The viewport will now display the image filmed by this
    virtual camera.

    \fn     void cViewport::setCamera(cCamera *a_camera)
    \param  a_camera  Virtual camera in world.
*/
//===========================================================================
void cViewport::setCamera(cCamera *a_camera)
{
    // set camera
    m_camera = a_camera;
}
