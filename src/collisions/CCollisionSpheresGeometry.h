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
      \file       CCollisionSpheresGeometry.h
      \version    1.0
      \date       09/2003
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CCollisionSpheresGeometryH
#define CCollisionSpheresGeometryH
//---------------------------------------------------------------------------
#include "CMaths.h"
#include "CTriangle.h"
#include <assert.h>
#include <list>
#include <map>
#include <math.h>
#include <windows.h>
#include <gl/gl.h>
//---------------------------------------------------------------------------
using std::map;
using std::less;
using std::list;
//---------------------------------------------------------------------------
class cCollisionSpheresPoint;
class cCollisionSpheresEdge;
class cCollisionSpheresTri;
class cCollisionSpheresLine;
class cCollisionSpheresLeaf;

// TYPEDEFS:
typedef map<cCollisionSpheresPoint *, cCollisionSpheresEdge *,
            less<cCollisionSpheresPoint *> > PtEmap;
typedef list<cCollisionSpheresTri *> Tlist;
//---------------------------------------------------------------------------

//===========================================================================
//  - CLASS DEFINITION -
/*!
      \class    cCollisionSpheresGenericShape
      \brief    cCollisionSpheresGenericShape is an abstract class for shape
                primitives (such as triangles or lines) in the model which
                are surrounded by spheres for the collision detector.
*/
//===========================================================================
class cCollisionSpheresGenericShape
{
  // FRIENDS:
  //! Leaf nodes of the collision detection sphere tree.
  friend class cCollisionSpheresLeaf;

  public:
    // CONSTRUCTOR:
    //! Constructor of cCollisionSpheresGenericShape.
    cCollisionSpheresGenericShape() {sphere = NULL;}

    // METHODS:
    //! Return center.
    virtual const cVector3d &getCenter() const = 0;
    //! Return radius.
    virtual double getRadius() const = 0;
    //! Calculate distance to another primitive.
    virtual bool dist(cCollisionSpheresGenericShape *other,
                  cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                  cVector3d& a_colPoint, double& a_colSquareDistance) = 0;
    //! Axis on which to sort.
    static int split;
    //! Overloaded less than operator (for sorting).
    bool operator<(cCollisionSpheresGenericShape* other);


  protected:
    // PROPERTIES:
    //! Pointer to the ollision sphere surrounding the primitive.
    cCollisionSpheresLeaf *sphere;
};


//===========================================================================
//  - CLASS DEFINITION -
/*!
      \class    cCollisionSpheresPoint
      \brief    cCollisionSpheresPoint defines points used in the primitive
                shapes in the model.
*/
//===========================================================================
class cCollisionSpheresPoint
{
  // FRIENDS:
  //! Edges of primitive shapes.
  friend class cCollisionSpheresEdge;
  //! Triangles in the model.
  friend class cCollisionSpheresTri;

  public:
    // CONSTRUCTORS:
    //! Constructor of cCollisionSpheresPoint.
    cCollisionSpheresPoint(double x = 0, double y = 0, double z = 0);
    //! Constructor of cCollisionSpheresPoint.
    cCollisionSpheresPoint(double x[3]);
    //! Copy constructor.
    cCollisionSpheresPoint &operator=(const cVector3d &other);

    // METHODS:
    //! Set position of point given three doubles.
    void set(double x, double y, double z);
    //! Set position of one coordinate given index and value.
    void set(int coord, double value);
    //! Set position of point given array of three doubles.
    void set(double x[3]);
    //! Set index.
    inline void setIndex(int index_)  {index = index_;}
    //! Return the index.
    inline int getIndex()  {return index;}
    //! Calculate normal of the point from normals of associated triangles.
    void procNormal();
    //! Recalculate properties for associated edges and triangles.
    void procChange();
    //! Return the edge, if any, from this point to the given point.
    cCollisionSpheresEdge *getEdgeTo(cCollisionSpheresPoint *other);

    // PROPERTIES:
    //! Position of the point.
    cVector3d pos;
    //! Map of edges from this point.
    PtEmap edgeMap;
    //! List of triangles for which this point is a vertex.
    Tlist triList;
    //! Index of the point.
    int index;
    //! Normal, calculated from all triangles for which the point is a vertex.
    cVector3d normal;
    //! Array subscript access to position of the point.
    virtual double &operator[](int i)
    {
      assert((i>=0) && (i<3));
      switch (i)
      {
        case 0:  return (pos.x);
        case 1:  return (pos.y);
        case 2:  return (pos.z);
      }
      return (pos.x);
    }
};


//===========================================================================
//  - CLASS DEFINITION -
/*!
      \class    cCollisionSpheresEdge
      \brief    cCollisionSpheresEdge defines edges of shape primitives
                in the model.
*/
//===========================================================================
class cCollisionSpheresEdge
{
  // FRIENDS:
  //! Triangles in the model.
  friend class cCollisionSpheresTri;

  public:
    // CONSTRUCTORS:
    //! Constructor of cCollisionSpheresEdge.
    cCollisionSpheresEdge(cCollisionSpheresPoint *a, cCollisionSpheresPoint *b);
    //! Constructor of cCollisionSpheresEdge.
    cCollisionSpheresEdge(cCollisionSpheresPoint *end_[2]);

