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
      \file       CCollisionSpheresGeometry.cpp
      \version    1.0
      \date       09/2003
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CVertex.h"
#include "CTriangle.h"
#include "CCollisionSpheresGeometry.h"
#include "CCollisionSpheres.h"
//---------------------------------------------------------------------------

int cCollisionSpheresGenericShape::split = 0;
bool cCollisionSpheresGenericShape::operator<(cCollisionSpheresGenericShape* other)
{
  return (getCenter().get(split) < other->getCenter().get(split));
}


//===========================================================================
/*!
      Constructor of cCollisionSpheresPoint.

      \fn         cCollisionSpheresPoint::cForceAlgoProxy(double x_, double y_,
                                        double z_)
      \param      x_    X-coordinate of point.
      \param      y_    Y-coordinate of point.
      \param      z_    Z-coordinate of point.
      \return     Return a pointer to new cCollisionSpheresPoint instance.
*/
//===========================================================================
cCollisionSpheresPoint::cCollisionSpheresPoint(double x_, double y_, double z_)
{
    set(x_, y_, z_);
    normal.set(0.0, 0.0, 0.0);
}


//===========================================================================
/*!
      Constructor of cCollisionSpheresPoint.

      \fn         cCollisionSpheresPoint::cCollisionSpheresPoint(double x_[3])
      \param      x_[3]     Array of the three coordinates for the point.
      \return     Return a pointer to new cCollisionSpheresPoint instance.
*/
//===========================================================================
cCollisionSpheresPoint::cCollisionSpheresPoint(double x_[3])
{
    set(x_[0], x_[1], x_[2]);
    normal.set(0.0, 0.0, 0.0);
}


//===========================================================================
/*!
      Copy constructor of cCollisionSpheresPoint.

      \fn         cCollisionSpheresPoint &cCollisionSpheresPoint::operator=
                                         (const cVector3d &other)
      \param      other   Reference to the other cCollisionSpheresPoint.
      \return     Return a pointer to new cCollisionSpheresPoint instance.
*/
//===========================================================================
cCollisionSpheresPoint &cCollisionSpheresPoint::operator=(const cVector3d &other)
{
    set(other.x, other.y, other.z);
    normal.set(0.0, 0.0, 0.0);
    return *this;
}


//===========================================================================
/*!
      Set position of point given three doubles.

      \fn         void cCollisionSpheresPoint::set(double x_, double y_,
                                                   double z_)
      \param      x_      X-coordinate of the point.
      \param      y_      Y-coordinate of the point.
      \param      z_      Z-coordinate of the point.
*/
//===========================================================================
void cCollisionSpheresPoint::set(double x_, double y_, double z_)
{
    pos.x = x_;
    pos.y = y_;
    pos.z = z_;
    procChange();
}


//===========================================================================
/*!
      Set position of one coordinate given index and value.

      \fn         void cCollisionSpheresPoint::set(int coord, double value)
      \param      coord     The index of the coordinate to set (0, 1, or 2).
      \param      value     The value to set this coordinate to.
*/
//===========================================================================
void cCollisionSpheresPoint::set(int coord, double value)
{
    switch (coord)
    {
      case 0 : pos.x = value;
      case 1 : pos.y = value;
      case 2 : pos.z = value;
    }
    procChange();
}


//===========================================================================
/*!
      Set position of point given array of three doubles.

      \fn         void cCollisionSpheresPoint::set(double x_[3])
      \param      x_[3]     The array containing the x, y, and z coordinates.
*/
//===========================================================================
void cCollisionSpheresPoint::set(double x_[3])
{
    set(x_[0], x_[1], x_[2]);
    procChange();
}


