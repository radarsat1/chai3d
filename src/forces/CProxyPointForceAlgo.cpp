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
    Constructor of cProxyPointForceAlgo.

    \fn       cProxyPointForceAlgo::cProxyPointForceAlgo()
*/
//===========================================================================
cProxyPointForceAlgo::cProxyPointForceAlgo()
{
    // initialize world pointer
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

    // by default, we do not use dynamic proxy (which handles moving objects)
    m_dynamicProxy = 0;

    // initialize dynamic proxy members
    m_lastObjectGlobalPos.set(0,0,0);
    m_lastObjectGlobalRot.identity();
    m_touchingObject = 0;
}


//===========================================================================
/*!
    Initialize the algorithm, including setting the pointer to the world
    in which the algorithm is to operate, and setting the initial position
    of the device.

    \fn       void cProxyPointForceAlgo::initialize(cWorld* a_world,
              const cVector3d& a_initialPos)
    \param    a_world  Pointer to world in which force algorithm is operating.
    \param    a_initialPos  Initial position of the device.
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

    // set pointer to world in which force algorithm operates
    m_world = a_world;
}


//===========================================================================
/*!
    This method computes the force to add to the device due to any collisions
    with meshes by calling computeNextBestProxyPosition() to update the
    proxy location and then computeForce() to calculate a force vector based
    on the proxy location.

    \fn       cVector3d cProxyPointForceAlgo::computeForces(
              const cVector3d& a_nextDevicePos)
    \param    a_nextDevicePos  Current position of the device.
    \return   Return the force to add to the device due to any collisions
              with meshes.
*/
//===========================================================================
cVector3d cProxyPointForceAlgo::computeForces(const cVector3d& a_nextDevicePos)
{
    // check if world has been defined; if so, compute forces
    if (m_world != NULL)
    {
      // update position of device
      m_deviceGlobalPos =  a_nextDevicePos;

      // if dynamic proxy is enabled, call method to account for moving objects
      if (m_dynamicProxy) correctProxyForObjectMotion();

      // compute next best position of proxy
      computeNextBestProxyPosition();

      // update proxy to next best position
      m_proxyGlobalPos = m_nextBestProxyGlobalPos;

      // compute force vector applied to device
      computeForce();

      // Update "last-state" dynamic contact information
      if (m_touchingObject) updateDynamicContactState();
      
      // return result
      return (m_lastGlobalForce);
    }

    // if no world has been defined in which algorithm operates, there is no force
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
    // local variable declarations
    bool hit;
    cGenericObject* colObject;
    cTriangle* colTriangle;
    cVector3d colPoint, oldColPoint, vDeviceGoal;
    double colDistance=DBL_MAX, cosAngle, distanceTriangleProxy, distProxyGoal;
    cVector3d proxy, goal, colGoal,vProxyGoal, vColProxy, offset;
    cVector3d force, normalForce, tangentialForce;
    cVector3d vertex0, vertex1, vertex2;
    cVector3d normal0, normal1, normal2;
    cGenericObject* parent;

    // read current position of proxy and device; the device position is
    // considered the initial goal position for the proxy
    proxy = m_proxyGlobalPos;
    goal = m_deviceGlobalPos;

    // no contacts with triangles have occured yet
    m_numContacts = 0;

    // if the distance between the proxy and the goal position (device) is
    // smaller than a factor CHAI_SCALE_PROXY of the proxy's radius, we
    // consider that both points are equal, thus the proxy is at the same
    // position as the device, and we can exit
    if ( proxy.distance(goal) < (m_radius / CHAI_SCALE_PROXY))
    {
        m_nextBestProxyGlobalPos = goal;
        if (m_touchingObject)
            m_touchingObject = 0;
        return;
    }

    // the proxy and device are not located at the same position, so we draw a
    // virtual line between both points
    goal.subr(proxy, colGoal);
    colGoal.add(goal);

    // test whether the path along this virtual line from the proxy to its
    // goal is obstructed by a triangle of a mesh by calling the world's
    // collision detection method
    hit = m_world->computeCollisionDetection( proxy, colGoal, colObject,
            colTriangle, colPoint, colDistance, true, 1);

    // if no collision occurs, then we move the proxy to its goal
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

    // a first collision has occured; we first compute the global positions
    // of each vertex of the triangle from the mesh to which the triangle
    // belongs (its parent)
    parent = colTriangle->getParent();
    parent->getGlobalRot().mulr( colTriangle->getVertex0()->getPos(), vertex0);
    vertex0.add(parent->getGlobalPos());
    parent->getGlobalRot().mulr( colTriangle->getVertex1()->getPos(), vertex1);
    vertex1.add(parent->getGlobalPos());
    parent->getGlobalRot().mulr( colTriangle->getVertex2()->getPos(), vertex2);
    vertex2.add(parent->getGlobalPos());

    // next we compute the triangle surface normal in world coordinates
    normal0 = cComputeSurfaceNormal(vertex0, vertex1, vertex2);

    // we compute a vector going from the proxy to the goal
    goal.subr(proxy, vProxyGoal);

    // we measure the distance between the proxy and the goal
    distProxyGoal = vProxyGoal.length();

    // we compute the cosine of the angle between the normal and proxy-goal vector
    vProxyGoal.normalize();
    cosAngle = vProxyGoal.dot(normal0);

    // by knowing the angle between the normal and the collision segment
    // (vector from proxy to goal), we determine the next position of
    // the proxy along the collision segment; we make sure that the
    // proxy's surface (small ball) does not penetrate the triangle
    distanceTriangleProxy = m_radius / cAbs(cosAngle);

    // since a collision has occured with a triangle, we store a pointer to
    // the intersected triangle and increment the number of contacts counter
    m_triangle0 = colTriangle;
    m_numContacts = 1;

    // we now move the proxy toward the triangle; we compute a small vector
    // called offset, which is the projection of the vector between the
    // proxy and the collision point onto the normal of the triangle
    proxy.subr(colPoint, vColProxy);
    offset = cProject(vColProxy, normal0);

    // as long as the length of offset is not within CHAI_SMALL of zero,
    // normalize it and then set its length to the radius of the proxy;
    // also set the variable "proxy" to be a distance equal to the proxy radius
    // from the collision point along the vector between the collision point
    // and the initial proxy position
    if (offset.lengthsq() > CHAI_SMALL)
    {
        offset.normalize();
        offset.mul(m_radius);
        vColProxy.normalize();
        vColProxy.mul(distanceTriangleProxy);
        colPoint.addr(vColProxy, proxy);
    }

    // if the distance between the proxy and the goal position (device) is
    // smaller than a factor CHAI_SCALE_PROXY of the proxy's radius, we
    // consider that both points are equal, thus the proxy is at the same
    // position as the device, and we can exit
    if ( proxy.distance(goal) < (m_radius / CHAI_SCALE_PROXY))
    {
        m_nextBestProxyGlobalPos = goal;
        if (m_touchingObject)
            m_touchingObject = 0;
        return;
    }

    // the proxy is now contrained on a plane; we now calculate the nearest
    // point to the original goal (device position) on this plane; this point
    // is computed by projecting the ideal goal onto the plane defined by the
    // intersected triangle
    goal = cProjectPointOnPlane( m_deviceGlobalPos, vertex0, vertex1, vertex2 );

    // since the proxy has a radius, the new goal position is set above
    // the triangle so that the proxy will not penetrate the triangle
    goal.add(offset);

    // before moving the proxy to this new goal position we need to check
    // if a second triangle could stop the proxy from reaching its new goal; as
    // described earlier we compute a new collision segment going from the
    // proxy towards the goal position
    goal.subr(proxy, colGoal);

    // if the distance between the proxy and the new goal position is
    // smaller than CHAI_SMALL, we consider that both points are equal,
    // and thus the proxy is at the same position as the goal, and we can exit
    if (colGoal.length() < CHAI_SMALL)
    {
        m_nextBestProxyGlobalPos = proxy;
        m_touchingObject = parent;
        return;
    }

    // add goal to get global vector of the new goal
    colGoal.add(goal);

    // calculate static friction
    //
    // calculate force exerted on proxy by user as the stiffness coefficient
    // of the intersected triangle's mesh times the vector between the proxy
    // and the device
    force = cMul(colTriangle->getParent()->m_material.getStiffness(),
            cSub(m_proxyGlobalPos, m_deviceGlobalPos));

    // calculate the normal component of the force
    normalForce = cProject(force, normal0);

    // calculate the tangential component of the force as the total force
    // minus the normal component
    tangentialForce = cSub(force, normalForce);

    // if the magnitude of the tangential force is less than or equal to the
    // static friction coefficient times the magnitude of the normal force, then
    // the user's position is in the friction cone of the surface, and the
    // proxy should not be moved (set next best position to current position)
    if (tangentialForce.length() <=
        colTriangle->getParent()->m_material.getStaticFriction()*normalForce.length())
    {
        m_nextBestProxyGlobalPos = m_proxyGlobalPos;
        m_touchingObject = parent;
        return;
    }

    // save the first collision point (offset by the proxy radius above the
    // triangle) since we're about to overwrite it in the second call to the
    // collision detection method
    oldColPoint = cAdd(colPoint, offset);

    // test whether the path along the virtual line between the updated proxy
    // to its new goal is obstructed by a triangle of a mesh by calling the
    // world's collision detection method again
    hit = m_world->computeCollisionDetection( proxy, colGoal, colObject,
            colTriangle, colPoint, colDistance, true, 2);

    // if no collision occurs, we move the proxy to its goal, unless
    // dynamic friction prevents us from doing so
    if (hit == false)
    {
        // calculate dynamic friction
        //
        // calculate the depth of penetration as the vector between the device
        // and the proxy collision point, dotted with the first triangle normal
        double penetrationDepth = cDot(cSub(m_deviceGlobalPos, oldColPoint),
                normal0);

        // calculate the friction radius as the absolute value of the penetration
        // depth times the coefficient of dynamic friction of the intersected
        // triangle's mesh
        double frictionRadius = fabs(penetrationDepth *
                colTriangle->getParent()->m_material.getDynamicFriction());

        // calculate the distance between the proxy position and the current
        // goal position (the device position projected onto the constraint
        // plane of the first intersected triangle, offset above by proxy radius)
        double r = cSub(m_proxyGlobalPos, goal).length();

        // if this distance is smaller than CHAI_SMALL, we consider that both
        // points are equal, and thus the proxy is at the same position as the
        // goal, and we can exit
        if (r < CHAI_SMALL)
        {
            m_nextBestProxyGlobalPos = goal;
        }

        // if the proxy is outside the friction cone, update is position to
        // be on the perimeter of the friction cone
        else if (r > frictionRadius)
        {
            m_nextBestProxyGlobalPos = cAdd(goal,
                              cMul(frictionRadius/r,cSub(oldColPoint, goal)));                    
        }

        // otherwise, if the proxy is inside the friction cone, the proxy
        // should not be moved (set next best position to current position)
        else
        {
            m_nextBestProxyGlobalPos = m_proxyGlobalPos;
        }

        // set current touching pointer (for dynamic proxy) and return
        m_touchingObject = parent;
        return;
    }

    //-----------------------------------------------------------------------
    // SECOND COLLISION:
    //-----------------------------------------------------------------------

    // a second collision has occured; we first compute the global positions
    // of each vertex of the triangle from the mesh to which the triangle
    // belongs (its parent)
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

    // by knowing the angle between the normal and the collision segment
    // (vector from proxy to goal), we determine the next position of
    // the proxy along the collision segment; we make sure that the
    // proxy's surface (small ball) does not penetrate the triangle
    distanceTriangleProxy = m_radius / cAbs(cosAngle);

    // we test if the collision point is located between the proxy and the goal;
    // if not, we simply move the proxy to its goal position
    if ( (colDistance - distanceTriangleProxy) > distProxyGoal)
    {
        m_nextBestProxyGlobalPos = goal;
        if (m_touchingObject)
            m_touchingObject = 0;
        return;
    }

    // since a collision has occured with a triangle, we store a pointer to
    // the intersected triangle and increment the number of contacts counter
    m_triangle1 = colTriangle;
    m_numContacts = 2;

    // we move the proxy to its new position
    vProxyGoal.mul(distanceTriangleProxy);
    colPoint.subr(vProxyGoal, proxy);

    // if the distance between the proxy and the new goal position is
    // smaller than a factor CHAI_SCALE_PROXY of the proxy's radius, we
    // consider that both point are equal, and thus the proxy is at the same
    // position than the device
    if ( proxy.distance(goal) < (m_radius / CHAI_SCALE_PROXY))
    {
        m_nextBestProxyGlobalPos = goal;
        m_touchingObject = parent;
        return;
    }

    // the proxy is now contrained by two triangles and can only move along
    // a virtual line; we now calculate the nearest point to the original
    // goal (device position) along this line by projecting the ideal
    // goal onto the line
    //
    // the line is expressed by the cross product of both surface normals
    cVector3d line;
    normal0.crossr(normal1, line);

    // compute the projection of device position (goal) onto line; this gives
    // us the new goal position
    goal = cProjectPointOnLine( m_deviceGlobalPos, proxy, line);

    // before moving the proxy to this new goal position we need to check
    // if a third triangle could stop the proxy from reaching its new goal; as
    // described earlier we compute a new collision segment going from the
    // proxy towards the goal position
    goal.subr(proxy, colGoal);

    // if the distance between the proxy and the new goal position is
    // smaller than CHAI_SMALL, we consider that both points are equal,
    // and thus the proxy is at the same position as the goal, and we can exit
    if (colGoal.length() < CHAI_SMALL)
    {
        m_nextBestProxyGlobalPos = proxy;
        m_touchingObject = parent;
        return;
    }

    // add goal to get global vector of the new goal
    colGoal.add(goal);

    // calculate static friction
    //
    // calculate force exerted on proxy by user as the stiffness coefficient
    // of the intersected triangle's mesh times the vector between the proxy
    // and the device
    force = cMul(colTriangle->getParent()->m_material.getStiffness(),
             cSub(m_proxyGlobalPos, m_deviceGlobalPos));

    // calculate the normal component of the force
    normalForce = cProject(force, normal1);

    // calculate the tangential component of the force as the total force
    // minus the normal component
    tangentialForce = cSub(force, normalForce);

    // if the magnitude of the tangential force is less than or equal to the
    // static friction coefficient times the magnitude of the normal force, then
    // the user's position is in the friction cone of the surface, and the
    // proxy should not be moved (set next best position to current position)
    if (tangentialForce.length() <=
        colTriangle->getParent()->m_material.getStaticFriction()*normalForce.length())
    {
        m_nextBestProxyGlobalPos = m_proxyGlobalPos;
        m_touchingObject = parent;
        return;
    }

    // save the second collision point (offset by the proxy radius above the
    // triangle) since we're about to overwrite it in the third call to the
    // collision detection method
    oldColPoint = cAdd(colPoint, offset);

    // test whether the path along the virtual line between the updated proxy
    // to its new goal is obstructed by a triangle of a mesh by calling the
    // world's collision detection method again
    hit = m_world->computeCollisionDetection( proxy, colGoal, colObject,
            colTriangle, colPoint, colDistance, true, 3);

    // if no collision occurs, we move the proxy to its goal, unless
    // dynamic friction prevents us from doing so
    if (hit == false)
    {
        // calculate dynamic friction
        //
        // calculate the depth of penetration as the vector between the device
        // and the proxy collision point, dotted with the first triangle normal
        double penetrationDepth = cDot(cSub(m_deviceGlobalPos, oldColPoint),
                normal1);

        // calculate the friction radius as the absolute value of the penetration
        // depth times the coefficient of dynamic friction of the intersected
        // triangle's mesh
        double frictionRadius = fabs(penetrationDepth *
                colTriangle->getParent()->m_material.getDynamicFriction());

        // calculate the distance between the proxy position and the current
        // goal position (the device position projected onto the constraint
        // planes of the first two intersected triangles)
        double r = cSub(m_proxyGlobalPos, goal).length();

        // if this distance is smaller than CHAI_SMALL, we consider that both
        // points are equal, and thus the proxy is at the same position as the
        // goal, and we can exit
        if (r < CHAI_SMALL)
        {
            m_nextBestProxyGlobalPos = goal;
        }

        // if the proxy is outside the friction cone, update is position to
        // be on the perimeter of the friction cone
        else if (r > frictionRadius)
        {
            m_nextBestProxyGlobalPos = cAdd(goal,
                    cMul(frictionRadius/r,cSub(oldColPoint, goal)));
        }

        // otherwise, if the proxy is inside the friction cone, the proxy
        // should not be moved (set next best position to current position)
        else
        {
            m_nextBestProxyGlobalPos = m_proxyGlobalPos;
        }

        // set current touching pointer (for dynamic proxy) and return
        m_touchingObject = parent;
        return;
    }

    //-----------------------------------------------------------------------
    // THIRD COLLISION:
    //-----------------------------------------------------------------------

    // a third collision has occured; we first compute the global positions
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

    // by knowing the angle between the normal and the collision segment
    // (vector from proxy to goal), we determine the next position of
    // the proxy along the collision segment; we make sure that the
    // proxy's surface (small ball) does not penetrate the triangle
    distanceTriangleProxy = m_radius / cAbs(cosAngle);

    // we test if the collision point is located between the proxy and the goal;
    // if not we simply move the proxy to the goal position
    if ( (colDistance - distanceTriangleProxy) > distProxyGoal)
    {
        m_nextBestProxyGlobalPos = goal;
        if (m_touchingObject)
          m_touchingObject = 0;
        return;
    }

    // the proxy is now constrained to a single point just above the third triangle
    m_triangle2 = colTriangle;
    m_numContacts = 3;

    // calculate static friction
    //
    // calculate force exerted on proxy by user as the stiffness coefficient
    // of the intersected triangle's mesh times the vector between the proxy
    // and the device
    force = cMul(colTriangle->getParent()->m_material.getStiffness(),
             cSub(m_proxyGlobalPos, m_deviceGlobalPos));

    // calculate the normal component of the force
    normalForce = cProject(force, normal2);

    // calculate the tangential component of the force as the total force
    // minus the normal component
    tangentialForce = cSub(force, normalForce);

    // if the magnitude of the tangential force is less than or equal to the
    // static friction coefficient times the magnitude of the normal force, then
    // the user's position is in the friction cone of the surface, and the
    // proxy should not be moved (set next best position to current position)
    if (tangentialForce.length() <=
            colTriangle->getParent()->m_material.getStaticFriction()*normalForce.length())
    {
        m_nextBestProxyGlobalPos = m_proxyGlobalPos;
        m_touchingObject = parent;
        return;
    }

    // by knowing the angle between the normal and the collision segment
    // (vector from proxy to goal), we determine the next position of
    // the proxy along the collision segment; we make sure that the
    // proxy's surface (small ball) does not penetrate the triangle
    vProxyGoal.mul(m_radius/(cAbs(cosAngle)));
    colPoint.subr(vProxyGoal, m_nextBestProxyGlobalPos);

    // set the last touched object pointer for the dynamic proxy
    m_touchingObject = parent;
}


