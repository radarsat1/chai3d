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
                  of the device. The tools interacts with the enviornment
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
    //! Device initialization. Forces are NOT started.
    virtual void initialize();
    //! Start communication with device.  Forces are NOT started.
    virtual void start();
    //! Stop communication with device.
    virtual void stop();
    //! Toggles forces on
    virtual void setForcesON();
    //! Toggles forces off
    virtual void setForcesOFF();
    
    //! Update position and orientation of the device.
    virtual void updatePose();
    //! Compute interaction forces with environment.
    virtual void computeForces();
    
    //! Apply latest computed forces to device.
    virtual void applyForces();
    //! Render the object in OpenGL 
    virtual void render(const int a_renderMode=0);
    //! Set radius of pointer
    virtual void setRadius(double a_radius);
    //! Set haptic device driver.
    virtual void setDevice(cGenericDevice *a_device);
    //! toggle on/off frames on the tool position following its orientation
    virtual void visualizeFrames(bool val);
    //! Get information about the proxy directly
    virtual cProxyPointForceAlgo* getProxy() { return &m_proxyPointForceAlgo; }

    //! Control proxy rendering options
    virtual void setRenderingMode(proxy_render_modes render_mode) { m_render_mode = render_mode; }

    // MEMBERS:
    //! Color of sphere representing position of device.
    cColorf m_colorDevice;
    //! Color of sphere representing position of tool (proxy).
    cColorf m_colorProxy;
    //! Color of sphere representing position of tool (proxy) when switch in ON.
    cColorf m_colorProxyButtonOn;
    //! Color of line connecting proxy and device position together
    cColorf m_colorLine;

  protected:
    // MEMBERS:
    //! Radius of sphere representing position of pointer.
    double m_displayRadius;
    //! haptic device driver.
    cGenericDevice *m_device;
    //! World in which tool is interacting
    cWorld* m_world;
    //! Mesh force algorithm.
    cProxyPointForceAlgo m_proxyPointForceAlgo;
    //! Implicit function based object
    cPotentialFieldForceAlgo m_potentialFieldForceAlgo;
    //! Orientation of wrist in local coodinates of device
    cMatrix3d m_deviceLocalRot;
    //! Orientation of wrist in global coodinates of device
    cMatrix3d m_deviceGlobalRot;
    //! flag for frames visualization
    bool m_visualizeFrames;
    //! flag to turn forces on or off
    bool m_forceON;
    //! flag to avoid initial bumps in force
    bool m_forceStarted;
    //! Should we render the device position, the proxy position, or both?
    proxy_render_modes m_render_mode;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------