//===========================================================================
/*!
      Return the edge, if any, from this point to the given point.

      \fn         cCollisionSpheresEdge *cCollisionSpheresPoint::getEdgeTo(
                                         cCollisionSpheresPoint *other)
      \param      other     The point which, along with "this", forms the edge.
      \return     Return a pointer to the edge connecting "this" to the given
                  point.
*/
//===========================================================================
cCollisionSpheresEdge *cCollisionSpheresPoint::getEdgeTo(cCollisionSpheresPoint
                                                         *other)
{
    PtEmap::iterator edgeIter;
    edgeIter = edgeMap.find(other);
    if (edgeIter == edgeMap.end())
      return new cCollisionSpheresEdge(this, other);
    return edgeIter->second;
}


//===========================================================================
/*!
      Recalculate properties for associated edges and triangles.

      \fn         void cCollisionSpheresPoint::procChange()
*/
//===========================================================================
void cCollisionSpheresPoint::procChange()
{
    PtEmap::iterator edgeIter;
    Tlist::iterator triIter;
    for (edgeIter = edgeMap.begin(); edgeIter != edgeMap.end(); edgeIter++)
      edgeIter->second->procChange();
    for (triIter = triList.begin(); triIter != triList.end(); triIter++)
      (*triIter)->procChange();
}


//===========================================================================
/*!
      Calculate normal of the point from normals of associated triangles.

      \fn         void cCollisionSpheresPoint::procNormal()
*/
//===========================================================================
void cCollisionSpheresPoint::procNormal()
{
    Tlist::iterator triIter;
    for (triIter = triList.begin(); triIter != triList.end(); triIter++)
      normal = cAdd(normal, (*triIter)->getNormal());
    normal.normalize();
}


//===========================================================================
/*!
      Constructor of cCollisionSpheresEdge.

      \fn         cCollisionSpheresEdge::cCollisionSpheresEdge(
                        cCollisionSpheresPoint *a, cCollisionSpheresPoint *b)
      \param      a     First vertex of the edge.
      \param      b     Second vertex of the edge.
      \return     Return a pointer to new cCollisionSpheresEdge instance.
*/
//===========================================================================
cCollisionSpheresEdge::cCollisionSpheresEdge(cCollisionSpheresPoint *a,
                                             cCollisionSpheresPoint *b)
{
    end[0] = a;
    end[1] = b;
    setup();
}


//===========================================================================
/*!
      Constructor of cCollisionSpheresEdge.

      \fn         cCollisionSpheresEdge::cCollisionSpheresEdge(
                          cCollisionSpheresPoint *end_[2])
      \param      end_[2]   Array of two vertices of the edge.
      \return     Return a pointer to new proxy instance.
*/
//===========================================================================
cCollisionSpheresEdge::cCollisionSpheresEdge(cCollisionSpheresPoint *end_[2])
{
    end[0] = end_[0];
    end[1] = end_[1];
    setup();
}


//===========================================================================
/*!
      Enter the two vertices of the edge in the edge map.

      \fn         void cCollisionSpheresEdge::setup()
*/
//===========================================================================
void cCollisionSpheresEdge::setup()
{
    end[0]->edgeMap.insert(PtEmap::value_type(end[1], this));
    end[1]->edgeMap.insert(PtEmap::value_type(end[0], this));
    procChange();
}


//===========================================================================
/*!
      Recalculate properties of the edge from its vertices.

      \fn         void cCollisionSpheresEdge::procChange()
*/
//===========================================================================
void cCollisionSpheresEdge::procChange()
{
    d = cSub((*end[1]).pos, (*end[0]).pos);
    D = d.x*d.x + d.y*d.y + d.z*d.z;
    double lambda = 0.5;
    center.x = (*end[0]).pos.x + lambda*((*end[1]).pos.x - (*end[0]).pos.x);
    center.y = (*end[0]).pos.y + lambda*((*end[1]).pos.y - (*end[0]).pos.y);
    center.z = (*end[0]).pos.z + lambda*((*end[1]).pos.z - (*end[0]).pos.z);
}


//===========================================================================
/*!
      Return dot product of edge with given vector divided by 2-norm.

      \fn        double cCollisionSpheresEdge::compParam(const cVector3d &diff)
      \param     diff      The vector to dot with this edge.
      \return    Return dot product of edge with given vector divided by 2norm.
*/
//===========================================================================
double cCollisionSpheresEdge::compParam(const cVector3d &diff) const
{
    return cDot(d, diff)/D;
}


