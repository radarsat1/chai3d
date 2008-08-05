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
#ifndef CProxyPointForceAlgoH
#define CProxyPointForceAlgoH
//---------------------------------------------------------------------------
#include "CVector3d.h"
#include "CMatrix3d.h"
#include "CGenericCollision.h"
#include "CGenericPointForceAlgo.h"
#include <map>
//---------------------------------------------------------------------------
class cWorld;
//---------------------------------------------------------------------------

// The position/rotation state of an object
struct positionData
{
    cVector3d pos;
    cMatrix3d rot;
};


// A mapping from meshes to their positions at the previous proxy iteration
typedef std::map<cGenericObject*,positionData> meshPositionMap;

//===========================================================================
/*!
      \class    cProxyPointForceAlgo
      \brief    Implements the finger-proxy algorithm for computing
                interaction forces between a point force device and meshes.
*/
//===========================================================================
class cProxyPointForceAlgo : public cGenericPointForceAlgo
{
  public:
    // CONSTRUCTOR:
    //! Constructor of cProxyPointForceAlgo.
    cProxyPointForceAlgo();

    // METHODS - BASIC PROXY:
    //! Initialize the algorithm.
    void initialize(cWorld* a_world, const cVector3d& a_initialPos);
    //! Calculate interaction forces between device and meshes.
    cVector3d computeForces(const cVector3d& a_nextDevicePos);

    // METHODS - GETTER AND SETTER FUNCTIONS:
    //! Set radius of proxy.
    void setProxyRadius(const double a_radius) { m_radius = a_radius; }
    //! Read radius of proxy.
    virtual inline double getProxyRadius() const { return (m_radius); }
    //! Get last computed position of proxy in world coordinates.
    virtual inline cVector3d getProxyGlobalPosition() const { return (m_proxyGlobalPos); }
    //! Get last specified position of device in world coordinates.
    virtual inline cVector3d getDeviceGlobalPosition() const { return (m_deviceGlobalPos); }
    //! Get last computed global force vector
    virtual inline cVector3d getLastGlobalForce() const { return (m_lastGlobalForce); }

    // METHODS - DYNAMIC PROXY (TO HANDLE MOVING OBJECTS):
    //! Return the number of current contacts and the associated triangles
    virtual inline unsigned int getContacts(cTriangle*& a_t0, cTriangle*& a_t1,
            cTriangle*& a_t2);
    //! Return a pointer to the object with which device is currently in contact.
    virtual inline const cGenericObject* getContactObject() { return m_touchingObject; }
    //! Return global position of object with which device last contacted.
    virtual inline void getContactObjectLastGlobalPos(cVector3d& a_pos)
        { a_pos = m_lastObjectGlobalPos; }
    //! Return global rotation of object with which device last contacted.
    virtual inline void getContactObjectLastGlobalRot(cMatrix3d& a_rot)
        { a_rot = m_lastObjectGlobalRot; }
    //! Set dynamic proxy flag, (if on, all contacts are computed in object-local space).
    void enableDynamicProxy(int a_enable) { m_dynamicProxy = a_enable; }
    //! Return whether the dynamic proxy flag is on.
    int getDynamicProxyEnabled() { return m_dynamicProxy; }

  protected:
    // METHODS - BASIC PROXY:
    //! Compute the next goal position of the proxy.
    virtual void computeNextBestProxyPosition();
    //! Compute force to apply to device.
    virtual void computeForce();

    // METHODS - DYNAMIC PROXY (TO HANDLE MOVING OBJECTS):
    //! Let proxy move along with the object it's touching, if object has moved.
    void correctProxyForObjectMotion();
    //! Set the dynamic proxy state to reflect new contact information.
    void updateDynamicContactState();

    // MEMBERS - POSTIONS AND ROTATIONS:
    //! Global position of the proxy.
    cVector3d m_proxyGlobalPos;
    //! Global position of device.
    cVector3d m_deviceGlobalPos;
    //! Last computed force (in global coordinate frame).
    cVector3d m_lastGlobalForce;
    //! Next best position for the proxy (in global coordinate frame).
    cVector3d m_nextBestProxyGlobalPos;

    // MEMBERS - POINTERS TO INTERSECTED OBJECTS:
    //! Number of contacts between proxy and triangles (0, 1, 2 or 3).
    unsigned int m_numContacts;
    //! Pointer to first triangle with which proxy is in contact.
    cTriangle* m_triangle0;
    //! Pointer to second triangle with which proxy is in contact.
    cTriangle* m_triangle1;
    //! Pointer to third triangle with which proxy is in contact.
    cTriangle* m_triangle2;
    //! Pointer to the object (if any) with which the proxy is currently in contact.
    cGenericObject* m_touchingObject;

    // MEMBERS - DISPLAY PROPERTIES:
    //! Color of rendered proxy.
    cColorf m_colorProxy;
    //! Color of rendered line.
    cColorf m_colorLine;
    //! Radius used to display device position.
    double m_radius;
    //! Radius used to display the proxy position.
    double m_displayRadius;

    // MEMBERS - DYNAMIC PROXY (TO HANDLE MOVING OBJECTS):
    //! Dynamic proxy flag (if on, all contacts are computed in object-local space).
    int m_dynamicProxy;
    //! Mapping from meshes to position/rotation info for handling moving objects.
    meshPositionMap lastIterationPositions;
    //! Dynamic proxy tracks last position of object it's touching at each call.
    cVector3d m_lastObjectGlobalPos;
    //! Dynamic proxy tracks last rotation of object it's touching at each call.
    cMatrix3d m_lastObjectGlobalRot;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

