//===========================================================================
//  - UNIT -
//
//    Copyright (C) 2003. Stanford University - Robotics Laboratory
/*!
      \author     Collision detection algorithm by Joel Brown
      \author     jbrown@cs.stanford.edu
      \author     Based on code by Stephen Sorkin
      \author     ssorkin@cs.stanford.edu
      \author     Integrated into CHAI by Christopher Sewell
      \author     csewell@stanford.edu
      \file       CCollisionSpheres.h
      \version    1.0
      \date       09/2003
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CCollisionSpheresH
#define CCollisionSpheresH
#ifdef _MSVC
// Turn off annoying compiler warnings
#pragma warning(disable: 4305)
#pragma warning(disable: 4800) 
#pragma warning(disable: 4786)
#endif
//---------------------------------------------------------------------------
#include "CMaths.h"
#include "CCollisionSpheresGeometry.h"
#include "CVertex.h"
#include "CTriangle.h"
#include "CGenericCollision.h"
#include "CDraw3D.h"
#include "CColor.h"
#include "CMesh.h"
#include <assert.h>
#include <list>
#include <queue>
#include <vector>
#include "gl/glu.h"
//---------------------------------------------------------------------------
// TYPEDEFS:
//! List of shape primitives.
typedef vector<cCollisionSpheresGenericShape *> Plist;

// FORWARD DECLARATIONS:
//! Nodes of the collision sphere tree (abstract class).
class cCollisionSpheresSphere;
//! Leaf nodes of the sphere tree.
class cCollisionSpheresLeaf;
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class    cCollisionSpheres
      \brief    cCollisionSpheres provides methods to create a sphere tree for
                collision detection, and to use this tree to check for the
                intersection of a line with a mesh.
*/
//===========================================================================
class cCollisionSpheres : public cGenericCollision
{
  public:
    // CONSTRUCTOR:
    //! Constructor of cCollisionSpheres.
    cCollisionSpheres(vector<cTriangle>* a_triangles, bool a_useNeighbors);
    virtual ~cCollisionSpheres();

    // METHODS:
    //! Build the sphere tree based on the given triangles.
    void initialize();
    //! Draw the collision spheres at the given level.
    void render();
    //! Return a list of triangles intersected by the given line, if any.
    bool computeCollision(cVector3d& a_segmentPointA,
        cVector3d& a_segmentPointB, cGenericObject*& a_colObject,
        cTriangle*& a_colTriangle, cVector3d& a_colPoint,
        double& a_colSquareDistance, int a_proxyCall = -1);

    // PROPERTIES:
    //! The sphere at the root of the sphere tree.
    cCollisionSpheresSphere* m_root;
    //! Pointer to the list of triangles in the mesh.
    vector<cTriangle>* m_trigs;
    //! Triangle returned by last successful collision test.
    cTriangle* m_lastCollision;
    //! Use neighbor list to speed up collision detection?
    bool m_useNeighbors;
    //! Beginning of list of leaf nodes.
    cCollisionSpheresLeaf* m_firstLeaf;
};


//===========================================================================
/*!
      \class    cCollisionSpheresSphere
      \brief    cCollisionSpheresSphere is an abstract class for nodes of
                the collision sphere tree.
*/
//===========================================================================
class cCollisionSpheresSphere
{
  // FRIENDS:
  //! Internal nodes of the collision sphere tree.
  friend class cCollisionSpheresNode;
  //! Leaf nodes of the collision sphere tree.
  friend class cCollisionSpheresLeaf;

  public:
    // CONSTRUCTORS:
    //! Constructor of cCollisionSpheresSphere.
    cCollisionSpheresSphere(cCollisionSpheresSphere *parent_);
    //! Default constructor of cCollisionSpheresSphere.
    cCollisionSpheresSphere() {};
    //! Destructor of cCollisionsSpheresSphere.
    virtual ~cCollisionSpheresSphere() {};