    // METHODS:
    //! Return the first or second point of the edge.
    inline const cCollisionSpheresPoint &get(int coord) const
		  {assert((coord >= 0) && (coord < 2)); return *end[coord];}
    //! Calculate the distance to the given edge.
    double dist(const cCollisionSpheresEdge &other) const;
    //! Calculate the distance to the given point.
    double dist(const cCollisionSpheresPoint &other) const;
    //! Return the center of the edge.
    inline const cVector3d *getCenter() const  {return &center;}
    //! Return the radius of the edge.
    inline double getRadius() const  {
    if (D <= 0.0) return 0.0; return sqrt(D)/2;}
    //! Recalculate properties of the edge from its vertices.
    void procChange();
    //! Return dot product of edge with given vector divided by 2-norm.
    double compParam(const cVector3d &diff) const;
    //! Return whether 2-norm is zero, within tolerance.
    bool isDegenerate() const;
    //! Return value of given number clipped within the given range.
    bool clipTo(double &x, const double min, const double max) const;
    //! Enter the two vertices of the edge in the edge map.
    void setup();

  private:
    // PROPERTIES:
    //! The two vertices of the edge.
    cCollisionSpheresPoint *end[2];
    //! The center of the edge.
    cVector3d center;
    //! The distance between the vertices.
    cVector3d d;
    //! The 2-norm of the edge.
    double D;
    //! List of triangles bounded by the edge.
    list<cCollisionSpheresTri *> triList;
};


//===========================================================================
//  - CLASS DEFINITION -
/*!
      \class    cCollisionSpheresTri
      \brief    cCollisionSpheresTri defines the triangle primitives that
                make up the model and are bounded by the collision spheres.
                It is associated with a specific cTriangle object in the mesh.
*/
//===========================================================================
class cCollisionSpheresTri : public cCollisionSpheresGenericShape
{
  public:
    // CONSTRUCTORS AND DESTRUCTOR:
    //! Constructor of cCollisionSpheresTri.
    cCollisionSpheresTri(cCollisionSpheresPoint *a, cCollisionSpheresPoint *b,
                         cCollisionSpheresPoint *c);
    //! Constructor of cCollisionSpheresTri.
    cCollisionSpheresTri(cCollisionSpheresPoint *corner_[3]);
    //! Destructor of cCollisionSpheresTri.
    ~cCollisionSpheresTri();

    // METHODS:
    //! Return the given edge of the triangle.
    inline const cCollisionSpheresEdge &getSide(int coord) const
		  {assert((coord >= 0) && (coord < 3)); return *side[coord];}
    //! Return the given vertex of the triangle.
    inline const cCollisionSpheresPoint &getCorner(int coord) const
		  {assert((coord >= 0) && (coord < 3)); return *corner[coord];}
    //! Calculate the distance to an edge.
    double dist(cCollisionSpheresEdge *other);
    //! Calculate the distance to a point.
    double dist(cCollisionSpheresPoint *other);
    //! Return whether triangle collides with given line.
    bool dist(cCollisionSpheresGenericShape *other,
              cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
              cVector3d& a_colPoint, double& a_colSquareDistance);
    //! Return the center of the triangle.
    inline const cVector3d &getCenter() const  {return center;}
    //! Return the radius of the triangle.
    inline double getRadius() const  {return radius;}
    //! Return the normal of the triangle.
    inline const cVector3d &getNormal() const  {return normal;}
    //! Draw the triangle in OpenGL.
    void drawGL();
    //! Recalculate properties from triangle's edges and vertices.
    void procChange();
    //! Associate the triangle with its edges and vice-versa.
    void setup();
    //! Returns the cTriangle object in the mesh associated with this triangle.
    cTriangle* getOriginal() { return m_original; }
    //! Sets the cTriangle object in the mesh associated with this triangle.
    void setOriginal(cTriangle* a_original) { m_original = a_original; }

  private:
    // PROPERTIES:
    //! The vertices of the triangle.
    cCollisionSpheresPoint *corner[3];
    //! The edges of the triangle.
    cCollisionSpheresEdge *side[3];
    //! The indices of the triangle's vertices.
    int cornerIdx[3];
    //! The normal vector for the triangle.
    cVector3d normal;
    //! The center of the triangle.
    cVector3d center;
    //! The radius of the triangle.
    double radius;
    //! The cTriangle object in the mesh associated with this triangle.
    cTriangle* m_original;
};


//===========================================================================
//  - CLASS DEFINITION -
/*!
      \class    cCollisionSpheresLine
      \brief    cCollisionSpheresLine defines a line primitive that may
                collide with other primitives.  It is used by the proxy
                algorithm.
*/
//===========================================================================
class cCollisionSpheresLine : public cCollisionSpheresGenericShape
{
  // FRIENDS:
  //! Leaf nodes of the collision detection sphere tree.
  friend class cCollisionSpheresLeaf;

  public:
    // CONSTRUCTOR:
    //! Constructor of cCollisionSpheresLine.
    cCollisionSpheresLine(cVector3d& a_segmentPointA, cVector3d& a_segmentPointB);

    //! METHODS:
    //! Return the center of the line.
    inline const cVector3d &getCenter() const {return center;}
    //! Return the radius of the line.
    inline double getRadius() const {return radius;}
    //! Return whether triangle collides with line.
    bool dist(cCollisionSpheresGenericShape *other,
                  cGenericObject*& a_colObject, cTriangle*& a_colTriangle,
                  cVector3d& a_colPoint, double& a_colSquareDistance);
    //! Get first endpoint of the line.
    cVector3d getSegmentPointA() { return m_segmentPointA; }
    //! Get direction vector of the line.
    cVector3d getDir() { return m_dir; }

  protected:
    // PROPERTIES:
    //! The center of the line.
    cVector3d center;
    //! The radius of the line.
    double radius;
    //! The first endpoint of the line.
    cVector3d m_segmentPointA;
    //! The direction vector of the line.
    cVector3d m_dir;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
