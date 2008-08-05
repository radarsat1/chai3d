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

    // set showTree status default color for tree
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
    Destructor of cGenericObject.

    \fn       cGenericObject::~cGenericObject()
*/
//===========================================================================
cGenericObject::~cGenericObject()
{
    // delete all children
    deleteAllChildren();
};


//===========================================================================
/*!
    Add a child object to current generic object.

    \fn       void cGenericObject::addChild(cGenericObject* a_object)
    \param    a_object  Object to be added to children list.
*/
//===========================================================================
void cGenericObject::addChild(cGenericObject* a_object)
{
    // update parent
    a_object->m_parent = this;

    // add child to list
    m_children.push_back(a_object);
}


//===========================================================================
/*!
    Remove an object from children list. Object is not deleted from memory.

    \fn       bool cGenericObject::removeChild(const cGenericObject* a_object)
    \param    a_object  Object to be removed from children list.
*/
//===========================================================================
bool cGenericObject::removeChild(cGenericObject* a_object)
{
    // set iterator
    std::vector<cGenericObject*>::iterator nextObject;

    for(nextObject = m_children.begin();
        nextObject != m_children.end();
        nextObject++ ) {


        if ((*nextObject) == a_object)
        {
            // no more parent.
            a_object->m_parent = NULL;

            // remove object from list
            m_children.erase(nextObject);

            // return success
            return (true);
        }

    }

    // opertation failed
    return (false);
}


//===========================================================================
/*!
    Remove child from children list and delete it.

    \fn         bool cGenericObject::deleteChild(const cGenericObject a_object)
    \param      a_object  Object to be removed and deleted from children list.
*/
//===========================================================================
bool cGenericObject::deleteChild(cGenericObject* a_object)
{
    // remove object from list
    bool result = removeChild(a_object);

    // if operation succeeds, delete object
    if (result)
    {
        delete a_object;
    }

    // return result
    return (result);
}


//===========================================================================
/*!
    Clear all objects from children list.

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
    Delete and clear all objects from children list.

    \fn     void cGenericObject::clearAllChildren()
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

    // clear children list
    m_children.clear();
}


//===========================================================================
/*!
    Translate an object by giving a translation offset.

    \fn     void cGenericObject::translate(const cVector3d& a_translation)
    \param  a_translation  Translation offset.
*/
//===========================================================================
void cGenericObject::translate(const cVector3d& a_translation)
{
    // apply translation to object and bounding box
    m_boundaryBoxMin.add(a_translation);
    m_boundaryBoxMax.add(a_translation);

    cVector3d new_position = cAdd(m_localPos,a_translation);
    setPos(new_position);
}


//===========================================================================
/*!
    Translate an object by giving a translation offset.

    \fn     void cGenericObject::translate(const double a_x, const double a_y,
            const double a_z)
    \param  a_x  Translation component X.
    \param  a_y  Translation component Y.
    \param  a_z  Translation component Z.
*/
//===========================================================================
void cGenericObject::translate(const double a_x, const double a_y, const double a_z)
{
    translate(cVector3d(a_x,a_y,a_z));    
}


//===========================================================================
/*!
    Rotate object by multiplying current rotation matrix with an external
    rotation matrix passed as parameter.

    \fn     void cGenericObject::rotate(const cMatrix3d& a_rotation)
    \param  a_rotation  Rotation matrix
*/
//===========================================================================
void cGenericObject::rotate(const cMatrix3d& a_rotation)
{
    // apply rotation:
    cMatrix3d new_rot = m_localRot;
    new_rot.mul(a_rotation);
    setRot(new_rot);
}


//===========================================================================
/*!
    Rotate object arround an axis and angle.

    \fn     void cGenericObject::rotate(const cVector3d& a_axis,
            const double a_angle)
    \param  a_axis   Rotation axis (length must equal to 1)
    \parame a_angle  Rotation angle in radian.
*/
//===========================================================================
void cGenericObject::rotate(const cVector3d& a_axis, const double a_angle)
{
    // apply rotation
    cMatrix3d new_rot = m_localRot;
    new_rot.rotate(a_axis, a_angle);
    setRot(new_rot);

}


//===========================================================================
/*!
    Scale the object by passing a scaling factor

    \fn     void cGenericObject::scale(const double a_scaleFactor)
    \param  a_scaleFactor   current size of object is multiplied by this scale factor.
*/
//===========================================================================
void cGenericObject::scale(const double a_scaleFactor)
{
    // scale this object
    scaleObject(a_scaleFactor);

    // scale children
    for (unsigned int i=0; i<m_children.size(); i++)
    {
        cGenericObject* nextObject = m_children[i];

        // scale position of child
        nextObject->m_localPos.mul(a_scaleFactor);

        // scale child
        nextObject->scale(a_scaleFactor);
    }
}


