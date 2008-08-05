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
      \file       CCollisionSpheres.cpp
      \version    1.0
      \date       09/2003
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CCollisionSpheres.h"
#include <algorithm>

// INITIALIZATIONS:
cCollisionSpheresNode* g_nextInternalNode;
cCollisionSpheresLeaf* g_nextLeafNode;
const double LITTLE = 1e-10;
const double LARGE = 1e10;
//---------------------------------------------------------------------------


//===========================================================================
/*!
    Constructor of cCollisionSpheres.

    \fn     cCollisionSpheres::cCollisionSpheres(vector<cTriangle>* a_triangles,
                                                 bool a_useNeighbors)
    \param  a_triangle      Pointer to array of triangles.
    \param  a_useNeighbors  Use neighbor lists to speed up collision detection?
*/
//===========================================================================
cCollisionSpheres::cCollisionSpheres(vector<cTriangle>* a_triangles,
                                     bool a_useNeighbors)
{
    m_trigs = a_triangles;
    m_useNeighbors = a_useNeighbors;
    m_root = NULL;
}


//===========================================================================
/*!
    Destructor of cCollisionSpheres.

    \fn     cCollisionSpheres::~cCollisionSpheres()
*/
//===========================================================================
cCollisionSpheres::~cCollisionSpheres()
{
  if (m_root != NULL)
    delete[] m_root;

  if ((m_trigs) && (m_trigs->size() > 1) && (m_firstLeaf))
    delete[] m_firstLeaf;
}


//===========================================================================
/*!
      Build the sphere tree based on the given triangles.

      \fn       void cCollisionSpheres::initialize()
*/
//===========================================================================
void cCollisionSpheres::initialize()
{
    int l_numTriangles = m_trigs->size();
    m_root = NULL;
    if (l_numTriangles > 0)
    {

      // allocate leaf nodes
      g_nextLeafNode = new cCollisionSpheresLeaf[l_numTriangles];
      m_firstLeaf = g_nextLeafNode;

      // if there are more than one triangle, allocate internal nodes
      if (l_numTriangles > 1)
      {
        g_nextInternalNode = new cCollisionSpheresNode[l_numTriangles-1];
        m_root = g_nextInternalNode;
        new(g_nextInternalNode++) cCollisionSpheresNode(m_trigs);
      }
      else
      {
        new(&m_firstLeaf[0]) cCollisionSpheresLeaf(&((*m_trigs)[0]));
        m_root = g_nextLeafNode;
      }
    }
    else
    {
      m_root = 0;
    }

    m_lastCollision = NULL;
}


//===========================================================================
/*!
      Draw the collision spheres at the given level.

      \fn       void cCollisionSpheres::draw(int depth)
      \param    depth   Only draw nodes at this depth in the tree.
*/
//===========================================================================
void cCollisionSpheres::render()
{
    if (m_root != NULL)
    {

      // set rendering settings
      glDisable(GL_LIGHTING);
      glLineWidth(1.0);
      glColor4fv(m_material.m_ambient.pColor());

      // render tree
      m_root->draw(m_displayDepth);

      // restore settings
      glEnable(GL_LIGHTING);
    }
}


