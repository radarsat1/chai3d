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
#include "CGenericObject.h"
#include "CGenericCollision.h"
#include "CProxyPointForceAlgo.h"
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cGenericObject.

    \fn     cGenericObject::cGenericObject()
*/
//===========================================================================
cGenericObject::cGenericObject()
{
    // initialize parent.
    m_parent = NULL;

    // initialize local position and orientation
    m_localPos.set(0.0, 0.0 ,0.0);
    m_localRot.identity();

    // initialize global position and orientation
    m_globalPos.set(0.0, 0.0 ,0.0);
    m_globalRot.identity();

    // initialize openGL matrix with position vector and orientation matrix
    m_frameGL.set(m_globalPos, m_globalRot);

    // set display status of object
    m_show = true;

    // set showframe status and frame size
    m_showFrame = false;
    m_frameSize = 1.0;

    // initialize the boundary box
    m_boundaryBoxMin.zero();
    m_boundaryBoxMax.zero();
    m_showBox = false;
    m_boundaryBoxColor.set(0.5, 0.5, 0.0);

    // set showTree status and default color for tree
    m_showTree = false;
    m_treeColor.set(0.5, 0.0, 0.0);

    // collision detection algorithm
    m_collisionDetector = NULL;
    m_showCollisionTree = false;

    // custom user information
    m_tag = 0;
    m_userData = 0;
    
};


//===========================================================================
/*!
    Destructor of cGenericObject.  This function deletes all children
    starting from this point in the scene graph, so if you have objects
    that shouldn't be deleted, be sure to remove them from the scene
    graph before deleting their parents.

    \fn       cGenericObject::~cGenericObject()
*/
//===========================================================================
cGenericObject::~cGenericObject()
{

    // Each of my children will remove their own collision detectors when
    // they get deleted, so I just delete my own right now...
    if (m_collisionDetector) deleteCollisionDetector(0);

    // delete all children
    deleteAllChildren();
};


//===========================================================================
/*!
    Adds an object to the scene graph below this object

    \fn       void cGenericObject::addChild(cGenericObject* a_object)
    \param    a_object  Object to be added to children list.
*/
//===========================================================================
void cGenericObject::addChild(cGenericObject* a_object)
{
    // update the child object's parent pointer
    a_object->m_parent = this;

    // add this child to my list of children
    m_children.push_back(a_object);
}


//===========================================================================
/*!
    Removes an object from my list of children, without deleting the
    child object from memory.

    This method assigns the child object's parent point to null, so
    if you're moving someone around in your scene graph, make sure you
    call this function _before_ you add the child to another node in
    the scene graph.

    \fn       bool cGenericObject::removeChild(const cGenericObject* a_object)
    \param    a_object  Object to be removed from my list of children
    \return   Returns true if the specified object was found on my list of children
*/
//===========================================================================
bool cGenericObject::removeChild(cGenericObject* a_object)
{
    std::vector<cGenericObject*>::iterator nextObject;

    for(nextObject = m_children.begin();
        nextObject != m_children.end();
        nextObject++ ) {

        // Did we find the object we're trying to delete?
        if ((*nextObject) == a_object)
        {
            // he doesn't have a parent any more
            a_object->m_parent = NULL;

            // remove this object from my list of children
            m_children.erase(nextObject);

            // return success
            return true;
        }

    }

    // opertation failed
    return false;
}


//===========================================================================
/*!
    Removes an object from my list of children, and deletes the
    child object from memory.

    \fn       bool cGenericObject::deleteChild(const cGenericObject* a_object)
    \param    a_object  Object to be removed from my list of children and deleted
    \return   Returns true if the specified object was found on my list of children
*/
//===========================================================================
bool cGenericObject::deleteChild(cGenericObject* a_object)
{
    // remove object from list
    bool result = removeChild(a_object);

    // if operation succeeds, delete the object
    if (result)
    {
        delete a_object;
    }

    // return result
    return result;
}


