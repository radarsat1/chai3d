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
#include "CProxyPointForceAlgo.h"
//---------------------------------------------------------------------------
#include "CMesh.h"
#include "CWorld.h"
#include <float.h>
//---------------------------------------------------------------------------
const double CHAI_SCALE_PROXY = 100.0;
const double CHAI_SCALE_COLLISION_RAY = 5.0;
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cForceAlgoProxy.

    \fn         cProxyPointForceAlgo::cProxyPointForceAlgo(cWorld* a_world)
    \param      a_world  Parent world in which algorithm operates.
*/
//===========================================================================
cProxyPointForceAlgo::cProxyPointForceAlgo()
{
    // set world
    m_world = NULL;

    // no contacts yet between proxy and environment
    m_numContacts = 0;

    // initialize device and proxy positions
    m_deviceGlobalPos.zero();
    m_proxyGlobalPos.zero();
    m_lastGlobalForce.zero();

    // set default colors
    m_colorProxy.set(1.0f, 0.7f, 0.0, 1.0f);
    m_colorLine.set(0.5f, 0.5f, 0.5f);
    m_radius = 0.10f;

    m_dynamic_proxy = 0;

    m_last_object_global_pos.set(0,0,0);
    m_last_object_global_rot.identity();

    m_touchingObject = 0;
}


//===========================================================================
/*!
      Initialize the algorithm, including a call to the initialization method
      of the collision detection algorithm for each mesh.

      \fn       void cProxyPointForceAlgo::initialize(const cWorld* a_world,
                const cVector3d& a_initialPos)
      \param    a_world  World in which force algorithm is operating.
      \param    a_initialPos   Initial position of device
*/
//===========================================================================
void cProxyPointForceAlgo::initialize(cWorld* a_world, const cVector3d& a_initialPos)
{
    // no contacts yet between proxy and environment
    m_numContacts = 0;

    // initialize device and proxy positions
    m_deviceGlobalPos = a_initialPos;
    m_proxyGlobalPos = a_initialPos;
    m_lastGlobalForce.zero();

    // set world in which force algorithm operates
    m_world = a_world;
}


//===========================================================================
/*!
      \fn         cVector3d cVector3d computeForces(const cVector3d& a_nextDevicePos)
      \param      a_nextDevicePos  Current position of the device.
      \return     Returns the force to add to the device due to any collisions
                  with meshes.
*/
//===========================================================================
cVector3d cProxyPointForceAlgo::computeForces(const cVector3d& a_nextDevicePos)
{
    // check if world has been defined. If yes, compute forces.
    if (m_world != NULL)
    {
      // update position of device
      m_deviceGlobalPos =  a_nextDevicePos;

      if (m_dynamic_proxy) correct_proxy_for_object_motion();

      // compute next best position of proxy
      computeNextBestProxyPosition();

      // update proxy to next best position
      m_proxyGlobalPos = m_nextBestProxyGlobalPos;

      // compute force vector applied to device
      computeForce();

      // Update "last-state" dynamic contact information
      if (m_touchingObject) update_dynamic_contact_state();
      
      // return result
      return (m_lastGlobalForce);
    }

    // no world has been defined in which algorithm operates
    else
    {
        return (cVector3d(0.0, 0.0, 0.0));
    }
}


