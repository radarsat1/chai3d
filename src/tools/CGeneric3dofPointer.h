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
    \author:    Federico Barbagli
    \version    1.1
    \date       01/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CGeneric3dofPointerH
#define CGeneric3dofPointerH
//---------------------------------------------------------------------------
#include "CGenericTool3dof.h"
#include "CGenericDevice.h"
#include "CColor.h"
#include "CProxyPointForceAlgo.h"
#include "CPotentialFieldForceAlgo.h"
//---------------------------------------------------------------------------

typedef enum {
  RENDER_PROXY=0,RENDER_DEVICE,RENDER_PROXY_AND_DEVICE
} proxy_render_modes;
   
//===========================================================================
/*!
      \class      cGeneric3dofPointer
      \brief      cGeneric3dofPointer provides a cursor like tool which is
                  graphically rendered by a small sphere illustrating the tip
                  of the device. The tools interacts with the environment
                  by using the finger-proxy algorithm
*/
//===========================================================================
class cGeneric3dofPointer : public cGenericTool3dof
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cGeneric3dofPointer.
    cGeneric3dofPointer(cWorld* a_world);
    //! Destructor of cGeneric3dofPointer.
    virtual ~cGeneric3dofPointer();

    // METHODS:

    // Graphics

    //! Render the object in OpenGL 
    virtual void render(const int a_renderMode=0);
    //! toggle on/off the tool frame
    virtual void visualizeFrames(bool a_showToolFrame) { m_showToolFrame = a_showToolFrame; }
    //! set the visual settings of the tool frame
    virtual void setToolFrame(bool a_showToolFrame, double a_toolFrameSize);
    //! Control proxy rendering options
    virtual void setRenderingMode(proxy_render_modes render_mode) { m_render_mode = render_mode; }

    // Initialization / shutdown

    //! Start communication with the device connected to the tool (0 indicates success)
    virtual int start();
    //! Stop communication with the device connected to the tool (0 indicates success)
    virtual int stop();
    //! Initialize encoders on device connected to the tool (0 indicates success)
    virtual int initialize();  
    //! Toggle forces on
    virtual int setForcesON();
    //! Toggle forces off
    virtual int setForcesOFF();

    // Data transfer

    //! Update position and orientation of the device.
    virtual void updatePose();
    //! Compute interaction forces with environment.
    virtual void computeForces();
    //! Apply latest computed forces to device.
    virtual void applyForces();

    // Miscellaneous 

    //! Set radius of pointer
    virtual void setRadius(double a_radius);
    //! Set haptic device driver.
    virtual void setDevice(cGenericDevice *a_device);
    //! Get information about the proxy directly
    virtual cProxyPointForceAlgo* getProxy() { return &m_proxyPointForceAlgo; }

    // MEMBERS:
    //! Color of sphere representing position of device.
    cColorf m_colorDevice;
    //! Color of sphere representing position of tool (proxy).
    cColorf m_colorProxy;
    //! Color of sphere representing position of tool (proxy) when switch in ON.
    cColorf m_colorProxyButtonOn;
    //! Color of line connecting proxy and device position together
    cColorf m_colorLine;

    //! Orientation of wrist in local coordinates of device
    cMatrix3d m_deviceLocalRot;
    //! Orientation of wrist in global coordinates of device
    cMatrix3d m_deviceGlobalRot;

    //! Normally this class waits for a very small force before initializing forces
    //! to avoid initial "jerks" (a safety feature); you can bypass that requirement
    //! with this variable
    bool m_waitForSmallForce;

    //! Mesh force algorithm.
    cProxyPointForceAlgo m_proxyPointForceAlgo;

  protected:
    // MEMBERS:
    //! Radius of sphere representing position of pointer.
    double m_displayRadius;
    //! haptic device driver.
    cGenericDevice *m_device;
    //! World in which tool is interacting
    cWorld* m_world;
    //! Implicit function based object
    cPotentialFieldForceAlgo m_potentialFieldForceAlgo;
    //! flag for frame visualization of the proxy
    bool m_showToolFrame;
    //! size of the frame visualization of the proxy
    double m_toolFrameSize;
    //! Should we render the device position, the proxy position, or both?
    proxy_render_modes m_render_mode;


    //! this flag records whether the user has enabled forces
    bool m_forceON;
    //! flag to avoid initial bumps in force (has the user sent a _small_ force yet?)
    bool m_forceStarted;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