//===========================================================================
/*!
    Clear all objects from my list of children, without deleting them

    \fn     void cGenericObject::clearAllChildren()
*/
//===========================================================================
void cGenericObject::clearAllChildren()
{
    // clear children list
    m_children.clear();
}


//===========================================================================
/*!
    Delete and clear all objects from my list of children

    \fn     void cGenericObject::deleteAllChildren()
*/
//===========================================================================
void cGenericObject::deleteAllChildren()
{
    // delete all children
    for (unsigned int i=0; i<m_children.size(); i++)
    {
        cGenericObject* nextObject = m_children[i];
        delete nextObject;
    }

    // clear my list of children
    m_children.clear();
}


//===========================================================================
/*!
    Translate this object by a specified offset

    \fn     void cGenericObject::translate(const cVector3d& a_translation)
    \param  a_translation  Translation offset
*/
//===========================================================================
void cGenericObject::translate(const cVector3d& a_translation)
{
    // apply the translation to this object and its bounding box
    m_boundaryBoxMin.add(a_translation);
    m_boundaryBoxMax.add(a_translation);

    cVector3d new_position = cAdd(m_localPos,a_translation);
    setPos(new_position);
}


//===========================================================================
/*!
    Translate an object by a specified offset

    \fn     void cGenericObject::translate(const double a_x, const double a_y,
            const double a_z)
    \param  a_x  Translation component X
    \param  a_y  Translation component Y
    \param  a_z  Translation component Z
*/
//===========================================================================
void cGenericObject::translate(const double a_x, const double a_y, const double a_z)
{
    translate(cVector3d(a_x,a_y,a_z));  
}


//===========================================================================
/*!
    Rotate this object by multiplying with a specified rotation matrix

    \fn     void cGenericObject::rotate(const cMatrix3d& a_rotation)
    \param  a_rotation  Rotation matrix
*/
//===========================================================================
void cGenericObject::rotate(const cMatrix3d& a_rotation)
{
    cMatrix3d new_rot = m_localRot;
    new_rot.mul(a_rotation);
    setRot(new_rot);
}


//===========================================================================
/*!
    Rotate this object around axis a_axis by angle a_angle (radians).  a_axis
    is not normalized, so unless you know what you're doing, normalize your
    axis before supplying it to this function.

    \fn     void cGenericObject::rotate(const cVector3d& a_axis,
            const double a_angle)
    \param  a_axis   Rotation axis
    \parame a_angle  Rotation angle in radians
*/
//===========================================================================
void cGenericObject::rotate(const cVector3d& a_axis, const double a_angle)
{
    cMatrix3d new_rot = m_localRot;
    new_rot.rotate(a_axis, a_angle);
    setRot(new_rot);

}


//===========================================================================
/*!
    Scale this object by the specified scale factor, and scale the 
    position of all my children within my reference frame.

    \fn     void cGenericObject::scale(const double a_scaleFactor)
    \param  a_scaleFactor   The current size of object is multiplied by this
                            scale factor.
*/
//===========================================================================
void cGenericObject::scale(const double a_scaleFactor)
{
    // scale this object
    scaleObject(a_scaleFactor);

    // scale all of my children
    for (unsigned int i=0; i<m_children.size(); i++)
    {
        cGenericObject* nextObject = m_children[i];

        // scale the position of this child
        nextObject->m_localPos.mul(a_scaleFactor);

        // scale child
        nextObject->scale(a_scaleFactor);
    }
}