//===========================================================================
/*!
    This method uses the information computed earlier in
    computeNextProxyPosition() to determine the force to apply to the device.
    The function computes a force proportional to the distance between the
    positions of the proxy and the device and scaled by the average
    stiffness of each contact triangle.

    \fn       void cProxyPointForceAlgo::computeForce()
*/
//===========================================================================
void cProxyPointForceAlgo::computeForce()
{
    // the stifness is computed by computing the average stiffness
    // for each triangle
    //
    // surface stiffness
    double stiffness;

    // if there are no contacts between proxy and environment, no force is applied
    if (m_numContacts == 0)
    {
        m_lastGlobalForce.zero();
        return;
    }

    // if there is one contact point, the stiffness is equal to the stiffness
    // of the one intersected triangle's mesh
    else if (m_numContacts == 1)
    {
        stiffness = ( m_triangle0->getParent()->m_material.getStiffness() );
    }

    // if there are two contact points, the stiffness is the average of the
    // stiffnesses of the two intersected triangles' meshes
    else if (m_numContacts == 2)
    {
        stiffness = ( m_triangle0->getParent()->m_material.getStiffness() +
                      m_triangle1->getParent()->m_material.getStiffness() ) / 2.0;
    }

    // if there are three contact points, the stiffness is the average of the
    // stiffnesses of the three intersected triangles' meshes
    else if (m_numContacts == 3)
    {
        stiffness = ( m_triangle0->getParent()->m_material.getStiffness() +
                      m_triangle1->getParent()->m_material.getStiffness() +
                      m_triangle2->getParent()->m_material.getStiffness() ) / 3.0;
    }

    // compute the force by modelling a spring between the proxy and the device
    cVector3d force;
    m_proxyGlobalPos.subr(m_deviceGlobalPos, force);
    force.mul(stiffness);
    m_lastGlobalForce = force;
}