//===========================================================================
/*!
      Return a list of triangles intersected by the given line, if any.

      \fn       bool computeCollision(cVector3d& a_segmentPointA,
                       cVector3d& a_segmentPointB, cGenericObject*& a_colObject,
                       cTriangle*& a_colTriangle, cVector3d& a_colPoint,
                       double& a_colSquareDistance, int a_num)

      \param  a_segmentPointA  First endpoint of the line.
      \param  a_segmentPointB  Second endpoint of the line.
      \param  a_colObject    Returns object with which collision occurs.
      \param  a_colTriangle  Returns triangle with which collision occurs.
      \param  a_colPoint     Returns point at which collision occurs.
      \param  a_colSquareDistance  Returns distance squared to collision pt.
      \param  a_proxyCall  If this is > 0, this is a call from a proxy, and the value
                           of a_proxyCall specifies which call this is.  -1 for non-proxy
                           calls.

      \return Return whether triangle collides with given line.
*/
//===========================================================================
bool cCollisionSpheres::computeCollision(cVector3d& a_segmentPointA,
        cVector3d& a_segmentPointB, cGenericObject*& a_colObject,
        cTriangle*& a_colTriangle, cVector3d& a_colPoint,
        double& a_colSquareDistance, int a_proxyCall)
{

    if (m_root == 0)
    {
      m_lastCollision = 0;
      return 0;
    }

    // if this is a subsequent call from the proxy algorithm after a detecting
    // an initial collision, only neighbors of the triangle from the first
    // collision detection need to be checked
    if ((m_useNeighbors) && (a_proxyCall > 1) && (m_root != NULL) &&
        (m_lastCollision != NULL) && (m_lastCollision->m_neighbors != NULL))
    {
      cVector3d dir;
      a_segmentPointB.subr(a_segmentPointA, dir);
      cGenericObject* colObject;
      cTriangle* colTriangle;
      cVector3d colPoint;
      double colSquareDistance = dir.lengthsq();
      bool firstHit = true;

      // check each neighbor, and find the closest for which there is a
      // collision, if any
      for (unsigned int i=0; i<m_lastCollision->m_neighbors->size(); i++)
      {
        if (((*(m_lastCollision->m_neighbors))[i])->computeCollision(
                                    a_segmentPointA,
                                    dir,
                                    colObject,
                                    colTriangle,
                                    colPoint,
                                    colSquareDistance
                                  ))
        {
          if (firstHit || (colSquareDistance < a_colSquareDistance))
          {
            m_lastCollision = colTriangle;
            a_colObject = colObject;
            a_colTriangle = colTriangle;
            a_colPoint = colPoint;
            a_colSquareDistance = colSquareDistance;
            firstHit = false;
          }
        }
      }
      if (!firstHit)
        return true;
      m_lastCollision = NULL;
      return false;
    }

    // otherwise, if this is the first call in an iteration of the proxy
    // algorithm (or a call from any other algorithm), check the sphere tree

    // create a cCollisionSpheresLine object and enclose it in a sphere leaf
    cCollisionSpheresLine* curLine = new cCollisionSpheresLine(a_segmentPointA,
                                                              a_segmentPointB);
    cCollisionSpheresLeaf* lineSphere = new cCollisionSpheresLeaf(curLine);

    // if the root is null, there is no collision
    if (m_root == NULL)
    {
      m_lastCollision = NULL;
      return false;
    }

    // otherwise, compare the line leaf to the sphere tree rooted at m_root
    cVector3d dir;
    a_segmentPointB.subr(a_segmentPointA, dir);
    a_colSquareDistance = dir.lengthsq();
    bool result = cCollisionSpheresSphere::dist(m_root, a_colObject,
      a_colTriangle, a_colPoint, a_colSquareDistance, lineSphere);
    if (result)
      m_lastCollision = a_colTriangle;
    else
      m_lastCollision = NULL;
    delete curLine;
    delete lineSphere;
    return result;
}