//===========================================================================
/*!
    Compute globalPos and globalRot given the localPos and localRot
    of this object and its children.

    if \e a_frameOnly is set to \b false, additional global positions such as
    vertex positions are computed (which can be time-consuming)

    \fn     void cGenericObject::computeGlobalPositions(const bool a_frameOnly,
            const cVector3d& a_globalPos, const cMatrix3d& a_globalRot)
    \param  a_frameOnly  If \b true then only the global frame is computed
    \param  a_globalPos  Global position of my parent
    \param  a_globalRot  Global rotation matrix of my parent
*/
//===========================================================================
void cGenericObject::computeGlobalPositions(const bool a_frameOnly,
     const cVector3d& a_globalPos, const cMatrix3d& a_globalRot)
{

    // update global position vector and global rotation matrix
    m_globalPos = cAdd(a_globalPos, cMul(a_globalRot, m_localPos));
    m_globalRot = cMul(a_globalRot, m_localRot);

    // update any positions within the current object that need to be 
    // updated (e.g. vertex positions)
    updateGlobalPositions(a_frameOnly);

    // propagate this method to my children
    for (unsigned int i=0; i<m_children.size(); i++)
    {
        m_children[i]->computeGlobalPositions(a_frameOnly, m_globalPos, m_globalRot);
    }

}


//===========================================================================
/*!
    Compute globalPos and globalRot for this object only, by recursively
    climbing up the scene graph tree until the root is reached.

    If \e a_frameOnly is set to \b false, additional global positions such as
    vertex positions are computed.

    \fn     void cGenericObject::computeGlobalCurrentObjectOnly(
            const bool a_frameOnly)
    \param  a_frameOnly  If \b true then only the global frame is computed
*/
//===========================================================================
void cGenericObject::computeGlobalCurrentObjectOnly(const bool a_frameOnly)
{

    cMatrix3d globalRot;
    cVector3d globalPos;
    globalRot.identity();
    globalPos.zero();

    // get a pointer to current object
    cGenericObject *curObject = this;

    // walk up the scene graph until we reach the root, updating
    // my global position and rotation at each step
    while (curObject != NULL)
    {
        curObject->getRot().mul(globalPos);
        globalPos.add(curObject->getPos());
        cMatrix3d rot;
        curObject->getRot().mulr(globalRot, rot);
        rot.copyto(globalRot);
        curObject = curObject->getParent();
    }

    // update values
    m_globalPos = globalPos;
    m_globalRot = globalRot;

    // update any positions within the current object that need to be 
    // updated (e.g. vertex positions)
    updateGlobalPositions(a_frameOnly);
}


//===========================================================================
/*!
    Users should call this function when it's necessary to re-initialize the OpenGL
    context; e.g. re-initialize textures and display lists.  Subclasses should
    perform whatever re-initialization they need to do.

    Note that this is not an event CHAI can easily detect, so it's up to
    the application-writer to call this function on the root of the scenegraph.

    \fn     void cGenericObject::onDisplayReset(const bool a_affectChildren)
    \param  a_affectChildren  If \b true, the operation propagates through the scene graph
*/
//===========================================================================
void cGenericObject::onDisplayReset(const bool a_affectChildren)
{
    // Since I don't have any display context to update, I don't do anything here...

    // We _don't_ call this method on the current object, which allows subclasses
    // to do their business in this method, then call the cGenericObject version
    // to propagate the call through the scene graph

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->onDisplayReset(true);
        }
    }
}


//===========================================================================
/*!
    Show or hide this object.

    If \e a_affectChildren is set to \b true then all children are updated
    with the new value.

    \fn     void cGenericObject::setShow(const bool a_show,
            const bool a_affectChildren)
    \param  a_show  If \b true object shape is visible.
    \param  a_affectChildren  If \b true all children are updated.
*/
//===========================================================================
void cGenericObject::setShow(const bool a_show, const bool a_affectChildren)
{
    // update current object
    m_show = a_show;

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->setShow(a_show, true);
        }
    }
}


//===========================================================================
/*!
    Show or hide the set of arrows that represent this object's reference frame.

    If \e a_affectChildren is set to \b true then all children are updated
    with the new value.

    \fn     void cGenericObject::setShowFrame(const bool a_showFrame,
            const bool a_affectChildren)
    \param  a_showFrame  If \b true then frame is displayed.
    \param  a_affectChildren  If \b true all children are updated.
*/
//===========================================================================
void cGenericObject::setShowFrame(const bool a_showFrame, const bool a_affectChildren)
{
    // update current object
    m_showFrame = a_showFrame;

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->setShowFrame(a_showFrame, a_affectChildren);
        }
    }
}


