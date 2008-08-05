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
#include "CGeneric3dofPointer.h"
//---------------------------------------------------------------------------

//==========================================================================
/*!
      Constructor of cGeneric3dofPointer.

      \fn       cGeneric3dofPointer::cGeneric3dofPointer(cWorld* a_world)
      \param    a_world  World in which the tool will operate.
*/
//===========================================================================
cGeneric3dofPointer::cGeneric3dofPointer(cWorld* a_world)
{
    // set world
    m_world = a_world;

    // set a default device for the moment
    m_device = new cGenericDevice();

    // set default values
    m_colorDevice.set(1.0f, 0.2f, 0.0);
    m_colorProxyButtonOn.set(1.0f, 0.4f, 0.0);
    m_colorProxy.set(0.8f, 0.6f, 0.0);
    m_colorLine.set(0.7f, 0.7f, 0.7f);
    m_displayRadius = 0.1f;

    // set radius of proxy 10% wider than device to avoid rendering artifacts
    // when both points are at same position
    m_proxyPointForceAlgo.setProxyRadius(1.1 * m_displayRadius);

    // toggle frames off
    m_visualizeFrames = false;

    // forces are ON at first
    m_forceON = true;
    m_forceStarted = false;

    m_render_mode = RENDER_PROXY_AND_DEVICE;
}


//==========================================================================
/*!
      Destructor of cGeneric3dofPointer.

      \fn       cGeneric3dofPointer::~cGeneric3dofPointer()
*/
//===========================================================================
cGeneric3dofPointer::~cGeneric3dofPointer()
{
    // check if device is available
    if (m_device == NULL) { return; }

    // close device driver
    m_device->close();

    delete m_device;
}


//==========================================================================
/*!
    Define a haptic device driver for this tool.

    \fn       cGeneric3dofPointer::void setDevice(cGenericDevice *a_device);

*/
//===========================================================================
void cGeneric3dofPointer::setDevice(cGenericDevice *a_device)
{
    m_device = a_device;
}


//==========================================================================
/*!
    Initialize device

    \fn       void cGeneric3dofPointer::initialize()

*/
//===========================================================================
void cGeneric3dofPointer::initialize()
{
    // check if device is available
    if (m_device == NULL) { return; }

    // initialize device
    m_device->initialize();
    m_device->open();
    updatePose();
    m_device->close();

    // initialize force algorithms
    m_proxyPointForceAlgo.initialize(m_world, m_deviceGlobalPos);
    m_potentialFieldForceAlgo.initialize(m_world, m_deviceGlobalPos);
}


//==========================================================================
/*!
      Reset the tool.

      \fn       void cGeneric3dofPointer::start()
*/
//===========================================================================
void cGeneric3dofPointer::start()
{
    // check if device is available
    if (m_device == NULL) { return; }

    // open connection to device
    m_device->open();
}


//==========================================================================
/*!
      Stop system. Apply zero force to device

      \fn       void cGeneric3dofPointer::stop()
*/
//===========================================================================
void cGeneric3dofPointer::stop()
{
    // check if device is available
    if (m_device == NULL) { return; }

    // stop the device
    m_device->close();
}


//==========================================================================
/*!
      Update position of pointer and orientation of wrist.

      \fn       void cGeneric3dofPointer::updatePose()
*/
//===========================================================================
void cGeneric3dofPointer::updatePose()
{
    cVector3d pos;

    // check if device is available
    if (m_device == NULL) { return; }

    // read local position of device
    int result = m_device->command(CHAI_CMD_GET_POS_NORM_3D, &pos);
    if (result != 0) { return; }

    // scale  position from device into virtual workspace of tool
    m_deviceLocalPos.set( m_halfWorkspaceAxisX * pos.x,
                          m_halfWorkspaceAxisY * pos.y,
                          m_halfWorkspaceAxisZ * pos.z);

    // update global position of tool
    cVector3d tPos;
    m_globalRot.mulr(m_deviceLocalPos, tPos);
    tPos.addr(m_globalPos, m_deviceGlobalPos);

    // read orientation of wrist
    m_device->command(CHAI_CMD_GET_ROT_MATRIX, &m_deviceLocalRot);

    // update global orientation of tool
    m_deviceLocalRot.mulr(m_globalRot, m_deviceGlobalRot);

    // read switch
    m_device->command(CHAI_CMD_GET_SWITCH_0, &m_button);

    // read velocity of the device in global coordinates
    m_device->command(CHAI_CMD_GET_VEL_3D, &m_deviceGlobalVel);
}


//===========================================================================
/*!
      Compute the interaction forces between the tool and the virtual
      object inside the virtual world.

      \fn       void cGeneric3dofPointer::computeForces()
*/
//===========================================================================
void cGeneric3dofPointer::computeForces()
{
    // init temp variable
    cVector3d force;
    force.zero();

    // compute forces in world coodinates
    force.add(m_proxyPointForceAlgo.computeForces(m_deviceGlobalPos));
    force.add(m_potentialFieldForceAlgo.computeForces(m_deviceGlobalPos));

    // copy result
    m_lastComputedGlobalForce.copyfrom(force);
}