//===========================================================================
/*!
      Constructor of cCollisionSpheresSphere.

      \fn         cCollisionSpheresSphere::cCollisionSpheresSphere(
                                         cCollisionSpheresSphere *parent_)
      \param      parent_     Pointer to parent of this node in the sphere tree.
      \return     Return a pointer to new cCollisionSpheresSphere instance.
*/
//===========================================================================
cCollisionSpheresSphere::cCollisionSpheresSphere(
                                       cCollisionSpheresSphere *parent_)
{
    parent = parent_;
    if (parent)
      level = parent->level + 1;
    else
      level = 0;
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Calculate the distance between the two given collision spheres.

      \fn      double cCollisionSpheresSphere::dist(cCollisionSpheresSphere *sa,
                cCollisionSpheresPose *pa, cList<cCollisionSpheresGenericShape>*
                pra, cCollisionSpheresSphere *sb, cCollisionSpheresPose *pb,
                cList<cCollisionSpheresGenericShape>* prb)
      \param   sa            The root of one sphere tree to check for collision.
      \param   a_colObject   Returns object with which collision occurs.
      \param   a_colTriangle Returns triangle with which collision occurs.
      \param   a_colPoint    Returns point at which collision occurs.
      \param   a_colSquareDistance  Returns distance squared to collision pt.
      \param   sb            Root of other sphere tree to check for collision.
      \return  Return whether any primitives within the two sphere trees collide.
*/
//===========================================================================
bool cCollisionSpheresSphere::dist(cCollisionSpheresSphere *sa,
                cGenericObject*& a_colObject,
                cTriangle*& a_colTriangle, cVector3d& a_colPoint,
                double& a_colSquareDistance,
                cCollisionSpheresSphere *sb)
{
    // if first sphere is an internal node, call internal node dist function
    if (!sa->isLeaf())
      return cCollisionSpheresNode::dist((cCollisionSpheresNode *)sa,
             a_colObject, a_colTriangle, a_colPoint, a_colSquareDistance, sb );

    // if second sphere is an internal node, call internal node dist function
    if (!sb->isLeaf())
      return cCollisionSpheresNode::dist((cCollisionSpheresNode *)sb,
             a_colObject, a_colTriangle, a_colPoint, a_colSquareDistance, sa);

    // if both spheres are leaves, call leaf dist function
    return cCollisionSpheresLeaf::dist((cCollisionSpheresLeaf *)sa,
           a_colObject, a_colTriangle, a_colPoint, a_colSquareDistance,
           (cCollisionSpheresLeaf *)sb);
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Constructor of cCollisionSpheresNode.

      \fn         cCollisionSpheresNode::cCollisionSpheresNode(Plist &primList,
                                         cCollisionSpheresSphere *parent_)
      \param      primList    List of shape primitives to be enclosed in the
                              subtree rooted at this internal node.
      \param      parent      Pointer to the parent of this node in the tree.
      \return     Return a pointer to new cCollisionSpheresNode instance.
*/
//===========================================================================
cCollisionSpheresNode::cCollisionSpheresNode(Plist &primList,
              cCollisionSpheresSphere *parent_):cCollisionSpheresSphere(parent_)
{
    // create the left and right subtrees of this node
    ConstructChildren(primList);
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Constructor of cCollisionSpheresNode.

      \fn         cCollisionSpheresNode::cCollisionSpheresNode(
                       cList<cTriangle>* tris, cCollisionSpheresSphere *parent_)
      \param      tris    Pointer to list of triangles to use for collision
                          detection.
      \param      parent  Pointer to the parent of this node in sphere tree.
      \return     Return a pointer to new cCollisionSpheresNode instance.
*/
//===========================================================================
cCollisionSpheresNode::cCollisionSpheresNode(std::vector<cTriangle>* tris,
              cCollisionSpheresSphere *parent_):cCollisionSpheresSphere(parent_)
{

    // create cCollisionSpheresTri object for each cTriangle object
    Plist primList;
    for (unsigned int i=0; i<tris->size(); i++)
    {
      cVector3d vpos =  (*tris)[i].getVertex0()->getPos();

      cCollisionSpheresPoint* p1 = new cCollisionSpheresPoint(
                                            vpos.x, vpos.y, vpos.z);

      vpos =  (*tris)[i].getVertex1()->getPos();
      cCollisionSpheresPoint* p2 = new cCollisionSpheresPoint(
                                            vpos.x, vpos.y, vpos.z);

      vpos =  (*tris)[i].getVertex2()->getPos();
      cCollisionSpheresPoint* p3 = new cCollisionSpheresPoint(
                                            vpos.x, vpos.y, vpos.z);
      cCollisionSpheresTri* temp = new cCollisionSpheresTri(p1, p2, p3);
      temp->setOriginal(&(*tris)[i]);
      primList.insert(primList.end(), temp);
    }

    // create left and right subtrees of this node
    ConstructChildren(primList);
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Create subtrees by splitting primitives into left and right lists.

      \fn       void cCollisionSpheresNode::ConstructChildren(Plist &primList)
      \param    primList   List of shape primitives to be split into left
                           and right subtrees.
*/
//===========================================================================
void cCollisionSpheresNode::ConstructChildren(Plist &primList)
{
    Plist::iterator primIter;
    assert(primList.size() >= 2);

    // prepare to split primitives into left and right subtrees
    Plist leftList, rightList;
    double min[3] = {LARGE, LARGE, LARGE};
    double max[3] = {-LARGE, -LARGE, -LARGE};
    int split = 0;

    // find minimum and maximum values for each coordinate of primitves' centers
    for (primIter = primList.begin(); primIter != primList.end(); primIter++)
    {
      cCollisionSpheresGenericShape *cur = *primIter;
      const cVector3d &center = cur->getCenter();
      for (int i = 0; i < 3; i++)
      {
        if (center.get(i) < min[i])
          min[i] = center.get(i);
        if (center.get(i) > max[i])
          max[i] = center.get(i);
      }
    }

    // find the coordinate index with the largest range (max to min)
    if ((max[1] - min[1]) > (max[split] - min[split]))
      split = 1;
    if ((max[2] - min[2]) > (max[split] - min[split]))
      split = 2;

    // sort the primitives according to the coordinate with largest range
    cCollisionSpheresGenericShape::split = split;
    std::sort(primList.begin(), primList.end());

    unsigned int s;

    // put first half in left subtree and second half in right subtree
    for (s=0; s<primList.size()/2; s++)
      leftList.insert(leftList.end(), primList[s]);
    for (s=primList.size()/2; s<primList.size(); s++)
      rightList.insert(rightList.end(), primList[s]);

    // if the left subtree is empty, transfer one from the right list:
    if (leftList.size() == 0)
    {
      leftList.insert(leftList.begin(), *rightList.begin());
      rightList.erase(rightList.begin());
    }

    // create new internal nodes as roots for left and right subtree lists, or
    // a leaf node if the subtree list has only one primitive
    if (leftList.size() == 1)
      left = new(g_nextLeafNode++) cCollisionSpheresLeaf(*(leftList.begin()), this);
    else
      left = new(g_nextInternalNode++) cCollisionSpheresNode(leftList, this);
    if (rightList.size() == 1)
      right = new(g_nextLeafNode++) cCollisionSpheresLeaf(*(rightList.begin()), this);
    else
      right = new(g_nextInternalNode++) cCollisionSpheresNode(rightList, this);

    // get centers and radii of left and right children
    const cVector3d &lc = left->center;
    const cVector3d &rc = right->center;
    double lr = left->getRadius();
    double rr = right->getRadius();

    // compute new radius
    double dist = lc.distance(rc);
    radius = (dist + lr + rr) / 2.0;

    // compute new center
    if (dist != 0)
    {
      double lambda = (radius - lr) / dist;
      center.x = lc.x + lambda*(rc.x - lc.x);
      center.y = lc.y + lambda*(rc.y - lc.y);
      center.z = lc.z + lambda*(rc.z - lc.z);
    }
    else
    {
      center = lc;
    }

    // if one sphere is entirely contained within the other, set this sphere
    // to the larger one
    if (lr > dist + rr)
    {
      center = lc;
      radius = lr;
    }
    if (rr > dist + lr)
    {
      center = rc;
      radius = rr;
    }
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Draw the collision sphere if at the given level.

      \fn       void cCollisionSpheresNode::draw(int depth)
      \param    depth   Only draw nodes at this depth in the tree.
*/
//===========================================================================
void cCollisionSpheresNode::draw(int depth)
{
    if (level == depth)
    {
      cColorf color;
      color.set(0.8, 0.8, 0.8, 0.50);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glPushMatrix();
      glTranslatef(center.x, center.y, center.z);
      glColor4fv( (const float *)&color);
      cDrawSphere(radius, 16, 16);
      glPopMatrix();
    }
    if (left) left->draw(depth);
    if (right) right->draw(depth);
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Exchange the two given pointers.

      \fn       void cCollisionSpheresNode::swapptr(void **a, void **b)
      \param    a     First pointer to be swapped.
      \param    b     Second pointer to be swapped.
*/
//===========================================================================
void cCollisionSpheresNode::swapptr(void **a, void **b)
{
    void *temp;
    temp = *a;
    *a = *b;
    *b = temp;
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Calculate distance between given nodes, the first an internal node.

      \fn      double cCollisionSpheresNode::dist(cCollisionSpheresNode *sa,
                     cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                     cVector3d& a_colPoint, double& a_colSquareDistance,
                     cCollisionSpheresSphere *sb)
      \param   sa            The root of one sphere tree to check for collision.
      \param   a_colObject   Returns object with which collision occurs.
      \param   a_colTriangle Returns triangle with which collision occurs.
      \param   a_colPoint    Returns point at which collision occurs.
      \param   a_colSquareDistance  Returns distance squared to collision pt.
      \param   sb            Root of other sphere tree to check for collision.
      \return  Return whether any primitives within the two sphere trees collide.
*/
//===========================================================================
bool cCollisionSpheresNode::dist(cCollisionSpheresNode *sa,
                 cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                 cVector3d& a_colPoint, double& a_colSquareDistance,
                 cCollisionSpheresSphere *sb)
{
    // if both are internal nodes, arrange that the larger one is first
    if (!sb->isLeaf() && (sa->radius < sb->radius))
      cCollisionSpheresNode::swapptr((void **)&sa, (void **)&sb);

    // if spheres don't overlap, there can be no collision
    double minSep = (sa->center).distance(sb->center);
    if ((minSep - (sa->radius + sb->radius)) >= LITTLE)
      return false;

    // initialize objects for calls to left and right subtrees
    cGenericObject* l_colObject, *r_colObject;
    cTriangle* l_colTriangle, *r_colTriangle;
    cVector3d l_colPoint, r_colPoint;
    double l_colSquareDistance = a_colSquareDistance;
    double r_colSquareDistance = a_colSquareDistance;
    bool l_result, r_result;

    // check for overlap of larger sphere's left subtree with smaller sphere
    l_result = cCollisionSpheresSphere::dist(sa->left, l_colObject,
             l_colTriangle, l_colPoint, l_colSquareDistance, sb);

    r_result = cCollisionSpheresSphere::dist(sa->right, r_colObject,
             r_colTriangle, r_colPoint, r_colSquareDistance, sb);

    // if there is an intersection in either subtree, return the closest one
    if ((l_result && !r_result) || (l_result && r_result &&
         (l_colSquareDistance <= r_colSquareDistance)))
    {
      a_colObject = l_colObject;
      a_colTriangle = l_colTriangle;
      a_colPoint = l_colPoint;
      a_colSquareDistance = l_colSquareDistance;
    }
    else
    {
      a_colObject = r_colObject;
      a_colTriangle = r_colTriangle;
      a_colPoint = r_colPoint;
      a_colSquareDistance = r_colSquareDistance;
    }
    return (l_result || r_result);
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Constructor of cCollisionSpheresLeaf.

      \fn         cCollisionSpheresLeaf::cCollisionSpheresLeaf(
                               cCollisionSpheresGenericShape *prim_,
                               cCollisionSpheresSphere *parent_)
      \param      prim     Pointer to a shape primitive to be enclosed by
                           the new sphere leaf node.
      \param      parent   Pointer to the parent of this node in sphere tree.
      \return     Return a pointer to new cCollisionSpheresLeaf instance.
*/
//===========================================================================
cCollisionSpheresLeaf::cCollisionSpheresLeaf(cCollisionSpheresGenericShape
      *prim_, cCollisionSpheresSphere *parent_):cCollisionSpheresSphere(parent_)
{
    assert(prim_);
    prim = prim_;
    prim->sphere = this;
    radius = prim->getRadius();
    center = prim->getCenter();
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Constructor of cCollisionSpheresLeaf.

      \fn         cCollisionSpheresLeaf::cCollisionSpheresLeaf(cTriangle* tri,
                                         cCollisionSpheresSphere *parent_)
      \param      tri     Pointer to triangle to be enclosed by new sphere leaf.
      \param      parent  Pointer to the parent of this node in the sphere tree.
      \return     Return a pointer to new cCollisionSpheresLeaf instance.
*/
//===========================================================================
cCollisionSpheresLeaf::cCollisionSpheresLeaf(cTriangle* tri,
              cCollisionSpheresSphere *parent_):cCollisionSpheresSphere(parent_)
{
    cVector3d vpos = tri->getVertex0()->getPos();
    cCollisionSpheresPoint* p1 = new cCollisionSpheresPoint(
                                            vpos.x, vpos.y, vpos.z);
    vpos = tri->getVertex1()->getPos();
    cCollisionSpheresPoint* p2 = new cCollisionSpheresPoint(
                                            vpos.x, vpos.y, vpos.z);
    vpos = tri->getVertex2()->getPos();
    cCollisionSpheresPoint* p3 = new cCollisionSpheresPoint(
                                            vpos.x, vpos.y, vpos.z);
    cCollisionSpheresTri* temp = new cCollisionSpheresTri(p1, p2, p3);
    temp->setOriginal(tri);
    prim = temp;
    prim->sphere = this;
    radius = prim->getRadius();
    center = prim->getCenter();
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Draw the collision sphere if at the given level.

      \fn       void cCollisionSpheresLeaf::draw(int depth)
      \param    depth   Only draw nodes at this depth in the tree.
*/
//===========================================================================
void cCollisionSpheresLeaf::draw(int depth)
{
    if (level == depth)
    {
      cColorf color;
      color.set(0.8, 0.8, 0.8, 0.2);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glPushMatrix();
      glTranslatef(center.x, center.y, center.z);
      glColor4fv( (const float *)&color);
      cDrawSphere(radius, 16, 16);
      glPopMatrix();
    }
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Calculate the distance between the two given leaf nodes.

      \fn      double cCollisionSpheresLeaf::dist(cCollisionSpheresLeaf *sa,
                 cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                 cVector3d& a_colPoint, double& a_colSquareDistance,
                 cCollisionSpheresLeaf *sb)
      \param   sa            The root of one sphere tree to check for collision.
      \param   a_colObject   Returns object with which collision occurs.
      \param   a_colTriangle Returns triangle with which collision occurs.
      \param   a_colPoint    Returns point at which collision occurs.
      \param   a_colSquareDistance  Returns distance squared to collision pt.
      \param   sb            Root of other sphere tree to check for collision.
      \return  Return whether primitives of the two leaves collide.
*/
//===========================================================================
bool cCollisionSpheresLeaf::dist(cCollisionSpheresLeaf *sa,
                 cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                 cVector3d& a_colPoint, double& a_colSquareDistance,
                 cCollisionSpheresLeaf *sb)
{
    return sa->prim->dist(sb->prim, a_colObject, a_colTriangle, a_colPoint,
            a_colSquareDistance);
}