//===========================================================================
/*!
    Set the display size of the arrows representing my reference frame.
    The size corresponds to the length of each displayed axis (X-Y-Z).

    If \e a_affectChildren is set to \b true then all children are updated
    with the new value.


    \fn     bool cGenericObject::setFrameSize(const double a_size,
            const bool a_affectChildren)
    \param  a_size  Size of graphical representation of frame.
    \param  a_affectChildren  If \b true all children are updated.
*/
//===========================================================================
bool cGenericObject::setFrameSize(const double a_size, const bool a_affectChildren)
{
    // check value of size
    if (a_size <= 0) { return (false); }

    // update current object
    m_frameSize = a_size;

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->setFrameSize(a_size, a_affectChildren);
        }
    }

    // operation succeded
    return (true);
}


//===========================================================================
/*!
    Show or hide the graphic representation of the scene graph at this
    node.

    If \e a_affectChildren is set to \b true then all children are updated
    with the new value.

    \fn     void cGenericObject::setShowTree(const bool a_showTree,
            const bool a_affectChildren)
    \param  a_showTree  If \b true then tree is displayed.
    \param  a_affectChildren  If \b true all children are updated.
*/
//===========================================================================
void cGenericObject::setShowTree(const bool a_showTree, const bool a_affectChildren)
{
    // update current object
    m_showTree = m_showTree;

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->setShowTree(a_showTree, true);
        }
    }
}


//===========================================================================
/*!
    Set the color of the graphic representation of the scene graph at
    this node.

    If \e a_affectChildren is set to \b true then all children are updated
    with the new value.

    \fn     void cGenericObject::setTreeColor(const cColor& iTreeColor,
      const bool a_affectChildren)
    \param  a_treeColor  Color of tree.
    \param  a_affectChildren  If \b true all children are updated.
*/
//===========================================================================
void cGenericObject::setTreeColor(const cColorf& a_treeColor, const bool a_affectChildren)
{
    // update current object
    m_treeColor = a_treeColor;

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->setTreeColor(a_treeColor, true);
        }
    }
}


//===========================================================================
/*!
    Show or hide the graphic representation of the boundary box of this object.

    If \e a_affectChildren is set to \b true then all children are updated
    with the new value.

    \fn     void cGenericObject::setShowBox(const bool a_showBox,
            const bool a_affectChildren)
    \param  a_showBox  If \b true, boundary box is displayed.
    \param  a_affectChildren  If \b true all children are updated.
*/
//===========================================================================
void cGenericObject::setShowBox(const bool a_showBox, const bool a_affectChildren)
{
    // update current object
    m_showBox = a_showBox;

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->setShowBox(a_showBox, true);
        }
    }
}


//===========================================================================
/*!
    Set the color of the graphic representation of the boundary boundary box of this object.

    \fn     void cGenericObject::setBoxColor(const cColor& a_boxColor,
            const bool a_affectChildren)
    \param  a_boxColor  Color of boundary box.
    \param  a_affectChildren  If \b true all children are updated.
*/
//===========================================================================
void cGenericObject::setBoxColor(const cColorf& a_boxColor, const bool a_affectChildren)
{
    // update current object
    m_boundaryBoxColor = a_boxColor;

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->setBoxColor(a_boxColor, true);
        }
    }
}


//===========================================================================
/*!
    Show or hide the graphic representation of the collision tree at this node.

    If \e a_affectChildren is set to \b true then all children are updated
    with the new value.

    \fn     void cGenericObject::showCollisionTree(const bool a_showCollisionTree,
            const bool a_affectChildren)
    \param  a_showCollisionTree If \b true, small normals are rendered graphicaly.
    \param  a_affectChildren  If \b true all children are updated
*/
//===========================================================================
void cGenericObject::showCollisionTree(const bool a_showCollisionTree, const bool a_affectChildren)
{
    // update current object
    m_showCollisionTree = a_showCollisionTree;

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->showCollisionTree(a_showCollisionTree, true);
        }
    }
}


