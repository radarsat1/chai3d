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
const int CHAI_RENDER_NON_TRANSPARENT           = 1;
const int CHAI_RENDER_TRANSPARENT_FRONT         = 2;
const int CHAI_RENDER_TRANSPARENT_BACK          = 4;
//---------------------------------------------------------------------------


//===========================================================================
/*!
      \class      cGenericObject
      \brief      cGenericObject describes a base class for expressing
                  virtual graphic entities which can be displayed in
                  a virtual world.
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
    //! Constructor of cGenericObject.
    cGenericObject();
    //! Destructor of cGenericObject.
    virtual ~cGenericObject();
    
    // METHODS - GENERAL:
    //! Read parent of current object.
    cGenericObject* getParent() const { return (m_parent); }

    // METHODS - TRANSLATION AND ORIENTATION
    //-----------------------------------------------------------------------
    /*!
        Set the local position of object.

        \param      a_pos  New position.
    */
    //-----------------------------------------------------------------------
    inline void setPos(const cVector3d& a_pos)
    {
        m_localPos = a_pos;    
    }

    //-----------------------------------------------------------------------
    /*!
        Set the local position of object.

    \param	a_x	 X component.
		\param  a_y	 Y component.
		\param	a_z	 Z component.
    */
    //-----------------------------------------------------------------------
    inline void setPos(const double a_x, const double a_y, const double a_z)
    {
        m_localPos.set(a_x, a_y, a_z);        
    }


    //-----------------------------------------------------------------------
    /*!
        Read local position of object.

        \return     Return vector.
    */
    //-----------------------------------------------------------------------
    inline cVector3d getPos() const
    {
        return (m_localPos);
    }


    //-----------------------------------------------------------------------
    /*!
        Read global position of object.

        \return     Return vector.
    */
    //-----------------------------------------------------------------------
    inline cVector3d getGlobalPos() const
    {
        return (m_globalPos);
    }


    //-----------------------------------------------------------------------
    /*!
        Set local rotation matrix.

        \param      a_rot  rotation matrix.
    */
    //-----------------------------------------------------------------------
    inline void setRot(const cMatrix3d& a_rot)
    {
        m_localRot = a_rot;        
    }


    //-----------------------------------------------------------------------
    /*!
        Read local rotation matrix.

        \return     Return matrix.
    */
    //-----------------------------------------------------------------------
    inline cMatrix3d getRot() const
    {
        return (m_localRot);
    }


    //-----------------------------------------------------------------------
    /*!
        Read global rotation matrix.

        \return     Return matrix.
    */
    //-----------------------------------------------------------------------
    inline cMatrix3d getGlobalRot() const
    {
        return (m_globalRot);
    }

    //! Translate object with a defined offset.
    void translate(const cVector3d& a_translation);
    //! Translate object with a defined offset.
    void translate(const double a_x, const double a_y, const double a_z);
    //! Rotate object by multiplying with a rotation matrix.
    void rotate(const cMatrix3d& a_rotation);
    //! Rotate object around axis a_axis by angle a_angle (radians)
    void rotate(const cVector3d& a_axis, const double a_angle);

    // METHODS - GLOBAL / LOCAL POSITIONS
    //! Compute global positions and rotation matrices of objects from local coordinates
    void computeGlobalPositions(const bool a_frameOnly = true,
                                const cVector3d& a_globalPos = cVector3d(0.0, 0.0, 0.0),
                                const cMatrix3d& a_globalRot = cIdentity3d());

    //! Compute global position of current object only
    void computeGlobalCurrentObjectOnly(const bool a_frameOnly = true);

    //! Compute collision detection using collision trees
    virtual bool computeCollisionDetection(cVector3d& a_segmentPointA, cVector3d& a_segmentPointB,
         cGenericObject*& a_colObject, cTriangle*& a_colTriangle, cVector3d& a_colPoint,
         double& a_colSquareDistance, const bool a_visibleObjectsOnly, int a_proxyCall,
         cGenericPointForceAlgo* force_algo=0);

    // METHODS - GRAPHIC RELATED
    //! Set the display status of the object.
    void setShow(const bool a_show, const bool a_affectChildren = false);
    //! Read the display status of object.
    bool getShow() const { return (m_show); }
    //! Set the display status of the tree.
    void setShowTree(const bool a_showTree, const bool a_affectChildren = false);
    //! Read the display status of the tree.
    bool getShowTree() const { return (m_showTree); }
    //! Set the tree color.
    void setTreeColor(const cColorf& a_treeColor, const bool a_affectChildren  = false);
    //! Read the tree color.
    cColorf getTreeColor() const { return (m_treeColor); }
    //! Set the display status of the frame.
    void setShowFrame(const bool a_showFrame, const bool a_affectChildren  = false);
    //! Read the display status of the frame.
    bool getShowFrame(void) const { return (m_showFrame); }
    //! Set the display status of boundary box.
    void setShowBox(const bool iShowBox, const bool iAffectChildren = false);
    //! Read the display status of boundary box.
    bool getShowBox() const { return (m_showBox); }
    //! Set the color of boundary box.
    void setBoxColor(const cColorf& a_boxColor, const bool a_affectChildren = false);
    //! Read the color of boundary box
    cColorf getBoxColor() const { return (m_boundaryBoxColor); }
    //! Show collision tree
    void showCollisionTree(const bool a_showCollisionTree, const bool a_affectChildren = false);

    // METHODS - FRAME [X,Y,Z]
    //! Set the size of frame. The size corresponds to length of each axis X-Y-Z.
    bool setFrameSize(const double a_size, const bool a_affectChildren = false);
    //! Scale the object by passing a scaling factor
    void scale(const double a_scaleFactor);

    // METHODS - BOUNDARY BOX
    //! Read the size of frame.
    double getFrameSize() const { return (m_frameSize); }
    //! Read minimum point of boundary
    cVector3d getBoundaryMin() const { return (m_boundaryBoxMin); }
    //! Read maximum point of boundary
    cVector3d getBoundaryMax() const { return (m_boundaryBoxMax); }
    //! Compute Boundary box
    void computeBoundaryBox(const bool a_includeChildren);

    // METHODS - COLLISION DETECTOR
    //! Set a collision detector for current object
    void setCollisionDetector(cGenericCollision* a_collisionDetector)
         { m_collisionDetector = a_collisionDetector; }
    //! Get pointer to current collision detector.
    cGenericCollision* getCollisionDetector() { return (m_collisionDetector); }
    //! Set collision rendering properties
    void setCollisionDetectorProperties(unsigned int a_displayDepth,
         cColorf& a_color, const bool a_affectChildren);

    // METHODS - CHILDREN
    //! Read an object from children list.
    inline cGenericObject* getChild(const unsigned int a_index) const { return (m_children[a_index]); }
    //! Add an object to children list.
    void addChild(cGenericObject* a_object);
    //! Remove an object from children list.
    bool removeChild(cGenericObject* a_object);
    //! Remove an object from children list and delete it.
    bool deleteChild(cGenericObject *a_object);
    //! Clear all objects from children list.
    void clearAllChildren();
    //! Clear and delete all objects from children list.
    void deleteAllChildren();
    //! Return number of children
    int getNumChildren() { return m_children.size(); }

    // MEMBERS
    //! An arbitrary tag, not used by CHAI
    int m_tag;
    //! An arbitrary data pointer, not used by CHAI
    void* m_userData;

  protected:

    // MEMBERS - GENERAL
    //! Parent object.
    cGenericObject* m_parent;
    //! Selection flag
    bool m_flagSelected;
    //! List of child objects.
    vector<cGenericObject*> m_children;

    // MEMBERS - POSITION & ORIENTATION
    //! Local position of object in relation to parent object.
    cVector3d m_localPos;
    //! Global position relative to world coordinate system.
    cVector3d m_globalPos;
    //! Local rotation matrix in relation to parent object.
    cMatrix3d m_localRot;
    //! Global rotation matrix relative to world coordinate system.
    cMatrix3d m_globalRot;

    // MEMBERS - BOUDARY BOX
    //! Minimum position of boundary box
    cVector3d m_boundaryBoxMin;
    //! Maximum position of boundary box
    cVector3d m_boundaryBoxMax;

    // MEMBERS - FRAME [X,Y,Z]
    //! Size of graphical representation of frame (X-Y-Z).
    double m_frameSize;

    // MEMBERS - GRAPHIC RELATED
    //! If \b true shape of the object (mesh for instance) is displayed.
    bool m_show;
    //! If \b true, the frame (X-Y-Z) is displayed.
    bool m_showFrame;
    //! If \b true, boudary box is displayed.
    bool m_showBox;
    //! If \b true, the skeleton tree is rendered.
    bool m_showTree;
    //! If \b true, the collision tree is displayed (if available).
    bool m_showCollisionTree;
    //! Color of tree.
    cColorf m_treeColor;
    //! Color of bounding box
    cColorf m_boundaryBoxColor;

    // MEMBERS - COLLISION DETECTION
    //! Collision detection algorithm.
    cGenericCollision* m_collisionDetector;

    // VIRTUAL METHODS:
    //! Render shape of current object in OpenGL.
    virtual void render(const int a_renderMode=0) {};
    //! Update global positions.
    virtual void updateGlobalPositions(const bool a_frameOnly) {};
    //! Update bounding box of current object
    virtual void updateBoundaryBox() {};
    //! Scale object of defined scale factor
    virtual void scaleObject(double a_scaleFactor) {};

    // MEMBERS:
    //! OpenGL matrix describing position and orientation transformation.
    cMatrixGL m_frameGL;

  public:
    // METHODS:
    //! Render entire scene graph from this current object.
    void renderSceneGraph(const int a_renderMode=0);

    
};


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