//===========================================================================
/*!
      Given the new position of the device and considering the current
      position of the proxy, this function attempts to move the proxy towards
      the device position (the goal).  If its path is blocked by an obstacle
      (e.g., a triangle in a mesh), the proxy is moved to this intersection
      point and a new goal is calculated as the closest point to the original
      goal in the half-plane above the intersection triangle.
      The process is repeated if necessary, bringing the proxy to its
      final location.

      \fn   void cProxyPointForceAlgo::computeNextBestProxyPosition()
*/
//===========================================================================
void cProxyPointForceAlgo::computeNextBestProxyPosition()
{
    
    bool hit;
    cGenericObject* colObject;
    cTriangle* colTriangle;
    cVector3d colPoint, oldColPoint, vDeviceGoal;
    double colDistance=DBL_MAX, cosAngle, distanceTriangleProxy, distProxyGoal;
    cVector3d proxy, goal, colGoal,vProxyGoal, vColProxy, offset, f, fn, ft;
    cVector3d vertex0, vertex1, vertex2;
    cVector3d normal0, normal1, normal2;
    cGenericObject* parent;

    // read current position of proxy and device. the device position is
    // considered as the goal position for the proxy
    proxy = m_proxyGlobalPos;
    goal = m_deviceGlobalPos;

    // no contacts with triangles have occured yet.
    m_numContacts = 0;

    // If the distance between the proxy and the goal position (device) is
    // smaller than a factor CHAI_SCALE_PROXY of the proxy's radius, we
    // consider that both points are equal, thus the proxy is at the same
    // position as the device, and we can exit.
    if ( proxy.distance(goal) < (m_radius / CHAI_SCALE_PROXY))
    {
        m_nextBestProxyGlobalPos = goal;
        if (m_touchingObject)
          m_touchingObject = 0;
        return;
    }

    // the proxy and device are not located at the same position. We draw a virtual
    // line between both points and search for an obstacle (triangle).
    //
    // Since the radius of the proxy is not zero and that there is a small risk
    // that the proxy's surface could collide with a triangle that its center
    // avoids, we extend the length of the collision ray.
    goal.subr(proxy, colGoal);

    // CHECK
    // colGoal.normalize();
    // colGoal.mul(CHAI_SCALE_COLLISION_RAY); // * m_radius);
    colGoal.add(goal);

    hit = m_world->computeCollisionDetection( proxy,
                                              colGoal,
                                              colObject,
                                              colTriangle,
                                              colPoint,
                                              colDistance,
                                              true, 1);

    // If no collision occurs, then we move the proxy to its goal
    if (hit == false)
    {
        m_nextBestProxyGlobalPos = goal;
        if (m_touchingObject)
          m_touchingObject = 0;
        return;
    }    

    //-----------------------------------------------------------------------
    // FIRST COLLISION:
    //-----------------------------------------------------------------------

    // A first collision has occured; we first compute the global positions
    // of each vertex of the triangle.
    parent = colTriangle->getParent();
    parent->getGlobalRot().mulr( colTriangle->getVertex0()->getPos(), vertex0);
    vertex0.add(parent->getGlobalPos());
    parent->getGlobalRot().mulr( colTriangle->getVertex1()->getPos(), vertex1);
    vertex1.add(parent->getGlobalPos());
    parent->getGlobalRot().mulr( colTriangle->getVertex2()->getPos(), vertex2);
    vertex2.add(parent->getGlobalPos());

    // Next we compute the triangle surface normal in world coordinates
    normal0 = cComputeSurfaceNormal(vertex0, vertex1, vertex2);

    // We compute a vector going from the proxy to the goal
    goal.subr(proxy, vProxyGoal);

    // We measure the distance between the proxy and the goal
    distProxyGoal = vProxyGoal.length();

    // We compute the cosine of the angle between the normal and proxy-goal vector
    vProxyGoal.normalize();
    cosAngle = vProxyGoal.dot(normal0);

    // By knowing the angle between the normal and the collision segment
    // (vector from proxy to goal), we determine the next position of
    // the proxy along the collision segment. We make sure that the
    // proxy's surface (small ball) does not penetrate the triangle 
    distanceTriangleProxy = m_radius / cAbs(cosAngle);

    // We test if the collision point is located between the proxy and the goal.
    //
    // If not, we simply move the proxy to its goal position.
    // 
    // (The collision hit can occur beyond the goal position; this results from
    // the fact that the collision ray is slightly longer than the original
    // proxy-goal segment.)
    /*
    if ( (colDistance - distanceTriangleProxy) > distProxyGoal)
    {
        m_nextBestProxyGlobalPos = goal;
        m_touchingObject = parent;
        return;
    }
    */

    // a collision has occured with a triangle. we store its information.
    m_triangle0 = colTriangle;
    m_numContacts = 1;

    // We now move the proxy toward the triangle. We compute a small vector
    // called \e offset, which is perpendicular to the triangle and goes
    // from the triangle surface to the center of the proxy. The length of
    // this vector simply corresponds to the proxy's radius.
    proxy.subr(colPoint, vColProxy);
    offset = cProject(vColProxy, normal0);

    if (offset.lengthsq() > CHAI_SMALL)
    {
      offset.normalize();
      offset.mul(m_radius);
      vColProxy.normalize();
      vColProxy.mul(distanceTriangleProxy);
      colPoint.addr(vColProxy, proxy);
    }

    // If the distance between the proxy and the goal position (device) is
    // smaller than a factor CHAI_SCALE_PROXY of the proxy's radius, we
    // consider that both points are equal, thus the proxy is at the same
    // position as the device, and we can exit.
    if ( proxy.distance(goal) < (m_radius / CHAI_SCALE_PROXY))
    {
        m_nextBestProxyGlobalPos = goal;
        if (m_touchingObject)
          m_touchingObject = 0;
        return;
    }

    // the proxy is now contrained on a plane. we now calculate the nearest
    // point to the original goal (device position). This point is computed
    // by projecting the ideal goal onto the plane defined by the triangle
    goal = cProjectPointOnPlane( m_deviceGlobalPos,
                                 vertex0, vertex1, vertex2 );

    // Since the proxy has a radius, the new goal position is set above
    // the triangle so that the proxy will not penetrate the triangle
    goal.add(offset);

    // Before moving the proxy to this new goal position we need to check
    // if a second triangle could stop the proxy reaching its new goal. As
    // described earlier we compute a new collision segment going from the
    // proxy towards the goal position.
    goal.subr(proxy, colGoal);

    if (colGoal.length() < 0.0000001)
    {
        m_nextBestProxyGlobalPos = proxy;
        m_touchingObject = parent;
        return;
    }

    // CHECK
    // colGoal.normalize();
    // colGoal.mul(CHAI_SCALE_COLLISION_RAY * m_radius);
    colGoal.add(goal);

    // static friction
    f = cMul(colTriangle->getParent()->m_material.getStiffness(),
             cSub(m_proxyGlobalPos, m_deviceGlobalPos));
    fn = cProject(f, normal0);
    ft = cSub(f, fn);
    if (ft.length() <= colTriangle->getParent()->m_material.getStaticFriction()*fn.length())
    {
      m_nextBestProxyGlobalPos = m_proxyGlobalPos;
      m_touchingObject = parent;
      return;
    }
    oldColPoint = cAdd(colPoint, offset);

    // search for an eventual collision between the collision segment and the
    // virtual world
    hit = m_world->computeCollisionDetection( proxy,
                                              colGoal,
                                              colObject,
                                              colTriangle,
                                              colPoint,
                                              colDistance,
                                              true, 2);

    // if no collision occurs,  we move the proxy to its goal, unless
    // dynamic friction prevents us from doing so...
    if (hit == false)
    {
        // dynamic friction
        double frictionRadius = fabs(cDot(cSub(m_deviceGlobalPos, oldColPoint),
            normal0)*colTriangle->getParent()->m_material.getDynamicFriction());
        double r = cSub(m_proxyGlobalPos, goal).length();
        if (r < 0.0000001)
        {
          m_nextBestProxyGlobalPos = goal;                    
        }
        else if (r > frictionRadius)
        {
          m_nextBestProxyGlobalPos = cAdd(goal,
                              cMul(frictionRadius/r,cSub(oldColPoint, goal)));                    
        }
        else
        {
          m_nextBestProxyGlobalPos = m_proxyGlobalPos;
        }
        m_touchingObject = parent;
        return;
    }

    //-----------------------------------------------------------------------
    // SECOND COLLISION:
    //-----------------------------------------------------------------------

    // a second collision has occured. we first compute the global positions
    // of each vertex of the triangle.
    parent = colTriangle->getParent();
    parent->getGlobalRot().mulr( colTriangle->getVertex0()->getPos(), vertex0);
    vertex0.add(parent->getGlobalPos());
    parent->getGlobalRot().mulr( colTriangle->getVertex1()->getPos(), vertex1);
    vertex1.add(parent->getGlobalPos());
    parent->getGlobalRot().mulr( colTriangle->getVertex2()->getPos(), vertex2);
    vertex2.add(parent->getGlobalPos());

    // next we compute the triangle surface normal in world coordinates
    normal1 = cComputeSurfaceNormal(vertex0, vertex1, vertex2);

    // we compute a vector from the proxy to the goal
    goal.subr(proxy, vProxyGoal);

    // we measure the distance between the proxy and the goal
    distProxyGoal = vProxyGoal.length();

    // we compute the cosine of the angle between normal and proxy-goal vector
    vProxyGoal.normalize();
    cosAngle = vProxyGoal.dot(normal1);
    distanceTriangleProxy = m_radius / cAbs(cosAngle);

    // We test if the collision point is located between the proxy and the goal.
    //
    // If not we simply move the proxy to its goal position.
    if ( (colDistance - distanceTriangleProxy) > distProxyGoal)
    {
        m_nextBestProxyGlobalPos = goal;
        if (m_touchingObject)
          m_touchingObject = 0;
        return;
    }

    // a collision has occured with a triangle. we store its information.
    m_triangle1 = colTriangle;
    m_numContacts = 2;

    // we move the proxy to its new position
    vProxyGoal.mul(distanceTriangleProxy);
    colPoint.subr(vProxyGoal, proxy);

    // if the distance between the proxy and the new goal position is
    // smaller than a factor CHAI_SCALE_PROXY of the proxy's radius, we
    // consider that both point are equal, thus the proxy is at the same
    // position than the device.
    if ( proxy.distance(goal) < (m_radius / CHAI_SCALE_PROXY))
    {
        m_nextBestProxyGlobalPos = goal;
        m_touchingObject = parent;
        return;
    }

    // the proxy is now contrained by two triangles and can only move allong
    // a virtual line . we now calculate the nearest point to the original
    // goal (device position). This point is computed by project the ideal
    // goal onto the line.

    // the line is expressed by the cross product of both surface normals
    cVector3d line;
    normal0.crossr(normal1, line);

    // compute the projection of device position (goal) onto line. this gives
    // us the new goal position
    goal = cProjectPointOnLine( m_deviceGlobalPos,
                                proxy,
                                line);

    // Before moving the proxy to this new goal position we need to check
    // if a third triangle could stop the proxy reaching it. We
    // compute a collision segment going from the proxy to the goal.
    goal.subr(proxy, colGoal);

    if (colGoal.length() < 0.0000001)
    {
        m_nextBestProxyGlobalPos = proxy;
        m_touchingObject = parent;
        return;
    }

    // CHECK
    // colGoal.normalize();
    // colGoal.mul(CHAI_SCALE_COLLISION_RAY * m_radius);
    colGoal.add(goal);

    // static friction
    f = cMul(colTriangle->getParent()->m_material.getStiffness(),
             cSub(m_proxyGlobalPos, m_deviceGlobalPos));
    fn = cProject(f, normal1);
    ft = cSub(f, fn);
    if (ft.length() <= colTriangle->getParent()->m_material.getStaticFriction()*fn.length())
    {
      m_nextBestProxyGlobalPos = m_proxyGlobalPos;
      m_touchingObject = parent;
      return;
    }
    oldColPoint = cAdd(colPoint, offset);

    // search for an eventual collision between the collision segment and the
    // virtual world.
    hit = m_world->computeCollisionDetection( proxy,
                                              colGoal,
                                              colObject,
                                              colTriangle,
                                              colPoint,
                                              colDistance,
                                              true,3);

    // if no collision occurs we move the proxy to the new goal position,
    // unless dynamic friction prevents us from doing so
    if (hit == false)
    {
        // dynamic friction
        double frictionRadius = fabs(cDot(cSub(m_deviceGlobalPos, oldColPoint),
               normal1)*colTriangle->getParent()->m_material.getDynamicFriction());
        double r = cSub(m_proxyGlobalPos, goal).length();
        if (r < 0.0000001)
        {
          m_nextBestProxyGlobalPos = goal;                    
        }
        else if (r > frictionRadius)
        {
          m_nextBestProxyGlobalPos = cAdd(goal,
                              cMul(frictionRadius/r,cSub(oldColPoint, goal)));                    
        }
        else
        {
          m_nextBestProxyGlobalPos = m_proxyGlobalPos;
        }
        m_touchingObject = parent;
        return;
    }

    //-----------------------------------------------------------------------
    // THIRD COLLISION:
    //-----------------------------------------------------------------------

    // a third collision has occured. we first compute the global positions
    // of each vertex of the triangle.
    parent = colTriangle->getParent();
    parent->getGlobalRot().mulr( colTriangle->getVertex0()->getPos(), vertex0);
    vertex0.add(parent->getGlobalPos());
    parent->getGlobalRot().mulr( colTriangle->getVertex1()->getPos(), vertex1);
    vertex1.add(parent->getGlobalPos());
    parent->getGlobalRot().mulr( colTriangle->getVertex2()->getPos(), vertex2);
    vertex2.add(parent->getGlobalPos());

    // next we compute the triangle surface normal in world coordinates
    normal2 = cComputeSurfaceNormal(vertex0, vertex1, vertex2);

    // we compute a vector from the proxy to the goal point
    goal.subr(proxy, vProxyGoal);

    // we measure the distance between the proxy and the goal
    distProxyGoal = vProxyGoal.length();

    // we compute the cosine of the angle between normal and proxy-goal vector
    vProxyGoal.normalize();
    cosAngle = vProxyGoal.dot(normal2);
    distanceTriangleProxy = m_radius / cAbs(cosAngle);

    // we test if the collision point is located between the proxy and the goal.
    // if not we simply move the proxy to the goal position.
    if ( (colDistance - distanceTriangleProxy) > distProxyGoal)
    {
        m_nextBestProxyGlobalPos = goal;
        if (m_touchingObject)
          m_touchingObject = 0;
        return;
    }

    // the proxy is moved just above the third triangle
    m_triangle2 = colTriangle;
    m_numContacts = 3;

    // static friction
    f = cMul(colTriangle->getParent()->m_material.getStiffness(),
             cSub(m_proxyGlobalPos, m_deviceGlobalPos));
    fn = cProject(f, normal2);
    ft = cSub(f, fn);
    if (ft.length() <= colTriangle->getParent()->m_material.getStaticFriction()*fn.length())
    {
      m_nextBestProxyGlobalPos = m_proxyGlobalPos;
      m_touchingObject = parent;
      return;
    }

    vProxyGoal.mul(m_radius/(cAbs(cosAngle)));
    colPoint.subr(vProxyGoal, m_nextBestProxyGlobalPos);
    m_touchingObject = parent;
}