//===========================================================================
/*!
    Return the number of current contacts, and the associated triangles in the
    output parameters.

    \fn       void cProxyPointForceAlgo::getContacts(cTriangle*& a_t0,
              cTriangle*& a_t1, cTriangle*& a_t2);
    \param    a_t0  Returns pointer to first contact triangle.
    \param    a_t1  Returns pointer to second contact triangle.
    \param    a_t2  Returns pointer to third contact triangle.
    \return   Return the number of current contacts.
*/
//===========================================================================
unsigned int cProxyPointForceAlgo::getContacts(cTriangle*& a_t0, cTriangle*& a_t1,
                                               cTriangle*& a_t2)
{
    // set the triangle pointers to the contact triangles
    a_t0 = m_triangle0;
    a_t1 = m_triangle1;
    a_t2 = m_triangle2;

    // return the number of triangle intersections
    return m_numContacts;
}


//===========================================================================
/*!
    This method sets the dynamic proxy state to reflect new contact
    information.

    \fn       void cProxyPointForceAlgo::updateDynamicContactState();
*/
//===========================================================================
void cProxyPointForceAlgo::updateDynamicContactState()
{
    // if the proxy is not currently in contact with any object, no update
    // is needed
    if (m_touchingObject == 0) return;

    // update the position and rotation of the object currently being touched
    m_lastObjectGlobalPos = m_touchingObject->getGlobalPos();
    m_lastObjectGlobalRot = m_touchingObject->getGlobalRot();
}