//===========================================================================
/*!
      Return whether 2-norm is zero, within tolerance.

      \fn         bool cCollisionSpheresEdge::isDegenerate() const
      \return     Return whether 2-norm is zero, within tolerance.
*/
//===========================================================================
bool cCollisionSpheresEdge::isDegenerate() const
{
    return (D < 10e-10);
}


//===========================================================================
/*!
      Return value of given number clipped within the given range.

      \fn        bool cCollisionSpheresEdge::clipTo(double &x, const double min,
                                            const double max)
      \param     x     The value to clip.
      \param     min   The minimum of the range to which the value is clipped.
      \param     max   The maximum of the range to which the value is clipped.
      \return    Return value of given number clipped within the given range.
*/
//===========================================================================
bool cCollisionSpheresEdge::clipTo(double &x, const double min,
                                   const double max) const
{
    if (x < min)
    {
      x = min;
      return true;
    }
    if (x > max)
    {
      x = max;
      return true;
    }
    return false;
}


//===========================================================================
/*!
      Calculate the distance to the given edge.

      \fn         double cCollisionSpheresEdge::dist(const cCollisionSpheresEdge
                                              &other) const
      \param      other   The edge to which distance from "this" is calculated.
      \return     Return the distance from "this" to the given edge.
*/
//===========================================================================
double cCollisionSpheresEdge::dist(const cCollisionSpheresEdge &other) const
{
    if (isDegenerate())
      return other.dist(*end[0]);
    if (other.isDegenerate())
      return this->dist(*other.end[0]);
    cVector3d diff, supp1, supp2;
    diff = cSub((*other.end[0]).pos, (*end[0]).pos);
    double R = cDot(d, other.d);
    double denom = D * other.D - R;
    double S1 = cDot(d, diff);
    double S2 = cDot(other.d, diff);
    double t, u;
    if (denom < 10e-10)
    {
      t = 0;
    } else
    {
      t = ((S1 * other.D) - (S2 * R)) / denom;
      clipTo(t, 0, 1);
    }
    u = ((t * R) - S2) / other.D;
    if (clipTo(u, 0, 1))
    {
      t = ((u * R) + S1) / D;
      clipTo(t, 0, 1);
    }
    double lambda = t;
    supp1.x = (*end[0]).pos.x + lambda*((*end[1]).pos.x - (*end[0]).pos.x);
    supp1.y = (*end[0]).pos.y + lambda*((*end[1]).pos.y - (*end[0]).pos.y);
    supp1.z = (*end[0]).pos.z + lambda*((*end[1]).pos.z - (*end[0]).pos.z);
    lambda = u;
    supp2.x = (*other.end[0]).pos.x + lambda*((*other.end[1]).pos.x - (*other.end[0]).pos.x);
    supp2.y = (*other.end[0]).pos.y + lambda*((*other.end[1]).pos.y - (*other.end[0]).pos.y);
    supp2.z = (*other.end[0]).pos.z + lambda*((*other.end[1]).pos.z - (*other.end[0]).pos.z);
    return supp1.distance(supp2);
}


//===========================================================================
/*!
      Calculate the distance to the given point.

      \fn         double cCollisionSpheresEdge::dist(const
                                      cCollisionSpheresPoint &other) const
      \param      other   The point to which distance from "this" is calculated.
      \return     Return the distance from "this" to the given point.
*/
//===========================================================================
double cCollisionSpheresEdge::dist(const cCollisionSpheresPoint &other) const
{
    cVector3d diff, supp;
    double t;
    if (!isDegenerate())
    {
      diff = cSub(other.pos, (*end[0]).pos);
      t = compParam(diff);
      if (t < 0)
        t = 0;
      else if (t > 1)
        t = 1;
    } else
    {
      t = 0;
    }
    double lambda = t;
    supp.x = (*end[0]).pos.x + lambda*((*end[1]).pos.x - (*end[0]).pos.x);
    supp.y = (*end[0]).pos.y + lambda*((*end[1]).pos.y - (*end[0]).pos.y);
    supp.z = (*end[0]).pos.z + lambda*((*end[1]).pos.z - (*end[0]).pos.z);
    return supp.distance(other.pos);
}