//===========================================================================
/*!
      This method uses the information computed earlier in
      computeNextProxyPosition() to determine the force to apply to the device.
      The function takes into account the different stiffness properties
      of each contact triangle and compute a force proportional to the
      distance between positions of the proxy and the device.

      \fn   void cProxyPointForceAlgo::computeForce()
*/
//===========================================================================
void cProxyPointForceAlgo::computeForce()
{
    // The stifness is computed by computing the average stiffness
    // for each triangle.

    // surface stiffness.
    double stiffness;

    // no contacts between proxy and environment, no force is applied
    if (m_numContacts == 0)
    {
        m_lastGlobalForce.zero();
        return;
    }

    // one contact point
    else if (m_numContacts == 1)
    {
        stiffness = ( m_triangle0->getParent()->m_material.getStiffness() );
    }

    // two contact points
    else if (m_numContacts == 2)
    {
        stiffness = ( m_triangle0->getParent()->m_material.getStiffness() +
                      m_triangle1->getParent()->m_material.getStiffness() ) / 2.0;
    }

    // three contact points
    else if (m_numContacts == 3)
    {
        stiffness = ( m_triangle0->getParent()->m_material.getStiffness() +
                      m_triangle1->getParent()->m_material.getStiffness() +
                      m_triangle2->getParent()->m_material.getStiffness() ) / 3.0;
    }

    // compute force by modelling a spring between proxy and device.
    cVector3d force;
    m_proxyGlobalPos.subr(m_deviceGlobalPos, force);
    force.mul(stiffness);
    m_lastGlobalForce = force;
}