//===========================================================================
/*!
    This method lets the proxy move along with the object it's touching, if
    the object has moved since the previous proxy iteration.

    \fn       void cProxyPointForceAlgo::correctProxyForObjectMotion();
*/
//===========================================================================
void cProxyPointForceAlgo::correctProxyForObjectMotion()
{
    // if the proxy is not currently in contact with any object, no update
    // is needed
    if(m_touchingObject == 0) return;

    // start with the non-dynamic proxy position
    cVector3d newGlobalProxy = m_proxyGlobalPos;

    // get the position and rotation of the object contacted in the last iteration
    cVector3d lastGlobalPos;
    getContactObjectLastGlobalPos(lastGlobalPos);
    cMatrix3d lastGlobalRot;
    getContactObjectLastGlobalRot(lastGlobalRot);

    // combine the rotation and translation into one matrix
    cMatrix3d lastGlobalRotT;
    lastGlobalRot.transr(lastGlobalRotT);

    // convert the last segment point A (proxy position) into the
    // _current_ reference frame of the object
    newGlobalProxy.sub(lastGlobalPos);

    // apply rotation as necessary
    lastGlobalRotT.mul(newGlobalProxy);
    m_touchingObject->getGlobalRot().mul(newGlobalProxy);

    // convert to global coordinates
    newGlobalProxy.add(m_touchingObject->getGlobalPos());

    // this ends up being the proxy in global coordinates
    m_proxyGlobalPos = newGlobalProxy;
}
