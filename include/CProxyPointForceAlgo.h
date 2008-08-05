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
struct position_data {
  cVector3d pos;
  cMatrix3d rot;
};


// A mapping from mesh to their positions at the previous
// proxy iteration
typedef std::map<cGenericObject*,position_data> mesh_position_map;

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
    //! Constructor of cProxy.
    cProxyPointForceAlgo();

    //! A mapping from meshes in the world to position/rotation information
    //!
    //! To be used for handling detection of collisions with moving objects.
    mesh_position_map last_iteration_positions;    

    // METHODS:
    //! Initialize the algorithm.
    void initialize(cWorld* a_world, const cVector3d& a_initialPos);
    //! Calculate interaction forces between device and mesh
    cVector3d computeForces(const cVector3d& a_nextDevicePos);
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

    //! Returns the number of current contacts and the associated triangles
    virtual inline unsigned int getContacts(cTriangle*& t0, cTriangle*& t1, cTriangle*& t2);
    
    virtual inline const cGenericObject* getContactObject() { return m_touchingObject; }

    virtual inline void getContactObjectLastGlobalPos(cVector3d& pos) { pos = m_last_object_global_pos; }
    virtual inline void getContactObjectLastGlobalRot(cMatrix3d& rot) { rot = m_last_object_global_rot; }

    //! If "dynamic proxy'ing" is enabled, all proxy contacts are computed
    //! in object-local space, so moving objects can be handled correctly.
    void enableDynamicProxy(int enable) { m_dynamic_proxy = enable; }
    int getDynamicProxyEnabled() { return m_dynamic_proxy; }

  protected:
    // METHODS:
    //! Compute the next goal position of the proxy.
    virtual void computeNextBestProxyPosition();
    //! Compute force applied to device.
    virtual void computeForce();

    // MEMBERS:
    //! Position of the proxy.
    cVector3d m_proxyGlobalPos;
    //! Position of device.
    cVector3d m_deviceGlobalPos;
    //! Last computed force.
    cVector3d m_lastGlobalForce;
    //! Next best position for the proxy.
    cVector3d m_nextBestProxyGlobalPos;

    //! Number of contacts between proxy and triangles (0, 1, 2 or 3)
    unsigned int m_numContacts;
    //! Triangle 0 with wich proxy is in contact
    cTriangle* m_triangle0;
    //! Triangle 1 with wich proxy is in contact
    cTriangle* m_triangle1;
    //! Triangle 2 with wich proxy is in contact
    cTriangle* m_triangle2;

    //! The object (if any) with which the proxy is currently in contact
   	cGenericObject *m_touchingObject;
	  
    //! color of rendered proxy
    cColorf m_colorProxy;
    //! color of rendered line.
    cColorf m_colorLine;
    //! radius used to display device position
    double m_radius;
    //! radius used to display the proxy position
    double m_displayRadius;

    //! If "dynamic proxy'ing" is enabled, all proxy contacts are computed
    //! in object-local space, so moving objects can be handled correctly.
    int m_dynamic_proxy;

    //! The "dynamic proxy" needs to track the last position of the object
    //! it's touching at each call to the proxy
    cVector3d m_last_object_global_pos;
    cMatrix3d m_last_object_global_rot;

    //! Lets the proxy move along with the object it's touching, if
    //! the object has moved since the previous proxy iteration.
    void correct_proxy_for_object_motion();

    //! Lets the dynamic proxy state reflect new contact information
    void update_dynamic_contact_state();
    
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