//===========================================================================
/*!
    Returns the number of current contacts and the associated triangles     
    \fn void cProxyPointForceAlgo::getContacts(cTriangle*& t0, cTriangle*& t1, cTriangle*& t2);
*/
//===========================================================================
unsigned int cProxyPointForceAlgo::getContacts(cTriangle*& t0, cTriangle*& t1, cTriangle*& t2)
{
    t0 = m_triangle0;
    t1 = m_triangle1;
    t2 = m_triangle2;
    return m_numContacts;
}


//===========================================================================
/*!
    Lets the dynamic proxy state reflect new contact information
    \fn void cProxyPointForceAlgo::update_dynamic_contact_state();
*/
//===========================================================================
void cProxyPointForceAlgo::update_dynamic_contact_state()
{

  if (m_touchingObject == 0) return;

  m_last_object_global_pos = m_touchingObject->getGlobalPos();
  m_last_object_global_rot = m_touchingObject->getGlobalRot();
  
}


//===========================================================================
/*!
    Lets the proxy move along with the object it's touching, if
    the object has moved since the previous proxy iteration.

    \fn void cProxyPointForceAlgo::correct_proxy_for_object_motion();
*/
//===========================================================================
void cProxyPointForceAlgo::correct_proxy_for_object_motion()
{
    if(m_touchingObject == 0) return;

    cVector3d newGlobalProxy = m_proxyGlobalPos;

    cVector3d lastGlobalPos;
    getContactObjectLastGlobalPos(lastGlobalPos);

    cMatrix3d lastGlobalRot;
    getContactObjectLastGlobalRot(lastGlobalRot);

    cMatrix3d lastGlobalRotT;
    lastGlobalRot.transr(lastGlobalRotT);

    // Convert the last segment point A (proxy position) into the
    // _current_ reference frame of the object
      newGlobalProxy.sub(lastGlobalPos);

    // Apply rotation as necessary...
      lastGlobalRotT.mul(newGlobalProxy);
      m_touchingObject->getGlobalRot().mul(newGlobalProxy);

    // Apply translation as necessary

    // Convert to global coordinates
      newGlobalProxy.add(m_touchingObject->getGlobalPos());

    // This ends up being the proxy in global coordinates...
      m_proxyGlobalPos = newGlobalProxy;

    /*
    // And this ends up being the proxy in the local coordinates of the _world_...
      localSegmentPointA = a_segmentPointA;
      localSegmentPointA.sub(m_localPos);
      cMatrix3d transLocalRot;
      m_localRot.transr(transLocalRot);
      transLocalRot.mul(localSegmentPointA);
    */
}