//===========================================================================
/*!
      Constructor of cCollisionSpheresTri.

      \fn         cCollisionSpheresTri::cCollisionSpheresTri(
                         cCollisionSpheresPoint *a, cCollisionSpheresPoint *b,
                         cCollisionSpheresPoint *c)
      \param      a     First vertex of the triangle.
      \param      b     Second vertex of the triangle.
      \param      c     Third vertex of the triangle.
      \return     Return a pointer to new cCollisionSpheresTri instance.
*/
//===========================================================================
cCollisionSpheresTri::cCollisionSpheresTri(cCollisionSpheresPoint *a,
                    cCollisionSpheresPoint *b, cCollisionSpheresPoint *c)
{
    corner[0] = a;
    corner[1] = b;
    corner[2] = c;
    setup();
}


//===========================================================================
/*!
      Constructor of cCollisionSpheresTri.

      \fn         cCollisionSpheresTri(cCollisionSpheresPoint *corner_[3])
      \param      corner_[3]    Array of the three vertices of the triangle.
      \return     Return a pointer to new cCollisionSpheresTri instance.
*/
//===========================================================================
cCollisionSpheresTri::cCollisionSpheresTri(cCollisionSpheresPoint *corner_[3])
{
    corner[0] = corner_[0];
    corner[1] = corner_[1];
    corner[2] = corner_[2];
    setup();
}


//===========================================================================
/*!
      Destructor of cCollisionSpheresTri.

      \fn         cCollisionSpheresTri::~cCollisionSpheresTri()
*/
//===========================================================================
cCollisionSpheresTri::~cCollisionSpheresTri()
{
    for (int i = 0; i < 3; i++)
      corner[i]->triList.remove(this);
}


//===========================================================================
/*!
      Associate the triangle with its edges and vice-versa.

      \fn         void cCollisionSpheresTri::setup()
*/
//===========================================================================
void cCollisionSpheresTri::setup()
{
    for (int i = 0; i < 3; i++)
    {
      corner[i]->triList.insert(corner[i]->triList.end(), this);
      cornerIdx[i] = corner[i]->getIndex();
      side[i] = corner[i]->getEdgeTo(corner[(i + 1) % 3]);
    }
    procChange();
}


//===========================================================================
/*!
      Recalculate properties from triangle's edges and vertices.

      \fn         void cCollisionSpheresTri::procChange()
*/
//===========================================================================
void cCollisionSpheresTri::procChange()
{
    int i;
    radius = 0;
    double lambda = 0.5;
    center.x = (*side[0]->getCenter()).x +
               lambda*((*corner[0]).pos.x - (*side[0]->getCenter()).x);
    center.y = (*side[0]->getCenter()).y +
               lambda*((*corner[0]).pos.y - (*side[0]->getCenter()).y);
    center.z = (*side[0]->getCenter()).z +
               lambda*((*corner[0]).pos.z - (*side[0]->getCenter()).z);

    for (i = 0; i < 3; i++)
    {
      double currad = corner[i]->pos.distance(center);
      if (currad > radius)
        radius = currad;
    }
    cVector3d s1, s2;
    s1 = cSub((*corner[1]).pos, (*corner[0]).pos);
    s2 = cSub((*corner[2]).pos, (*corner[0]).pos);
    normal = cCross(s1, s2);
    if (normal.length() > 0.0000001)
      normal.normalize();
    for (i = 0; i < 3; i++)
      corner[i]->procNormal();
}


