////////////////////////////////////////////////////////////////////////
//
//  (C) 2002 - Force Dimension LLC
//  All Rights Reserved.
//
//  Matricial math functions optimized for speed
//  Header file  
//
////////////////////////////////////////////////////////////////////////



#ifndef __MMATH_H__
#define __MMATH_H__

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
#define M_PI 3.141592653589
#endif


// vectorial computing
int    m_set        (double v[3], double x, double y, double z);
int    m_nil        (double v[3]);
int    m_add        (double d[3], double v1[3], double v2[3]);
int    m_sub        (double d[3], double v1[3], double v2[3]);
int    m_div        (double k,    double v [3]);
int    m_normalize  (double d[3], double v [3]);
double m_norm       (double v[3]);

// matricial computing
int    m_setmat     (double m[3][3], double v1[3],    double v2[3], double v3[3]);
int    m_mul        (double d[3],    double m [3][3], double v [3]);
int    m_inv        (double d[3][3], double m [3][3]);
double m_det        (double m[3][3]);



////////////////////////////////////////////////////////////////////////
///
///  Set a vector with given values.
///
///  \param v    vector to set
///  \param x    X vector value 
///  \param y    Y vector value
///  \param z    Z vector value 
///
///  \return
///  0  if no error occured,\n
///  -1 if an error occured.
///
////////////////////////////////////////////////////////////////////////
inline int
m_set (double v[3], 
       double x, 
       double y, 
       double z)
{
  v[0] = x;
  v[1] = y;
  v[2] = z;


  return 0;
}



////////////////////////////////////////////////////////////////////////
///
///  Set a vector to zero.
///
///  \param v    vector to set
///
///  \return
///  0  if no error occured,\n
///  -1 if an error occured.
///
////////////////////////////////////////////////////////////////////////
inline int  
m_nil (double v[3])
{
  v[0] = v[1] = v[2] = 0.0f;


  return 0;
}



////////////////////////////////////////////////////////////////////////
///
///  Add two vectors. The result (v0+v1) is placed in another vector.
///
///  \param d    \b [out] addition result
///  \param v0   first operand
///  \param v1   second operand
///
///  \return
///  0  if no error occured,\n
///  -1 if an error occured.
///
////////////////////////////////////////////////////////////////////////
inline int
m_add (double d[3], 
       double v0[3], 
       double v1[3])
{
  d[0] = v0[0] + v1[0];
  d[1] = v0[1] + v1[1];
  d[2] = v0[2] + v1[2];


  return 0;
}



////////////////////////////////////////////////////////////////////////
///
///  Subtract two vectors. The result (v0-v1) is placed in another vector.
///
///  \param d    \b [out] subtraction result
///  \param v0   first operand
///  \param v1   second operand
///
///  \return
///  0  if no error occured,\n
///  -1 if an error occured.
///
////////////////////////////////////////////////////////////////////////
inline int
m_sub (double d[3],
       double v0[3],
       double v1[3])
{
  d[0] = v0[0] - v1[0];
  d[1] = v0[1] - v1[1];
  d[2] = v0[2] - v1[2];


  return 0;
}



////////////////////////////////////////////////////////////////////////
///
///  Divide a vector by a constant.
///
///  \param k    division constant
///  \param v    \b [in/out] vector
///
///  \return
///  0  if no error occured,\n
///  -1 if an error occured.
///
////////////////////////////////////////////////////////////////////////
inline int
m_div (double k,
       double v[3])
{
  if (k == 0.0f)
  {
    return -1;
  }

  v[0] /= k;
  v[1] /= k;
  v[2] /= k;


  return 0;
}



////////////////////////////////////////////////////////////////////////
///
///  Normalize a vector. The resulting vector is placed in another vector.
///
///  \param d    \b [out] result
///  \param v    vector to normalize 
///
///  \return
///  0  if no error occured,\n
///  -1 if an error occured.
///
////////////////////////////////////////////////////////////////////////
inline int
m_normalize (double d[3],
             double v[3])
{
  d[0] = v[0];
  d[1] = v[1];
  d[2] = v[2];

  m_div (m_norm(v), d);


  return 0;
}



////////////////////////////////////////////////////////////////////////
///
///  Compute the norm of a given vector.
///
///  \param v     vector
///
///  \return
///  The norm of v.
///
////////////////////////////////////////////////////////////////////////
inline double 
m_norm (double v[3])
{
  return ((double)(sqrt((v[0])*(v[0]) + (v[1])*(v[1]) + (v[2])*(v[2]))));
}