//===========================================================================
/*!
    Delete any existing collision detector and set the current cd to null.  
    It's fine for an object to have a null collision detector (that's the
    default for a new object, in fact), it just means that no collisions will be
    found.

    \fn     void cGenericObject::deleteCollisionDetector(const bool a_affectChildren)
    \param  a_affectChildren  If \true all my children's cd's are also deleted
*/
//===========================================================================
void cGenericObject::deleteCollisionDetector(const bool a_affectChildren)
{

    if (m_collisionDetector)
    {
        delete m_collisionDetector;
        m_collisionDetector = 0;
    }

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->deleteCollisionDetector(true);
        }
    }
}


//===========================================================================
/*!
    Set the rendering properties for the graphic representation of collision detection
    tree at this node.

    If \e a_affectChildren is set to \b true then all children are updated
    with the new values.

    \fn     void cGenericObject::setCollisionDetectorProperties
            ( unsigned int a_displayDepth, cColor& a_color, const bool a_affectChildren)
    \param  a_color  Color used to render collision detector.
    \param  a_displayDepth  Indicated which depth of collision tree needs to be displayed
                            (see cGenericCollision)
    \param  a_affectChildren  If \true all children are updated
*/
//===========================================================================
void cGenericObject::setCollisionDetectorProperties(unsigned int a_displayDepth,
     cColorf& a_color, const bool a_affectChildren)
{
    // update current collision detector
    if (m_collisionDetector != NULL)
    {
        m_collisionDetector->m_material.m_ambient.set(a_color.getR(), a_color.getG(),
                                                      a_color.getB(), a_color.getA());
        m_collisionDetector->setDisplayDepth(a_displayDepth);
    }

    // update children
    if (a_affectChildren)
    {
        for (unsigned int i=0; i<m_children.size(); i++)
        {
            m_children[i]->setCollisionDetectorProperties(a_displayDepth,
            a_color, a_affectChildren);
        }
    }
}


// We need a constant to determine if an object has already been assigned
// a 'real' bounding box
#define BOUNDARY_BOX_EPSILON 1e-15

//===========================================================================
/*!
    Compute the bounding box of this object and (optionally) its children.
    
    If parameter \e a_includeChildren is set to \b true then each object's 
    bounding box covers its own volume and the volume of its children.

    Note that regardless of this parameter's value, this operation propagates
    down the scene graph.

    \fn     void cGenericObject::computeBoundaryBox(const bool a_includeChildren)
    \param  a_includeChildren  If \true, then children are included.
*/
//===========================================================================
void cGenericObject::computeBoundaryBox(const bool a_includeChildren)
{

    // compute the bounding box of this object
    updateBoundaryBox();

    // compute the bounding box of all my children
    for (unsigned int i=0; i<m_children.size(); i++)
    {

        m_children[i]->computeBoundaryBox(a_includeChildren);

        // inlcude children
        if (a_includeChildren)
        {
            // see if this child has a _valid_ boundary box
            bool child_box_valid = (
              fabs(cDistance(m_children[i]->getBoundaryMax(),
                             m_children[i]->getBoundaryMin())) >
                            BOUNDARY_BOX_EPSILON
              );

            // don't include invalid boxes in my bounding box
            if (child_box_valid == 0) continue;

            // get position and rotation of child frame
            cMatrix3d rot = m_children[i]->getRot();
            cVector3d pos = m_children[i]->getPos();
            cVector3d V;

            // convert value into parent frame
            cVector3d childBoxMin = m_children[i]->getBoundaryMin();
            rot.mulr(childBoxMin, V);
            V.addr(pos, childBoxMin);

            // convert value into parent frame
            cVector3d childBoxMax = m_children[i]->getBoundaryMax();
            rot.mulr(childBoxMax, V);
            V.addr(pos, childBoxMax);

            // see if _I_ have a valid boundary box
            bool current_box_valid = (
              fabs(cDistance(m_boundaryBoxMax,m_boundaryBoxMin)) > BOUNDARY_BOX_EPSILON
              );

            // if I don't, take my child's boundary box, which is valid...
            if (current_box_valid == 0) {
              m_boundaryBoxMin = childBoxMin;
              m_boundaryBoxMax = childBoxMax;
            }

            else {

              // compute new boundary
              m_boundaryBoxMin.x = cMin(m_boundaryBoxMin.x, childBoxMin.x);
              m_boundaryBoxMin.y = cMin(m_boundaryBoxMin.y, childBoxMin.y);
              m_boundaryBoxMin.z = cMin(m_boundaryBoxMin.z, childBoxMin.z);

              // compute new boundary
              m_boundaryBoxMax.x = cMax(m_boundaryBoxMax.x, childBoxMax.x);
              m_boundaryBoxMax.y = cMax(m_boundaryBoxMax.y, childBoxMax.y);
              m_boundaryBoxMax.z = cMax(m_boundaryBoxMax.z, childBoxMax.z);

            }
        }
    }
}