//==========================================================================
/*!
      Apply the latest computed force to the device.

      \fn       void cGeneric3dofPointer::applyForces()
*/
//===========================================================================
void cGeneric3dofPointer::applyForces()
{
    // check if device is available
    if (m_device == NULL) { return; }

    // convert force into device local coordinates
    cMatrix3d tRot;
    m_globalRot.transr(tRot);
    tRot.mulr(m_lastComputedGlobalForce, m_lastComputedLocalForce);

    if ((!m_forceStarted) && (m_lastComputedLocalForce.lengthsq() <0.000001))
        m_forceStarted = true;

    // send force to device
    if ((m_forceON) && (m_forceStarted))
     m_device->command(CHAI_CMD_SET_FORCE_3D, &m_lastComputedLocalForce);
    else
    {
        cVector3d ZeroForce = cVector3d(0.0, 0.0, 0.0);
        m_device->command(CHAI_CMD_SET_FORCE_3D, &ZeroForce);
    }
}


//==========================================================================
/*!
      Render in OpenGL current tool.

      \fn       void cGeneric3dofPointer::render()
      \param    a_renderMode  rendering mode.
*/
//===========================================================================
void cGeneric3dofPointer::render(const int a_renderMode)
{
    if (a_renderMode & CHAI_RENDER_NON_TRANSPARENT)
    {

        // render small sphere representing tip of tool
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        // OpenGL matrix
        cMatrixGL frameGL;

        // compute local position of proxy
        cVector3d proxyLocalPos = m_proxyPointForceAlgo.getProxyGlobalPosition();
        proxyLocalPos.sub(m_globalPos);
        cMatrix3d tRot;
        m_globalRot.transr(tRot);
        tRot.mul(proxyLocalPos);

        if (m_render_mode != RENDER_DEVICE)
        {
          // render proxy
          if (m_button)
          {
              glColor4fv(m_colorProxyButtonOn.pColor());
          }
          else
          {
              glColor4fv(m_colorProxy.pColor());
          }

          frameGL.set(proxyLocalPos);
          frameGL.glMatrixPushMultiply();
              cDrawSphere(m_displayRadius, 16, 16);
          frameGL.glMatrixPop();
        }

        if (m_render_mode != RENDER_PROXY)
        {
          // render device position
          glColor4fv(m_colorDevice.pColor());
          frameGL.set(m_deviceLocalPos);
          frameGL.glMatrixPushMultiply();
              cDrawSphere(0.95 * m_displayRadius, 16, 16);
          frameGL.glMatrixPop();

          if (m_visualizeFrames)
          {
              // render device orientation
              frameGL.set(m_deviceLocalPos, m_deviceLocalRot);
              frameGL.glMatrixPushMultiply();
                  cDrawFrame(0.28);
              frameGL.glMatrixPop();
          }
        }

        if (m_render_mode == RENDER_PROXY_AND_DEVICE)
        {
          // render line between device and proxy
          glDisable(GL_LIGHTING);
          glLineWidth(1.0);
          glColor4fv(m_colorLine.pColor());
          glBegin(GL_LINES);
              glVertex3d(m_deviceLocalPos.x, m_deviceLocalPos.y, m_deviceLocalPos.z);
              glVertex3d(proxyLocalPos.x, proxyLocalPos.y, proxyLocalPos.z);
          glEnd();
          glEnable(GL_LIGHTING);
        }
    }
}


//==========================================================================
/*!
      Set the radius of the proxy. the value passed as parameter corresponds
      to the size of the sphere which is rendered graphically. The physical
      size of the prozy, one which collides with the triangles is set to
      1/100th of original value.

      \fn       void cGeneric3dofPointer::setRadius(double a_radius)
      \param    a_radius  radius of pointer.
*/
//===========================================================================
void cGeneric3dofPointer::setRadius(double a_radius)
{
    // update radius of device
    m_displayRadius = a_radius;

    // set radius of proxy 10% wider than device to avoid rendering artifacts
    // when both points are at same position
    m_proxyPointForceAlgo.setProxyRadius(a_radius / 100.0);
}


//==========================================================================
/*!
      Toggles on and off the visualization of a reference frame
      located on the tool's point.

      \fn       void cGeneric3dofPointer::visualizeFrames(bool val)
      \param    val flag that turns frames on/off
*/
//===========================================================================
void cGeneric3dofPointer::visualizeFrames(bool val)
{
    m_visualizeFrames = val;
}



//==========================================================================
/*!
    Turns forces ON

    \fn     void cGeneric3dofPointer::setForcesON()

*/
//===========================================================================
void cGeneric3dofPointer::setForcesON()
{
    if (!m_forceON)
    {
          m_forceStarted = false;
          m_forceON = true;
    }
}


//==========================================================================
/*!
    Turns forces OFF

    \fn       void cGeneric3dofPointer::setForcesOFF()
*/
//===========================================================================
void cGeneric3dofPointer::setForcesOFF()
{
    m_forceON = false;
}