//===========================================================================
/*!
      Calculate the distance to another primitive shape.

      \fn         double cCollisionSpheresTri::dist(
                        cCollisionSpheresGenericShape *other,
                        cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                        cVector3d& a_colPoint, double& a_colSquareDistance))
      \param      other          The line to which distance is calculated.
      \param      a_colObject    Returns object with which collision occurs.
      \param      a_colTriangle  Returns triangle with which collision occurs.
      \param      a_colPoint     Returns point at which collision occurs.
      \param      a_colSquareDistance  Returns distance squared to collision pt.
      \return     Return whether triangle collides with given line.
*/
//===========================================================================
bool cCollisionSpheresTri::dist(cCollisionSpheresGenericShape *other,
          cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
          cVector3d& a_colPoint, double& a_colSquareDistance)
{
   cCollisionSpheresLine* l_line = (cCollisionSpheresLine*)other;
   return  (m_original->computeCollision(
                                    l_line->getSegmentPointA(),
                                    l_line->getDir(),
                                    a_colObject,
                                    a_colTriangle,
                                    a_colPoint,
                                    a_colSquareDistance
                                  ));
}


//===========================================================================
/*!
      Draw the triangle in OpenGL.

      \fn         void cCollisionSpheresTri::drawGL()
*/
//===========================================================================
void cCollisionSpheresTri::drawGL()
{
    cVector3d s1, s2;
    glNormal3d(corner[0]->normal.x, corner[0]->normal.y, corner[0]->normal.z);
    glVertex3d((*corner[0])[0], (*corner[0])[1], (*corner[0])[2]);
    glNormal3d(corner[1]->normal.x, corner[1]->normal.y, corner[1]->normal.z);
    glVertex3d((*corner[1])[0], (*corner[1])[1], (*corner[1])[2]);
    glNormal3d(corner[2]->normal.x, corner[2]->normal.y, corner[2]->normal.z);
    glVertex3d((*corner[2])[0], (*corner[2])[1], (*corner[2])[2]);
}


//===========================================================================
/*!
      Constructor of cCollisionSpheresLine.

      \fn         cCollisionSpheresLine::cCollisionSpheresLine(
                          std::pair<cVector3d, cVector3d> &a_line)
      \param      a_segmentPointA   First endpoint of the line.
      \param      a_segmentPointB   Second endpoint of the line.
      \return     Return a pointer to new cCollisionSpheresLine instance.
*/
//===========================================================================
cCollisionSpheresLine::cCollisionSpheresLine(cVector3d& a_segmentPointA,
        cVector3d& a_segmentPointB)
{
    center = cAdd(a_segmentPointA, a_segmentPointB);
    center.x *= 0.5;
    center.y *= 0.5;
    center.z *= 0.5;
    cVector3d rad = cSub(center, a_segmentPointA);
    radius = sqrt(rad.x*rad.x + rad.y*rad.y + rad.z*rad.z);
    m_segmentPointA = a_segmentPointA;
    a_segmentPointB.subr(a_segmentPointA, m_dir);
}


//===========================================================================
/*!
      Calculate the distance to another primitive shape.  This function just
      reverses the call, calling the received shape's distance function with
      "this" as its other shape parameter.

      \fn         double cCollisionSpheresLine::dist(
                        cCollisionSpheresGenericShape *other,
                        cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                        cVector3d& a_colPoint, double& a_colSquareDistance)
      \param      other          Triangle to which distance is calculated.
      \param      a_colObject    Returns object with which collision occurs.
      \param      a_colTriangle  Returns triangle with which collision occurs.
      \param      a_colPoint     Returns point at which collision occurs.
      \param      a_colSquareDistance  Returns distance squared to collision pt.
      \return     Return whether shapes collide by calling other's dist function.
*/
//===========================================================================
bool cCollisionSpheresLine::dist(cCollisionSpheresGenericShape *other,
          cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
          cVector3d& a_colPoint, double& a_colSquareDistance)
{
    return other->dist(this, a_colObject, a_colTriangle,
            a_colPoint, a_colSquareDistance);
}