//===========================================================================
/*!
    Compute globalPos and globalRot given localPos and localRot (frame).
    of current object and its children. \n
    if \e a_frameOnly is set to \b false, additional global positions such as
    vertex positions are computed.

    \fn     void cGenericObject::computeGlobalPositions(const bool a_frameOnly,
            const cVector3d& a_globalPos, const cMatrix3d& a_globalRot)
    \param  a_frameOnly  If \b true then only frame is computed.
    \param  a_globalPos  Global position of parent.
    \param  a_globalRot  Global rotation matrix of parent.
*/
//===========================================================================
void cGenericObject::computeGlobalPositions(const bool a_frameOnly,
     const cVector3d& a_globalPos, const cMatrix3d& a_globalRot)
{

    // update global position vector and global rotation matrix
    m_globalPos = cAdd(a_globalPos, cMul(a_globalRot, m_localPos));
    m_globalRot = cMul(a_globalRot, m_localRot);

    // update current object
    updateGlobalPositions(a_frameOnly);

    // update children
    for (unsigned int i=0; i<m_children.size(); i++)
    {
        m_children[i]->computeGlobalPositions(a_frameOnly, m_globalPos, m_globalRot);
    }

}


//===========================================================================
/*!
    Compute globalPos and globalRot of this object only. the algorithm
    computes the global position and rotation matrix by recursively climbing
    up the scene graph tree until the root is reached\n
    if \e a_frameOnly is set to \b false, additional global positions such as
    vertex positions are computed.

    \fn     void cGenericObject::computeGlobalCurrentObjectOnly(
            const bool a_frameOnly)
    \param  a_frameOnly  If \b true then only frame is computed.
*/
//===========================================================================
void cGenericObject::computeGlobalCurrentObjectOnly(const bool a_frameOnly)
{

    // init
    cMatrix3d globalRot;
    cVector3d globalPos;
    globalRot.identity();
    globalPos.zero();

    // get current object
    cGenericObject *curObject = this;

    // parse all parents
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

    // update current object
    updateGlobalPositions(a_frameOnly);
}


//===========================================================================
/*!
    Set the display status of the object.
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
    Set display status of frame.
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
    Set display size of frame. The size corresponds to the length of each
    displayed axis (X-Y-Z).
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
    Set display status of tree.
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
    Set color of tree.
    If \e a_affectChildren is set to \b true then all children are updated
    with the new value.

    \fn     void cGenericObject::setTreeColor(const cColor& iTreeColor, const bool a_affectChildren)
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
    Set the display status of boundary box.
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
    Set the color of boundary box.

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
    Show collision tree
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
    Set rendering properties of collision detector
    If \e a_affectChildren is set to \b true then all children are updated
    with the new values.

    \fn     void cGenericObject::setCollisionDetectorProperties
            ( unsigned int a_displayDepth, cColor& a_color, const bool a_affectChildren)
    \param  a_color  Color used to render collision detector.
    \param  a_displayDepth  Indicated which depth of collision tree needs to be displayed.
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


//===========================================================================
/*!
    Compute the bounding box of an object and its children. If parameter
    \e a_includeChildren is set to \b true then each bounding box covers
    itself and all its children.

    \fn     void cGenericObject::computeBoundaryBox(const bool a_includeChildren)
    \param  a_includeChildren  If \true, then children are included.
*/
//===========================================================================
void cGenericObject::computeBoundaryBox(const bool a_includeChildren)
{
    #define BOUNDARY_BOX_EPSILON 1e-15

    // compute bounding box of current object
    updateBoundaryBox();

    // compute bounding box of children
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
    Check if a ray intersects this current triangle. The segment is described
    by a start point /e a_segmentPointA and end point /e a_segmentPointB. \n

    If a collision occures, the square distance between the segment start point
    and collision point in measured and compared to any previous collision
    information stored in parameters \e a_colObject, \e a_colTriangle,
    \e a_colPoint, and \e a_colSquareDistance. If the new collision is located
    nearer to ray origin than previous collision point, it is stored
    in the corresponding parameters \e a_colObject, \e a_colTriangle,
    \e a_colPoint, and \e a_colSquareDistance.

    \param  a_segmentPointA  Start point of segment.
    \param  a_segmentPointB  End point of segment.
    \param  a_colObject  Pointer to nearest collided object.
    \param  a_colTriangle Pointer to nearest colided triangle.
    \param  a_colPoint  Position of nearest collision.
    \param  a_colSquareDistance  Square distance between ray origin and nearest
            collision point
    \param  a_proxyCall  If this is > 0, this is a call from a proxy, and the value
                         of a_proxyCall specifies which call this is.  -1 for non-proxy
                         calls.
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
    Render sceene graph. This method is called for each object and renders
    the frame and tree. The graphical representation of the object is
    rendered by method render(). Method render() is specific for each type
    of object and should be modified accordingly.

    \fn     void cGenericObject::renderSceneGraph(const int a_renderMode)
    \param  a_renderMode  Rendering mode.
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