//===========================================================================
/*!
    Test whether a ray intersects this object. The test segment is described
    by a start point /e a_segmentPointA and end point /e a_segmentPointB.

    If a collision occurs, the squared distance between the segment start point
    and the collision point in measured and compared to any previous collision
    information stored in parameters \e a_colObject, \e a_colTriangle,
    \e a_colPoint, and \e a_colSquareDistance.
    
    If the new collision is located nearer to the ray origin than the previous
    collision point, it is stored in the corresponding parameters \e a_colObject,
    \e a_colTriangle, \e a_colPoint, and \e a_colSquareDistance.

    \param  a_segmentPointA      Start point of segment.
    \param  a_segmentPointB      End point of segment.
    \param  a_colObject          Pointer to nearest collided object.
    \param  a_colTriangle        Pointer to nearest colided triangle.
    \param  a_colPoint           Position to the nearest collision
    \param  a_colSquareDistance  Squared distance between ray origin and nearest
                                 collision point

    \param  a_proxyCall  If this is > 0, this is a call from a proxy, and the value
                         of a_proxyCall specifies which call this is.  -1 is passed
                         for non-proxy calls.
*/
//===========================================================================
bool cGenericObject::computeCollisionDetection(
        cVector3d& a_segmentPointA, cVector3d& a_segmentPointB,
        cGenericObject*& a_colObject, cTriangle*& a_colTriangle, cVector3d& a_colPoint,
        double& a_colSquareDistance, const bool a_visibleObjectsOnly, int a_proxyCall,
        cGenericPointForceAlgo* force_algo)
{
    // no collision found yet
    bool hit = false;
    
    // convert collision segment into local coordinate system.
    cMatrix3d transLocalRot;
    m_localRot.transr(transLocalRot);

    cVector3d localSegmentPointA = a_segmentPointA;
    localSegmentPointA.sub(m_localPos);
    transLocalRot.mul(localSegmentPointA);

    cVector3d localSegmentPointB = a_segmentPointB;
    localSegmentPointB.sub(m_localPos);
    transLocalRot.mul(localSegmentPointB);

    // initialize objects for calls
    cGenericObject* t_colObject;
    cTriangle *t_colTriangle;
    cVector3d t_colPoint;
    double t_colSquareDistance = a_colSquareDistance;

    // check collision with current object
    if ((m_collisionDetector != NULL) && (!a_visibleObjectsOnly || m_show))
    {
        if (m_collisionDetector->computeCollision(localSegmentPointA, localSegmentPointB,
            t_colObject, t_colTriangle, t_colPoint, t_colSquareDistance, a_proxyCall))
        {
            // object was hit
            hit = true;

            a_colObject = t_colObject;
            a_colTriangle = t_colTriangle;
            a_colPoint = t_colPoint;
            a_colSquareDistance = t_colSquareDistance;

            // convert collision point into parent coordinate
            m_localRot.mul(a_colPoint);
            a_colPoint.add(m_localPos);

        }
    }

    // search for collision with all child objects
    for (unsigned int i=0; i<m_children.size(); i++)
    {
        if( m_children[i]->computeCollisionDetection(localSegmentPointA, localSegmentPointB,
            t_colObject, t_colTriangle, t_colPoint, t_colSquareDistance, a_visibleObjectsOnly, a_proxyCall,
            force_algo))
        {
            // object was hit
            hit = true;

            if (t_colSquareDistance < a_colSquareDistance)
            {
              a_colObject = t_colObject;
              a_colTriangle = t_colTriangle;
              a_colPoint = t_colPoint;
              a_colSquareDistance = t_colSquareDistance;

              // convert collision point into parent coordinate
              m_localRot.mul(a_colPoint);
              a_colPoint.add(m_localPos);
            }
        }
    }

    // return result
    return (hit);
}