////////////////////////////////////////////////////////////////////////
///
///  Set a matrix from three column vectors.
///
///  \param m    \b [out] matrix to set
///  \param v0   column vector 0 
///  \param v1   column vector 1 
///  \param v2   column vector 2  
///
///  \return
///  0  if no error occured,\n
///  -1 if an error occured.
///
////////////////////////////////////////////////////////////////////////
inline int
m_setmat (double m[3][3], 
          double v0[3],  
          double v1[3], 
          double v2[3])
{
  m[0][0] = v0[0];  m[0][1] = v1[0];  m[0][2] = v2[0];
  m[1][0] = v0[1];  m[1][1] = v1[1];  m[1][2] = v2[1];
  m[2][0] = v0[2];  m[2][1] = v1[2];  m[2][2] = v2[2];


  return 0;
}



////////////////////////////////////////////////////////////////////////
///
///  Multiply a vector by a matrix. The result is placed in another vector.
///
///  \param d    \b [out] multiplication result
///  \param m    multiplication matrix 
///  \param v    vector to multiply
///
///  \return
///  0  if no error occured,\n
///  -1 if an error occured.
///
////////////////////////////////////////////////////////////////////////
inline int
m_mul (double d[3],
       double m[3][3],
       double v[3])
{
  d[0] = m[0][0]*(v[0]) + m[0][1]*(v[1]) + m[0][2]*(v[2]);
  d[1] = m[1][0]*(v[0]) + m[1][1]*(v[1]) + m[1][2]*(v[2]);
  d[2] = m[2][0]*(v[0]) + m[2][1]*(v[1]) + m[2][2]*(v[2]);


  return 0;
}



////////////////////////////////////////////////////////////////////////
///
///  Compute the inverse of a given matrix.
///
///  \param d    \b [out] inverted matrix
///  \param m    matrix to invert
///  
///  \return
///  0  if no error occured,\n
///  -1 if an error occured.
///
////////////////////////////////////////////////////////////////////////
inline int
m_inv (double d[3][3],
       double m[3][3])
{
  double smat[3][3];
  double det         = m_det(m);


  // check determinant
  if (fabs(det) < 0.000000000001f)
  {
    return -1;
  }

  // take no risk
  smat[0][0] = m[0][0]; smat[0][1] = m[0][1]; smat[0][2] = m[0][2];
  smat[1][0] = m[1][0]; smat[1][1] = m[1][1]; smat[1][2] = m[1][2];
  smat[2][0] = m[2][0]; smat[2][1] = m[2][1]; smat[2][2] = m[2][2];

  // perform conversion
  d[0][0] =  (smat[1][1]*smat[2][2] - smat[2][1]*smat[1][2])/det;
  d[0][1] = -(smat[0][1]*smat[2][2] - smat[2][1]*smat[0][2])/det;
  d[0][2] =  (smat[0][1]*smat[1][2] - smat[1][1]*smat[0][2])/det;

  d[1][0] = -(smat[1][0]*smat[2][2] - smat[2][0]*smat[1][2])/det;
  d[1][1] =  (smat[0][0]*smat[2][2] - smat[2][0]*smat[0][2])/det;
  d[1][2] = -(smat[0][0]*smat[1][2] - smat[1][0]*smat[0][2])/det;

  d[2][0] =  (smat[1][0]*smat[2][1] - smat[2][0]*smat[1][1])/det;
  d[2][1] = -(smat[0][0]*smat[2][1] - smat[2][0]*smat[0][1])/det;
  d[2][2] =  (smat[0][0]*smat[1][1] - smat[1][0]*smat[0][1])/det;


  return 0;
}



////////////////////////////////////////////////////////////////////////
///
///  Compute a matrix determinant.
///
///  \param m     matrix
///
///  \return
///  The determinant of m.
///
////////////////////////////////////////////////////////////////////////
inline double
m_det (double m[3][3])
{
  return (  m[0][0] * m[1][1] * m[2][2]
  + m[0][1] * m[1][2] * m[2][0]
  + m[0][2] * m[1][0] * m[2][1]
  - m[2][0] * m[1][1] * m[0][2]
  - m[2][1] * m[1][2] * m[0][0]
  - m[2][2] * m[1][0] * m[0][1]);
}


#endif

