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
    \authir:    Dan Morris
    \version    1.1
    \date       01/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CGenericObjectH
#define CGenericObjectH
//---------------------------------------------------------------------------
#include "CMaths.h"
#include "CDraw3D.h"
#include "CColor.h"
#include "CMacrosGL.h"
#include <typeinfo>
#include <vector>
//---------------------------------------------------------------------------
using std::vector;
//---------------------------------------------------------------------------
class cTriangle;
class cGenericCollision;
class cGenericPointForceAlgo;
//---------------------------------------------------------------------------

// Constants that define specific rendering passes (see cCamera.cpp)
const int CHAI_RENDER_NON_TRANSPARENT           = 1;
const int CHAI_RENDER_TRANSPARENT_FRONT         = 2;
const int CHAI_RENDER_TRANSPARENT_BACK          = 4;
//---------------------------------------------------------------------------


//===========================================================================
/*!
      \class      cGenericObject
      \brief      This class is the root of basically every render-able object
                  in CHAI.  It defines a reference frame (position and rotation)
                  and virtual methods for rendering, which are overloaded by
                  useful subclasses.      
                  
                  This class also defines basic methods for maintaining a 
                  scene graph, and propagating rendering passes and reference
                  frame changes through a hierarchy of cGenericObjects.

                  Besides subclassing, a useful way to extend cGenericObject
                  is to store custom data in the m_tag and m_userData member
                  fields, which are not used by CHAI.

                  The most important methods to look at here are probably the
                  virtual methods, which are listed last in CGenericObject.h .
                  These methods will be called on each cGenericObject as
                  operations propagate through the scene graph.
*/
//===========================================================================
#ifdef _MSVC
class cGenericObject
#else
class __rtti cGenericObject
#endif
{    

  public:

    // CONSTRUCTOR & DESTRUCTOR:

    //! Constructor of cGenericObject
    cGenericObject();

    //! Destructor of cGenericObject
    virtual ~cGenericObject();
  

    // METHODS - RENDERING:

    //! Render the entire scene graph, starting from this object
    void renderSceneGraph(const int a_renderMode=CHAI_RENDER_NON_TRANSPARENT);

  
    // METHODS - GENERAL:

    //! Read parent of current object.
    cGenericObject* getParent() const { return (m_parent); }


    // METHODS - TRANSLATION AND ORIENTATION:
    
    //! Set the local position of this object
    inline void setPos(const cVector3d& a_pos) { m_localPos = a_pos; }

    //! Set the local position of this object
    inline void setPos(const double a_x, const double a_y, const double a_z) { m_localPos.set(a_x, a_y, a_z); }

    //! Get the local position of this object
    inline cVector3d getPos() const { return (m_localPos); }

    //! Get the global position of this object
    inline cVector3d getGlobalPos() const { return (m_globalPos); }

    //! Set the local rotation matrix for this object
    inline void setRot(const cMatrix3d& a_rot) { m_localRot = a_rot; }

    //! Get the local rotation matrix of this object
    inline cMatrix3d getRot() const { return (m_localRot); }

    //! Get the global rotation matrix of this object
    inline cMatrix3d getGlobalRot() const { return (m_globalRot); }

    //! Translate this object by a specified offset
    void translate(const cVector3d& a_translation);

    //! Translate this object by a specified offset
    void translate(const double a_x, const double a_y, const double a_z);

    //! Rotate this object by multiplying with a specified rotation matrix
    void rotate(const cMatrix3d& a_rotation);

    //! Rotate this object around axis a_axis by angle a_angle (radians)
    void rotate(const cVector3d& a_axis, const double a_angle);


    // METHODS - GLOBAL / LOCAL POSITIONS:

    //! Compute the global position and rotation of this object and its children
    void computeGlobalPositions(const bool a_frameOnly = true,
                                const cVector3d& a_globalPos = cVector3d(0.0, 0.0, 0.0),
                                const cMatrix3d& a_globalRot = cIdentity3d());

    //! Compute the global position and rotation of current object only
    void computeGlobalCurrentObjectOnly(const bool a_frameOnly = true);

    //! Compute collision detection using collision trees
    virtual bool computeCollisionDetection(cVector3d& a_segmentPointA, cVector3d& a_segmentPointB,
         cGenericObject*& a_colObject, cTriangle*& a_colTriangle, cVector3d& a_colPoint,
         double& a_colSquareDistance, const bool a_visibleObjectsOnly, int a_proxyCall,
         cGenericPointForceAlgo* force_algo=0);

    
    // METHODS - GRAPHICS:

    //! Show or hide this object, optionally propagating the change to children
    void setShow(const bool a_show, const bool a_affectChildren = false);
    //! Read the display status of object (true means it's visible)
    bool getShow() const { return (m_show); }

    //! Show or hide the child/parent tree, optionally propagating the change to children
    void setShowTree(const bool a_showTree, const bool a_affectChildren = false);
    //! Read the display status of the tree (true means it's visible)
    bool getShowTree() const { return (m_showTree); }

    //! Set the tree color, optionally propagating the change to children
    void setTreeColor(const cColorf& a_treeColor, const bool a_affectChildren  = false);
    //! Read the tree color
    cColorf getTreeColor() const { return (m_treeColor); }

    //! Show or hide the reference frame arrows for this object, optionally propagating the change to children
    void setShowFrame(const bool a_showFrame, const bool a_affectChildren  = false);
    //! Read the display status of the reference frame (true means it's visible)
    bool getShowFrame(void) const { return (m_showFrame); }

    //! Show or hide the boundary box for this object, optionally propagating the change to children
    void setShowBox(const bool iShowBox, const bool iAffectChildren = false);
    //! Read the display status of boundary box. (true means it's visible)
    bool getShowBox() const { return (m_showBox); }

    //! Set the color of boundary box for this object, optionally propagating the change to children
    void setBoxColor(const cColorf& a_boxColor, const bool a_affectChildren = false);
    //! Read the color of boundary box
    cColorf getBoxColor() const { return (m_boundaryBoxColor); }

    //! Show or hide the collision tree for this object, optionally propagating the change to children
    void showCollisionTree(const bool a_showCollisionTree, const bool a_affectChildren = false);

    //! This function should get called when it's necessary to re-initialize the OpenGL context
    virtual void onDisplayReset(const bool a_affectChildren = true);


    // METHODS - FRAME [X,Y,Z]

    //! Set the size of the rendered reference frame, optionally propagating the change to children
    bool setFrameSize(const double a_size, const bool a_affectChildren = false);
    //! Read the size of the rendered reference frame
    double getFrameSize() const { return (m_frameSize); }
    
    //! Scale the object by passing a scaling factor.  Scales the position of all my children in my reference frame.
    void scale(const double a_scaleFactor);

    // METHODS - BOUNDARY BOX
    //! Read the minimum point of this object's boundary box
    cVector3d getBoundaryMin() const { return (m_boundaryBoxMin); }
    //! Read the maximum point of this object's boundary box
    cVector3d getBoundaryMax() const { return (m_boundaryBoxMax); }

    //! Re-compute this object's bounding box, optionally forcing it to bound child objects
    void computeBoundaryBox(const bool a_includeChildren);


    // METHODS - COLLISION DETECTION

    //! Set a collision detector for current object
    void setCollisionDetector(cGenericCollision* a_collisionDetector)
         { m_collisionDetector = a_collisionDetector; }
    //! Get pointer to this object's current collision detector.
    cGenericCollision* getCollisionDetector() { return (m_collisionDetector); }
    //! Set collision rendering properties
    void setCollisionDetectorProperties(unsigned int a_displayDepth, cColorf& a_color, const bool a_affectChildren = false);
    //! Delete any existing collision detector and set the current cd to null (no collisions)
    void deleteCollisionDetector(const bool a_affectChildren = false);


    // METHODS - SCENE GRAPH

    //! Read an object from my list of children
    inline cGenericObject* getChild(const unsigned int a_index) const { return (m_children[a_index]); }
    //! Add an object to my list of children
    void addChild(cGenericObject* a_object);
    //! Remove an object from my list of children, without deleting it
    bool removeChild(cGenericObject* a_object);
    //! Remove an object from my list of children and delete it
    bool deleteChild(cGenericObject *a_object);
    //! Clear all objects from my list of children, without deleting them
    void clearAllChildren();
    //! Clear and delete all objects from my list of children
    void deleteAllChildren();
    //! Return the number of children on my list of children
    int getNumChildren() { return m_children.size(); }

    //! Remove me from my parent's CHAI scene graph
    bool removeFromGraph()
    {
      if (m_parent) return m_parent->removeChild(this);
      else return false;
    }


    // MEMBERS - CUSTOM USER DATA

    //! An arbitrary tag, not used by CHAI
    int m_tag;
    //! An arbitrary data pointer, not used by CHAI
    void* m_userData;

  protected:

    // MEMBERS - SCENE GRAPH

    //! Parent object
    cGenericObject* m_parent;
    //! My list of children
    vector<cGenericObject*> m_children;


    // MEMBERS - POSITION & ORIENTATION
    
    //! The position of this object in my parent's reference frame
    cVector3d m_localPos;
    //! The position of this object in the world's reference frame
    cVector3d m_globalPos;
    //! The rotation matrix that rotates my reference frame into my parent's reference frame
    cMatrix3d m_localRot;
    //! The rotation matrix that rotates my reference frame into the world's reference frame
    cMatrix3d m_globalRot;


    // MEMBERS - BOUDARY BOX
    
    //! Minimum position of boundary box
    cVector3d m_boundaryBoxMin;
    //! Maximum position of boundary box
    cVector3d m_boundaryBoxMax;


    // MEMBERS - FRAME [X,Y,Z]

    //! Size of graphical representation of frame (X-Y-Z).
    double m_frameSize;


    // MEMBERS - GRAPHICS

    //! If \b true, this object is rendered
    bool m_show;
    //! If \b true, this object's reference frame is rendered as a set of arrows
    bool m_showFrame;
    //! If \b true, this object's boudary box is displayed as a set of lines
    bool m_showBox;
    //! If \b true, the skeleton of the scene graph is rendered at this node
    bool m_showTree;
    //! If \b true, the collision tree is displayed (if available) at this node
    bool m_showCollisionTree;
    //! The color of the collision tree
    cColorf m_treeColor;
    //! The color of the bounding box
    cColorf m_boundaryBoxColor;

    // MEMBERS - COLLISION DETECTION

    //! The collision detector used to test for contact with this object
    cGenericCollision* m_collisionDetector;


    // VIRTUAL METHODS:

    //! Render this object in OpenGL
    virtual void render(const int a_renderMode=CHAI_RENDER_NON_TRANSPARENT) {};

    //! Update the m_globalPos and m_globalRot properties of any members of this object (e.g. all triangles)
    virtual void updateGlobalPositions(const bool a_frameOnly) {};

    //! Update the bounding box of this object, based on object-specific data (e.g. triangle positions)
    virtual void updateBoundaryBox() {};

    //! Scale this object by a_scaleFactor
    virtual void scaleObject(double a_scaleFactor) {};

    // MEMBERS:
    //! OpenGL matrix describing my position and orientation transformation
    cMatrixGL m_frameGL;
    
};


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