    // METHODS:
    //! Return the center of the sphere.
    inline const cVector3d &getCenter() {return center;}
    //! Return the radius of the sphere.
    inline double getRadius() {return radius;}
    //! Return whether the node is a leaf node.
    virtual bool isLeaf() = 0;
    //! Draw the collision sphere if at the given level.
    virtual void draw(int depth) = 0;
    //! Calculate the distance between the two given collision spheres.
    static bool cCollisionSpheresSphere::dist(cCollisionSpheresSphere *sa,
                cGenericObject*& a_colObject,
                cTriangle*& a_colTriangle, cVector3d& a_colPoint,
                double& a_colSquareDistance,
                cCollisionSpheresSphere *sb);

  protected:
    // PROPERTIES:
    //! The parent of the node in the tree.
    cCollisionSpheresSphere *parent;
    //! The center of the node.
    cVector3d center;
    //! The radius of the node.
    double radius;
    //! The level of the node in the tree.
    int level;
};


//===========================================================================
/*!
      \class    cCollisionSpheresNode
      \brief    cCollisionSpheresNode defines internal nodes of the collision
                sphere tree and provides methods for constructing the nodes
                and using them to determine collisions.
*/
//===========================================================================
class cCollisionSpheresNode : public cCollisionSpheresSphere
{
  public:
    // CONSTRUCTORS:
    //! Constructor of cCollisionSpheresNode.
    cCollisionSpheresNode(Plist &primList,
                          cCollisionSpheresSphere *parent_ = NULL);
    //! Constructor of cCollisionSpheresNode.
    cCollisionSpheresNode(std::vector<cTriangle>* tris,
                          cCollisionSpheresSphere *parent_ = NULL);
    //! Default constructor of cCollisionSpheresNode.
    cCollisionSpheresNode() {};
    //! Destructor of cCollisionSpheseNode.
    ~cCollisionSpheresNode() {};

    // METHODS:
    //! Create subtrees by splitting primitives into left and right lists.
    void ConstructChildren(Plist &primList);
    //! Return whether the node is a leaf node. (It is not.)
    bool isLeaf()  {return false;}
    //! Draw the collision sphere if at the given level.
    void draw(int depth);
    //! Calculate distance between given nodes, the first an internal node.
    static bool dist(cCollisionSpheresNode *sa,
                 cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                 cVector3d& a_colPoint, double& a_colSquareDistance,
                 cCollisionSpheresSphere *sb);
    //! Exchange the two given pointers.
    static void swapptr(void **a, void **b);

  private:
    // PROPERTIES:
    //! Pointer to the left child in the sphere tree.
    cCollisionSpheresSphere *left;
    //! Pointer to the right child in the sphere tree.
    cCollisionSpheresSphere *right;
};


//===========================================================================
/*!
      \class    cCollisionSpheresLeaf
      \brief    cCollisionSpheresLeaf defines leaf nodes of the collision
                sphere tree and provides methods for constructing the nodes
                and using them to determine collisions.
*/
//===========================================================================
class cCollisionSpheresLeaf : public cCollisionSpheresSphere
{
  public:
    // CONSTRUCTORS:
    //! Constructor of cCollisionSpheresLeaf.
    cCollisionSpheresLeaf(cCollisionSpheresGenericShape *prim_,
                          cCollisionSpheresSphere *parent_ = NULL);
    //! Constructor of cCollisionSpheresLeaf.
    cCollisionSpheresLeaf(cTriangle *tri,
                          cCollisionSpheresSphere *parent_ = NULL);
    //! Default constructor of cCollisionSpheresLeaf
    cCollisionSpheresLeaf() {};
    //! Destructor of cCollisionSpheresLeaf()
    ~cCollisionSpheresLeaf() { if (prim) delete prim; };

    // METHODS:
    //! Return whether the node is a leaf node. (It is.)
    bool isLeaf()  {return true;}
    //! Draw the collision sphere if at the given level.
    void draw(int depth);
    //! Calculate the distance between the two given leaf nodes.
    static bool cCollisionSpheresLeaf::dist(cCollisionSpheresLeaf *sa,
                 cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                 cVector3d& a_colPoint, double& a_colSquareDistance,
                 cCollisionSpheresLeaf *sb);

    // PROPERTIES:
    //! The shape primitive bounded by the sphere leaf.
    cCollisionSpheresGenericShape *prim;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------


