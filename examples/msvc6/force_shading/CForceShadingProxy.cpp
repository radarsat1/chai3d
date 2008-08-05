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
    \author:    Chris Sewell
    \version    1.0
    \date       05/2006
*/
//===========================================================================

#include "stdafx.h"
#include "CForceShadingProxy.h"
#include "CWorld.h"
#include <float.h>


//===========================================================================
/*!
    Compute next proxy position using Ruspini's two-pass algorithm, as 
    described in Section 4 of "The Haptic Display of Complex Graphical
    Environments" by Diego Ruspini, Krasimir Kolarov, and Oussama Khatib,
    SIGGRAPH 1997, pp. 345-352.

    \fn     void cForceShadingProxy::computeNextBestProxyPosition(cVector3d a_goal)
    \param  a_goal  The goal towards which to move the proxy, subject to constraints
*/
//===========================================================================
void cForceShadingProxy::computeNextBestProxyPosition(cVector3d a_goal)
{
    // If we are not using force shading, just call the regular
    // computeNextBestProxyPosition function with the device position
    // as the goal
    if (m_useShading == 0)
    {
        cProxyPointForceAlgo::computeNextBestProxyPosition(m_deviceGlobalPos);
        return;
    }
    
    // Otherwise, perform the first pass of the force shading algorithm:
    // search for a collision along the line segment between the current
    // proxy and device positions; if there is one, project the device
    // position onto the plane defined by the collision point and the
    // interpolated normal at that point (and then project back onto the
    // regular constraint plane if above it); then, for the second pass,
    // call the regular computeNextBestProxyPosition function with this
    // projected point as the goal

    // local variable declarations

    // Each of these variables is re-used in each of three collision passes...

    // Did we hit a triangle this pass?
    int hit = 0;

    //! Initialize normal and tangential forces.
    m_normalForce.set(0,0,0);
    m_tangentialForce.set(0,0,0);

    // What object, triangle, and point did we collide with?
    cGenericObject* colObject = 0;
    cTriangle* colTriangle = 0;
    cVector3d colPoint;
    
    // This is the parent object of the triangle we hit, which should really
    // be the same as colObject...
    cGenericObject* parent = 0;

    // How far away was the collision from the current proxy position?
    double colDistance=DBL_MAX;

    // The three vertices of the triangle we most recently collided with...
    cVector3d vertex0, vertex1, vertex2;

    // The current positions of the proxy and the goal
    cVector3d proxy, goal;

    // temporary variable to used to describe a segment path from the proxy to its
    // next goal position
    cVector3d segmentPointB;

    // A vector from the proxy to the goal
    cVector3d vProxyGoal;

    // A vector from the most recent collision point to the proxy
    cVector3d vColProxy;

    // The offset we'll apply to each goal position to "push it away" from
    // the collision plane, to account for the proxy's radius.
    cVector3d goalOffset;

    // The goal we try to move the proxy to _after_ finding a constraint plane
    // (i.e. after the proxy->goal segment is found to hit an object)
    cVector3d colGoal;

    // The force exerted on the proxy by the user
    cVector3d force;

    // The surface normal of each of the three triangles we come in contact with.
    // These variables are not re-used across collisions.
    cVector3d normal0, normal1, normal2;

    // Used for maintaining a minimum distance between the proxy and all
    // constraint planes...
    double cosAngle, distanceTriangleProxy;

    // A vector from the device to the proxy; used to orient surface normals
    cVector3d vDeviceProxy;

    // Read the current position of proxy and device; the device position
    // is considered the initial goal position for the proxy.
    proxy = m_proxyGlobalPos;
    goal = m_deviceGlobalPos;

    // Store the previous number of contacts for debugging...
    int previousNumberOfContacts = m_numContacts;

    // No contacts with triangles have occurred yet.
    m_numContacts = 0;

    // If the distance between the proxy and the goal position (device) is
    // very small then we can be considered done.
    if (goalAchieved(proxy,goal))
    {
        m_nextBestProxyGlobalPos = proxy;
        m_numContacts = 0;
        m_touchingObject = 0;
        return;
    }

    // Test whether the path from the proxy to the goal is obstructed.
    // For this we create a segment that goes from the proxy position to
    // the goal position plus a little extra to take into account the
    // physical radius of the proxy.
    segmentPointB = goal;
    offsetGoalPosition(segmentPointB,proxy);
    
    colTriangle = NULL;
    hit = m_world->computeCollisionDetection( proxy, segmentPointB, colObject,
            colTriangle, colPoint, colDistance, 0, 1);

    if (colTriangle == 0)
        hit = 0;
    else
        hit = 1;

    // If no collision occurs, then we move the proxy to its goal, and we're done
    if (hit == false)
    {
        m_nextBestProxyGlobalPos = goal;
        m_numContacts = 0;
        m_touchingObject = 0;
        m_slipping = 0;
        return;
    }

    m_touchingPoint = colPoint;

    //-----------------------------------------------------------------------
    // FIRST COLLISION:
    //-----------------------------------------------------------------------

    // Since a collision has occurred with a triangle, we store a pointer to
    // the intersected triangle and increment the number of contacts.
    m_triangle0 = colTriangle;
    m_numContacts = 1;

    // A first collision has occurred; we first compute the global positions
    // of each vertex of the triangle we hit...
    parent = colTriangle->getParent();

    cMatrix3d rot = parent->getGlobalRot();
    cVector3d pos = parent->getGlobalPos();

    rot.mulr(colTriangle->getVertex0()->getPos(), vertex0);
    vertex0.add(pos);
    rot.mulr(colTriangle->getVertex1()->getPos(), vertex1);
    vertex1.add(pos);
    rot.mulr(colTriangle->getVertex2()->getPos(), vertex2);
    vertex2.add(pos);

    // Compute the triangle surface normal in world coordinates
    normal0 = cComputeSurfaceNormal(vertex0, vertex1, vertex2);

    // Align the surface normal to point away from the device
    vDeviceProxy = cSub(proxy,m_deviceGlobalPos);
    vDeviceProxy.normalize();
    if (cDot(normal0,vDeviceProxy) < 0) normal0.negate();

    // Compute a vector from the proxy to the goal (device)
    goal.subr(proxy, vProxyGoal);

    // We want the center of the proxy to move as far toward the triangle as it can,
    // but we want it to stop when the _sphere_ representing the proxy hits the
    // triangle.  We want to compute how far the proxy center will have to
    // be pushed _away_ from the collision point - along the vector from the proxy
    // to the goal - to keep a distance m_radius between the proxy center and the
    // triangle.
    //
    // So we compute the cosine of the angle between the normal and proxy-goal vector...
    vProxyGoal.normalize();
    cosAngle = vProxyGoal.dot(normal0);

    // Now we compute how far away from the collision point - _backwards_
    // along vProxyGoal - we have to put the proxy to keep it from penetrating
    // the triangle.
    //
    // If only ASCII art were a little more expressive...
    distanceTriangleProxy = m_radius / cAbs(cosAngle);

    // We compute the projection of the vector between the proxy and the collision
    // point onto the normal of the triangle.  This is the direction in which
    // we'll move the _goal_ to "push it away" from the triangle (to account for
    // the radius of the proxy).

    // A vector from the collision point to the proxy...
    proxy.subr(colPoint, vColProxy);

    // Move the proxy to the collision point, minus the distance along the
    // movement vector that we computed above.
    //
    // Note that we're adjusting the 'proxy' variable, which is just a local
    // copy of the proxy position.  We still might decide not to move the
    // 'real' proxy due to friction.
    vColProxy.normalize();
    vColProxy.mul(distanceTriangleProxy);
    colPoint.addr(vColProxy, proxy);

    // If the distance between the proxy and the goal position (device) is
    // very small then we can be considered done.
    if (goalAchieved(proxy,goal))
    {
        m_nextBestProxyGlobalPos = proxy;
        m_touchingObject = parent;
        return;
    }

    // For a smooth interpolation for this example, we assume here that the mesh is a collection 
    // of rectangular faces (each defined with two triangles) that are vertical in the Y direction...
    
    // Find which two vertices have the closest Y values (for each triangle, two vertices should
    // have exactly the same Y values, within round-off error, given our assumption).
    float min_y = CHAI_LARGE;
    int min_i = -1;
    int min_j = -1;
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            if (i != j)
            {
                float cur_y = fabs(colTriangle->getVertex(i)->getPos().y - colTriangle->getVertex(j)->getPos().y);
                if (cur_y < min_y)
                {
                    min_y = cur_y;
                    min_i = i;
                    min_j = j;
                }
            }
        }
    }

    // Project the collision point onto the line between these two vertices (in global coordinates).
    cVector3d vertices[3];
    vertices[0] = vertex0;
    vertices[1] = vertex1;
    vertices[2] = vertex2;
    cVector3d proj_pt = cProjectPointOnLine(colPoint, vertices[min_i], cSub(vertices[min_j], vertices[min_i]));

    // Find the normals, in global coordinates, of each vertex of the collision triangle
    cVector3d norms[3];
    rot.mulr(colTriangle->getVertex0()->getNormal(), norms[0]);
    norms[0].add(pos);
    rot.mulr(colTriangle->getVertex1()->getNormal(), norms[1]);
    norms[1].add(pos);
    rot.mulr(colTriangle->getVertex2()->getNormal(), norms[2]);
    norms[2].add(pos);

    // Then add the normals of the two vertices with the same Y value, weighted inversely proportional
    // to their distances from  the projection of the collision point, to get the interpolated normal
    float dist1 = cDistance(proj_pt, vertices[min_i]);
    float dist2 = cDistance(proj_pt, vertices[min_j]);
    float wt1 = dist2/(dist1+dist2);
    float wt2 = dist1/(dist1+dist2);
    cVector3d interp_norm = cAdd(cMul(wt1,norms[min_i]), cMul(wt2,norms[min_j]));
    interp_norm.normalize();
    
    // The proxy is now constrained on a plane; we now calculate the nearest
    // point to the original goal (device position) on this plane; this point
    // is computed by projecting the ideal goal onto the plane defined by the
    // intersection point and the *interpolated* normal at this point
    goal = cProjectPointOnPlane(m_deviceGlobalPos, colPoint, interp_norm );

    // If the projected goal lies above the real constraint plane, project
    // it back onto this plane (to maintain surface contact for effects such
    // as friction)
    if (cDot(goal, normal0) != cDot(m_deviceGlobalPos, normal0))
    goal = cProjectPointOnPlane(goal, colPoint, normal0);

    // Since the proxy has a radius, the new goal position is offset to be
    // on the same side of the plane as the proxy, so the proxy will not
    // penetrate the triangle.
    goalOffset = interp_norm; 
    goalOffset.mul(m_radius);
    goal.add(goalOffset);   
    
    // Save the first constraint triangle already found
    cTriangle* saved_triangle_0 = m_triangle0;
    
    // Perform the second pass by just calling the regular
    // computeNextBestProxyPosition(...) function with the projected
    // point as the goal
    cProxyPointForceAlgo::computeNextBestProxyPosition(goal);

    // Shift constraint triangles, which will be reassigned from scratch in
    // the regular cProxyPointForceAlgo::computeNextBestProxyPosition(...)
    // to include the first one already found in first pass
    m_numContacts++;
    m_triangle2 = m_triangle1;
    m_triangle1 = m_triangle0;
    m_triangle0 = saved_triangle_0;
}