//===========================================================================
/*!
    Render the scene graph starting at this object. This method is called
    for each object and optionally renders the object itself, its reference frame
    and the collision and/or scenegraph trees.
    
    The object itself is rendered by calling render(), which should be defined
    for each subclass that has a graphical representation.

    The a_renderMode parameter is used to allow multiple rendering passes,
    and takes one of the following values:

    CHAI_RENDER_NON_TRANSPARENT, CHAI_RENDER_TRANSPARENT_FRONT, CHAI_RENDER_TRANSPARENT_BACK

    If you have multipass transparency disabled (see cCamera), your objects will
    only be rendered once per frame, with a_renderMode set to CHAI_RENDER_NON_TRANSPARENT.

    \fn     void cGenericObject::renderSceneGraph(const int a_renderMode)
    \param  a_renderMode  Rendering mode
*/
//===========================================================================
void cGenericObject::renderSceneGraph(const int a_renderMode)
{
    // render pose
    m_frameGL.set(m_localPos, m_localRot);
    m_frameGL.glMatrixPushMultiply();

    if (a_renderMode & CHAI_RENDER_NON_TRANSPARENT)
    {
        // disable lighting
        glDisable(GL_LIGHTING);

        // render tree
        if (m_showTree)
        {
            // set size on lines
            glLineWidth(1.0);

            // set color of tree
            glColor4fv(m_treeColor.pColor());

            // render tree
            for (unsigned int i=0; i<m_children.size(); i++)
            {
                cGenericObject *nextChild = m_children[i];

                // draw a line from origin of current frame to origin  of child frame
                glBegin(GL_LINES);
                    glVertex3d(0.0, 0.0, 0.0);
                    glVertex3dv((const double *)&nextChild->m_localPos);
                glEnd();
            }
        }

        // render boundary box
        if (m_showBox)
        {
            // set size on lines
            glLineWidth(1.0);

            // set color of boundary box
            glColor4fv(m_boundaryBoxColor.pColor());

            // draw box line
            cDrawWireBox(m_boundaryBoxMin.x, m_boundaryBoxMax.x,
                         m_boundaryBoxMin.y, m_boundaryBoxMax.y,
                         m_boundaryBoxMin.z, m_boundaryBoxMax.z);
        }

        // render collision tree
        if (m_showCollisionTree && (m_collisionDetector != NULL))
        {
            m_collisionDetector->render();
        }

        // enable lighting
        glEnable(GL_LIGHTING);

        // render frame
        if (m_showFrame)
        {
            // set rendering properties
            glPolygonMode(GL_FRONT, GL_FILL);

            // draw frame
            cDrawFrame(m_frameSize);
        }
    }

    // render graphical representation of object
    if (m_show)
    {
        render(a_renderMode);
    }

    // render children
    for (unsigned int i=0; i<m_children.size(); i++)
    {
        m_children[i]->renderSceneGraph(a_renderMode);
    }

    // pop current matrix
    m_frameGL.glMatrixPop();
}




