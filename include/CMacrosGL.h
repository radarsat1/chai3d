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
#ifndef CMacrosGLH
#define CMacrosGLH
//---------------------------------------------------------------------------
#include "CVector3d.h"
#include "CMatrix3d.h"
#include "CString.h"
#include "windows.h"
#include "gl/gl.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \struct   cMatrixGL
      \brief    CHAI describes rotations using 3x3 rotation matrices (cMatrix3d)
                and 3D vectors (cMatrix3d) to express position or translation.
                On the OpenGL side 4x4 matrices are required to perform all
                geometrical transformations. cMatrixGL provides a structure
                which encapsulates all the necessary
                functionalities to generate 4x4 OpenGL transformation matrices
                by passing 3D position vectors and rotation matrices.
                DCSGL also provides OpenGL calls to push, multiply and pop
                matrices off the OpenGL stack. OpenGL Matrices are COLUMN major.
*/
//===========================================================================
struct cMatrixGL
{
  private:
	//! array of type \e double.
	double  m[4][4];

  public:
    //-----------------------------------------------------------------------
	/*!
		  Returns a pointer to the matrix array in memory.

	      \return   Returns pointer of type \e double.
	*/
    //-----------------------------------------------------------------------
	const double* pMatrix() const { return m[0]; }


    //-----------------------------------------------------------------------
	/*!
		  Creates OpenGL translation matrix from a position vector passed as
          parameter.

          \param    a_pos   Input vector.
	*/
    //-----------------------------------------------------------------------
	inline void set(const cVector3d& a_pos)
	{
        m[0][0] = 1.0;      m[0][1] = 0.0;       m[0][2] = 0.0;       m[0][3] = 0.0;
        m[1][0] = 0.0;      m[1][1] = 1.0;       m[1][2] = 0.0;       m[1][3] = 0.0;
        m[2][0] = 0.0;      m[2][1] = 0.0;       m[2][2] = 1.0;       m[2][3] = 0.0;
        m[3][0] = a_pos.x;  m[3][1] = a_pos.y;   m[3][2] = a_pos.z;   m[3][3] = 1.0;
    }


    //-----------------------------------------------------------------------
	/*!
		  Creates OpenGL rotation matrix from a rotation 3x3 Matrix passed as
          parameter.

	      \param    a_rot  Rotation Matrix.
	*/
    //-----------------------------------------------------------------------
	void set(const cMatrix3d& a_rot)
	{
        m[0][0] = a_rot.m[0][0];  m[0][1] = a_rot.m[1][0];  m[0][2] = a_rot.m[2][0];  m[0][3] = 0.0;
        m[1][0] = a_rot.m[0][1];  m[1][1] = a_rot.m[1][1];  m[1][2] = a_rot.m[2][1];  m[1][3] = 0.0;
        m[2][0] = a_rot.m[0][2];  m[2][1] = a_rot.m[1][2];  m[2][2] = a_rot.m[2][2];  m[2][3] = 0.0;
        m[3][0] = 0.0;            m[3][1] = 0.0;            m[3][2] = 0.0;            m[3][3] = 1.0;
	}


    //-----------------------------------------------------------------------
	/*!
		  Creates OpenGL translation matrix from Vector giving translation.
		  Do not use directly.

	      \param    a_pos   Translation Vector.
	      \param    a_rot   Rotation Matrix.
	*/
    //-----------------------------------------------------------------------
	void set(const cVector3d& a_pos, const cMatrix3d& a_rot)
	{
        m[0][0] = a_rot.m[0][0];  m[0][1] = a_rot.m[1][0];  m[0][2] = a_rot.m[2][0];  m[0][3] = 0.0;
        m[1][0] = a_rot.m[0][1];  m[1][1] = a_rot.m[1][1];  m[1][2] = a_rot.m[2][1];  m[1][3] = 0.0;
        m[2][0] = a_rot.m[0][2];  m[2][1] = a_rot.m[1][2];  m[2][2] = a_rot.m[2][2];  m[2][3] = 0.0;
        m[3][0] = a_pos.x;        m[3][1] = a_pos.y;        m[3][2] = a_pos.z;        m[3][3] = 1.0;
	}


    //-----------------------------------------------------------------------
    /*!
        Copy current matrix to an external matrix passed as parameter.

        \param    a_destination  Destination matrix.
    */
    //-----------------------------------------------------------------------
    inline void copyto(cMatrixGL& a_destination) const
    {
        a_destination.m[0][0] = m[0][0];  a_destination.m[0][1] = m[0][1];
        a_destination.m[0][2] = m[0][2];  a_destination.m[0][3] = m[0][3];
        a_destination.m[1][0] = m[1][0];  a_destination.m[1][1] = m[1][1];
        a_destination.m[1][2] = m[1][2];  a_destination.m[1][3] = m[1][3];
        a_destination.m[2][0] = m[2][0];  a_destination.m[2][1] = m[2][1];
        a_destination.m[2][2] = m[2][2];  a_destination.m[2][3] = m[2][3];
        a_destination.m[3][0] = m[3][0];  a_destination.m[3][1] = m[3][1];
        a_destination.m[3][2] = m[3][2];  a_destination.m[3][3] = m[3][3];
    }


    //-----------------------------------------------------------------------
    /*!
        Copy values from an external matrix passed as parameter to current
        matrix.

        \param    a_source  Source matrix.
    */
    //-----------------------------------------------------------------------
    inline void copyfrom(const cMatrixGL& a_source)
    {
        m[0][0] = a_source.m[0][0];  m[0][1] = a_source.m[0][1];
        m[0][2] = a_source.m[0][2];  m[0][3] = a_source.m[0][3];
        m[1][0] = a_source.m[1][0];  m[1][1] = a_source.m[1][1];
        m[1][2] = a_source.m[1][2];  m[1][3] = a_source.m[1][3];
        m[2][0] = a_source.m[2][0];  m[2][1] = a_source.m[2][1];
        m[2][2] = a_source.m[2][2];  m[2][3] = a_source.m[2][3];
        m[3][0] = a_source.m[3][0];  m[3][1] = a_source.m[3][1];
        m[3][2] = a_source.m[3][2];  m[3][3] = a_source.m[3][3];
    }


    //-----------------------------------------------------------------------
    /*!
        Set the identity matrix. 
    */
    //-----------------------------------------------------------------------
    inline void identity()
    {
        m[0][0] = 1.0;  m[0][1] = 0.0;  m[0][2] = 0.0;  m[0][3] = 0.0;
        m[1][0] = 0.0;  m[1][1] = 1.0;  m[1][2] = 0.0;  m[1][3] = 0.0;
        m[2][0] = 0.0;  m[2][1] = 0.0;  m[2][2] = 1.0;  m[2][3] = 0.0;
        m[3][0] = 0.0;  m[3][1] = 0.0;  m[3][2] = 0.0;  m[3][3] = 1.0;
    }


    //-----------------------------------------------------------------------
    /*!
        Multiply current matrix with an external matrix passed as parameter.
        Result is stored in current matrix.

        \param    a_matrix  Matrix with which multiplication is performed.
    */
    //-----------------------------------------------------------------------
    inline void mul(const cMatrixGL& a_matrix)
    {
        // compute multiplication between both matrices
        double m00 = m[0][0] * a_matrix.m[0][0] + m[0][1] * a_matrix.m[1][0] +
                     m[0][2] * a_matrix.m[2][0] + m[0][3] * a_matrix.m[3][0];
        double m01 = m[0][0] * a_matrix.m[0][1] + m[0][1] * a_matrix.m[1][1] +
                     m[0][2] * a_matrix.m[2][1] + m[0][3] * a_matrix.m[3][1];
        double m02 = m[0][0] * a_matrix.m[0][2] + m[0][1] * a_matrix.m[1][2] +
                     m[0][2] * a_matrix.m[2][2] + m[0][3] * a_matrix.m[3][2];
        double m03 = m[0][0] * a_matrix.m[0][3] + m[0][1] * a_matrix.m[1][3] +
                     m[0][2] * a_matrix.m[2][3] + m[0][3] * a_matrix.m[3][3];

        double m10 = m[1][0] * a_matrix.m[0][0] + m[1][1] * a_matrix.m[1][0] +
                     m[1][2] * a_matrix.m[2][0] + m[1][3] * a_matrix.m[3][0];
        double m11 = m[1][0] * a_matrix.m[0][1] + m[1][1] * a_matrix.m[1][1] +
                     m[1][2] * a_matrix.m[2][1] + m[1][3] * a_matrix.m[3][1];
        double m12 = m[1][0] * a_matrix.m[0][2] + m[1][1] * a_matrix.m[1][2] +
                     m[1][2] * a_matrix.m[2][2] + m[1][3] * a_matrix.m[3][2];
        double m13 = m[1][0] * a_matrix.m[0][3] + m[1][1] * a_matrix.m[1][3] +
                     m[1][2] * a_matrix.m[2][3] + m[1][3] * a_matrix.m[3][3];

        double m20 = m[2][0] * a_matrix.m[0][0] + m[2][1] * a_matrix.m[1][0] +
                     m[2][2] * a_matrix.m[2][0] + m[2][3] * a_matrix.m[3][0];
        double m21 = m[2][0] * a_matrix.m[0][1] + m[2][1] * a_matrix.m[1][1] +
                     m[2][2] * a_matrix.m[2][1] + m[2][3] * a_matrix.m[3][1];
        double m22 = m[2][0] * a_matrix.m[0][2] + m[2][1] * a_matrix.m[1][2] +
                     m[2][2] * a_matrix.m[2][2] + m[2][3] * a_matrix.m[3][2];
        double m23 = m[2][0] * a_matrix.m[0][3] + m[2][1] * a_matrix.m[1][3] +
                     m[2][2] * a_matrix.m[2][3] + m[2][3] * a_matrix.m[3][3];

        double m30 = m[3][0] * a_matrix.m[0][0] + m[3][1] * a_matrix.m[1][0] +
                     m[3][2] * a_matrix.m[2][0] + m[3][3] * a_matrix.m[3][0];
        double m31 = m[3][0] * a_matrix.m[0][1] + m[3][1] * a_matrix.m[1][1] +
                     m[3][2] * a_matrix.m[2][1] + m[3][3] * a_matrix.m[3][1];
        double m32 = m[3][0] * a_matrix.m[0][2] + m[3][1] * a_matrix.m[1][2] +
                     m[3][2] * a_matrix.m[2][2] + m[3][3] * a_matrix.m[3][2];
        double m33 = m[3][0] * a_matrix.m[0][3] + m[3][1] * a_matrix.m[1][3] +
                     m[3][2] * a_matrix.m[2][3] + m[3][3] * a_matrix.m[3][3];

        // return values to current matrix
        m[0][0] = m00;  m[0][1] = m01;  m[0][2] = m02;  m[0][3] = m03;
        m[1][0] = m10;  m[1][1] = m11;  m[1][2] = m12;  m[1][3] = m13;
        m[2][0] = m20;  m[2][1] = m21;  m[2][2] = m22;  m[2][3] = m23;
        m[3][0] = m30;  m[3][1] = m31;  m[3][2] = m32;  m[3][3] = m33;
    }


    //-----------------------------------------------------------------------
    /*!
        Multiply current matrix with an external matrix passed as parameter.
        Result is stored in result matrix.

        \param      a_matrix  Matrix with which multiplication is performed.
        \param      a_result  Here is where result is stored.
    */
    //-----------------------------------------------------------------------
    inline void mulr(const cMatrix3d& a_matrix, cMatrix3d& a_result)
    {
        // compute multiplication between both matrices
        a_result.m[0][0] = m[0][0] * a_matrix.m[0][0] + m[0][1] * a_matrix.m[1][0] +
                           m[0][2] * a_matrix.m[2][0] + m[0][3] * a_matrix.m[3][0];
        a_result.m[0][1] = m[0][0] * a_matrix.m[0][1] + m[0][1] * a_matrix.m[1][1] +
                           m[0][2] * a_matrix.m[2][1] + m[0][3] * a_matrix.m[3][1];
        a_result.m[0][2] = m[0][0] * a_matrix.m[0][2] + m[0][1] * a_matrix.m[1][2] +
                           m[0][2] * a_matrix.m[2][2] + m[0][3] * a_matrix.m[3][2];
        a_result.m[0][3] = m[0][0] * a_matrix.m[0][3] + m[0][1] * a_matrix.m[1][3] +
                           m[0][2] * a_matrix.m[2][3] + m[0][3] * a_matrix.m[3][3];

        a_result.m[1][0] = m[1][0] * a_matrix.m[0][0] + m[1][1] * a_matrix.m[1][0] +
                           m[1][2] * a_matrix.m[2][0] + m[1][3] * a_matrix.m[3][0];
        a_result.m[1][1] = m[1][0] * a_matrix.m[0][1] + m[1][1] * a_matrix.m[1][1] +
                           m[1][2] * a_matrix.m[2][1] + m[1][3] * a_matrix.m[3][1];
        a_result.m[1][2] = m[1][0] * a_matrix.m[0][2] + m[1][1] * a_matrix.m[1][2] +
                           m[1][2] * a_matrix.m[2][2] + m[1][3] * a_matrix.m[3][2];
        a_result.m[1][3] = m[1][0] * a_matrix.m[0][3] + m[1][1] * a_matrix.m[1][3] +
                           m[1][2] * a_matrix.m[2][3] + m[1][3] * a_matrix.m[3][3];

        a_result.m[2][0] = m[2][0] * a_matrix.m[0][0] + m[2][1] * a_matrix.m[1][0] +
                           m[2][2] * a_matrix.m[2][0] + m[2][3] * a_matrix.m[3][0];
        a_result.m[2][1] = m[2][0] * a_matrix.m[0][1] + m[2][1] * a_matrix.m[1][1] +
                           m[2][2] * a_matrix.m[2][1] + m[2][3] * a_matrix.m[3][1];
        a_result.m[2][2] = m[2][0] * a_matrix.m[0][2] + m[2][1] * a_matrix.m[1][2] +
                           m[2][2] * a_matrix.m[2][2] + m[2][3] * a_matrix.m[3][2];
        a_result.m[2][3] = m[2][0] * a_matrix.m[0][3] + m[2][1] * a_matrix.m[1][3] +
                           m[2][2] * a_matrix.m[2][3] + m[2][3] * a_matrix.m[3][3];

        a_result.m[3][0] = m[3][0] * a_matrix.m[0][0] + m[3][1] * a_matrix.m[1][0] +
                           m[3][2] * a_matrix.m[2][0] + m[3][3] * a_matrix.m[3][0];
        a_result.m[3][1] = m[3][0] * a_matrix.m[0][1] + m[3][1] * a_matrix.m[1][1] +
                           m[3][2] * a_matrix.m[2][1] + m[3][3] * a_matrix.m[3][1];
        a_result.m[3][2] = m[3][0] * a_matrix.m[0][2] + m[3][1] * a_matrix.m[1][2] +
                           m[3][2] * a_matrix.m[2][2] + m[3][3] * a_matrix.m[3][2];
        a_result.m[3][3] = m[3][0] * a_matrix.m[0][3] + m[3][1] * a_matrix.m[1][3] +
                           m[3][2] * a_matrix.m[2][3] + m[3][3] * a_matrix.m[3][3];
    }


    //-----------------------------------------------------------------------
    /*!
        Compute the transpose of the current matrix.
        Result is stored in current matrix.
    */
    //-----------------------------------------------------------------------
    inline void trans()
    {
        double t;

        t = m[0][1]; m[0][1] = m[1][0]; m[1][0] = t;
        t = m[0][2]; m[0][2] = m[2][0]; m[2][0] = t;
        t = m[0][3]; m[0][3] = m[3][0]; m[3][0] = t;
        t = m[1][2]; m[1][2] = m[2][1]; m[2][1] = t;
        t = m[1][3]; m[1][3] = m[3][1]; m[3][1] = t;
        t = m[2][3]; m[2][3] = m[3][2]; m[3][2] = t;
    }


    //-----------------------------------------------------------------------
    /*!
        Compute the transpose of the current matrix.
        Result is stored in result matrix.

        \param      a_result  Result is stored here.
    */
    //-----------------------------------------------------------------------
    inline void transr(cMatrixGL& a_result)
    {
        a_result.m[0][0] = m[0][0];
        a_result.m[0][1] = m[1][0];
        a_result.m[0][2] = m[2][0];
        a_result.m[0][3] = m[3][0];

        a_result.m[1][0] = m[0][1];
        a_result.m[1][1] = m[1][1];
        a_result.m[1][2] = m[2][1];
        a_result.m[1][3] = m[3][1];

        a_result.m[2][0] = m[0][2];
        a_result.m[2][1] = m[1][2];
        a_result.m[2][2] = m[2][2];
        a_result.m[2][3] = m[3][2];

        a_result.m[3][0] = m[0][3];
        a_result.m[3][1] = m[1][3];
        a_result.m[3][2] = m[2][3];
        a_result.m[3][3] = m[3][3];
    }


    //-----------------------------------------------------------------------
    /*!
        Create a frustrum matrix. To be documented.
    */
    //-----------------------------------------------------------------------
    inline void buildFrustumMatrix(double l, double r, double b, double t,
                                   double n, double f)
    {
        m[0][0] = (2.0*n) / (r-l);
        m[0][1] = 0.0;
        m[0][2] = 0.0;
        m[0][3] = 0.0;

        m[1][0] = 0.0;
        m[1][1] = (2.0*n) / (t-b);
        m[1][2] = 0.0;
        m[1][3] = 0.0;

        m[2][0] = (r+l) / (r-l);
        m[2][1] = (t+b) / (t-b);
        m[2][2] = -(f+n) / (f-n);
        m[2][3] = -1.0;

        m[3][0] = 0.0;
        m[3][1] = 0.0;
        m[3][2] = -(2.0*f*n) / (f-n);
        m[3][3] = 0.0;
    }


    //-----------------------------------------------------------------------
    /*!
        Compute matrix inverse. Result is stored in current matrix

        \return     Return \b true if operation succeeds. Otherwize \b false.
    */
    //-----------------------------------------------------------------------
    bool inline invert()
    {
        #define SWAP_ROWS(a, b) { GLdouble *_tmp = a; (a)=(b); (b)=_tmp; }
        #define MAT(m,r,c) (m)[(c)*4+(r)]

         double *mat = m[0];

         GLdouble wtmp[4][8];
         GLdouble m0, m1, m2, m3, s;
         GLdouble *r0, *r1, *r2, *r3;

         r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

         r0[0] = MAT(mat,0,0), r0[1] = MAT(mat,0,1),
         r0[2] = MAT(mat,0,2), r0[3] = MAT(mat,0,3),
         r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

         r1[0] = MAT(mat,1,0), r1[1] = MAT(mat,1,1),
         r1[2] = MAT(mat,1,2), r1[3] = MAT(mat,1,3),
         r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

         r2[0] = MAT(mat,2,0), r2[1] = MAT(mat,2,1),
         r2[2] = MAT(mat,2,2), r2[3] = MAT(mat,2,3),
         r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

         r3[0] = MAT(mat,3,0), r3[1] = MAT(mat,3,1),
         r3[2] = MAT(mat,3,2), r3[3] = MAT(mat,3,3),
         r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

         // choose pivot
         if (fabs(r3[0])>fabs(r2[0])) SWAP_ROWS(r3, r2);
         if (fabs(r2[0])>fabs(r1[0])) SWAP_ROWS(r2, r1);
         if (fabs(r1[0])>fabs(r0[0])) SWAP_ROWS(r1, r0);
         if (0.0 == r0[0])
         {
          return false;
         }

         // eliminate first variable
         m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
         s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
         s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
         s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
         s = r0[4];
         if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
         s = r0[5];
         if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
         s = r0[6];
         if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
         s = r0[7];
         if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

         // choose pivot
         if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
         if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
         if (0.0 == r1[1])
         {
          return false;
         }

         // eliminate second variable
         m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
         r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
         r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
         s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
         s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
         s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
         s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

         // choose pivot
         if (fabs(r3[2])>fabs(r2[2])) SWAP_ROWS(r3, r2);
         if (0.0 == r2[2])
         {
          return false;
         }

         // eliminate third variable
         m3 = r3[2]/r2[2];
         r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
         r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
         r3[7] -= m3 * r2[7];

         // last check
         if (0.0 == r3[3])
         {
          return false;
         }

         s = 1.0/r3[3];
         r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

         m2 = r2[3];
         s  = 1.0/r2[2];
         r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
         r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
         m1 = r1[3];
         r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
         r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
         m0 = r0[3];
         r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
         r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

         m1 = r1[2];
         s  = 1.0/r1[1];
         r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
         r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
         m0 = r0[2];
         r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
         r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

         m0 = r0[1];
         s  = 1.0/r0[0];
         r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
         r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

         MAT(mat,0,0) = r0[4]; MAT(mat,0,1) = r0[5],
         MAT(mat,0,2) = r0[6]; MAT(mat,0,3) = r0[7],
         MAT(mat,1,0) = r1[4]; MAT(mat,1,1) = r1[5],
         MAT(mat,1,2) = r1[6]; MAT(mat,1,3) = r1[7],
         MAT(mat,2,0) = r2[4]; MAT(mat,2,1) = r2[5],
         MAT(mat,2,2) = r2[6]; MAT(mat,2,3) = r2[7],
         MAT(mat,3,0) = r3[4]; MAT(mat,3,1) = r3[5],
         MAT(mat,3,2) = r3[6]; MAT(mat,3,3) = r3[7];

         return (true);
        #undef MAT
        #undef SWAP_ROWS
    }


    //-----------------------------------------------------------------------
    /*!
        Build perspective matrix. To be documented.
    */
    //-----------------------------------------------------------------------
    inline void buildPerspectiveMatrix(double  fovy, double aspect,
                                       double zNear, double zFar)
    {
        double xMin, xMax, yMin, yMax;

        yMax = zNear * tan(fovy * CHAI_PI / 360.0);
        yMin = -yMax;

        xMin = yMin * aspect;
        xMax = yMax * aspect;

        buildFrustumMatrix(xMin, xMax, yMin, yMax, zNear, zFar);
    }


    //-----------------------------------------------------------------------
    /*!
        Build a 4x4 matrix transform based on the parameters for gluLookAt.
        To be documented.
    */
    //-----------------------------------------------------------------------
    inline void buildLookAtMatrix(double eyex, double eyey, double eyez,
                                double centerx, double centery, double centerz,
                                double upx, double upy, double upz)
    {
        double x[3], y[3], z[3];
        double mag;

        // create rotation matrix

        // Z vector
        z[0] = eyex - centerx;
        z[1] = eyey - centery;
        z[2] = eyez - centerz;
        mag = sqrt( z[0]*z[0] + z[1]*z[1] + z[2]*z[2] );
        if (mag) {  /* mpichler, 19950515 */
          z[0] /= mag;
          z[1] /= mag;
          z[2] /= mag;
        }

        // Y vector
        y[0] = upx;
        y[1] = upy;
        y[2] = upz;

        // X vector = Y cross Z
        x[0] =  y[1]*z[2] - y[2]*z[1];
        x[1] = -y[0]*z[2] + y[2]*z[0];
        x[2] =  y[0]*z[1] - y[1]*z[0];

        // Recompute Y = Z cross X
        y[0] =  z[1]*x[2] - z[2]*x[1];
        y[1] = -z[0]*x[2] + z[2]*x[0];
        y[2] =  z[0]*x[1] - z[1]*x[0];


        // Normalize
        mag = sqrt( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] );
        if (mag) {
          x[0] /= mag;
          x[1] /= mag;
          x[2] /= mag;
        }

        mag = sqrt( y[0]*y[0] + y[1]*y[1] + y[2]*y[2] );
        if (mag) {
          y[0] /= mag;
          y[1] /= mag;
          y[2] /= mag;
        }

        m[0][0] = x[0];  m[1][0] = x[1];  m[2][0] = x[2];  m[3][0] = -x[0]*eyex + -x[1]*eyey + -x[2]*eyez;
        m[0][1] = y[0];  m[1][1] = y[1];  m[2][1] = y[2];  m[3][1] = -y[0]*eyex + -y[1]*eyey + -y[2]*eyez;
        m[0][2] = z[0];  m[1][2] = z[1];  m[2][2] = z[2];  m[3][2] = -z[0]*eyex + -z[1]*eyey + -z[2]*eyez;
        m[0][3] = 0.0;   m[1][3] = 0.0;   m[2][3] = 0.0;   m[3][3] = 1.0;
    }


    //-----------------------------------------------------------------------
    /*!
        Build a 4x4 matrix transform based on the parameters for gluLookAt.
    */
    //-----------------------------------------------------------------------
    inline void buildLookAtMatrix(cVector3d& a_eye, cVector3d&  a_lookAt, cVector3d a_up)
    {
        buildLookAtMatrix(a_eye.x, a_eye.y, a_eye.z,
                          a_lookAt.x, a_lookAt.y, a_lookAt.z,
                          a_up.x, a_up.y, a_up.z);
    }


    //-----------------------------------------------------------------------
    /*!
        Push current OpenGL matrix.
    */
    //-----------------------------------------------------------------------
    inline void glMatrixPush()
    {
        glPushMatrix();
    }


    //-----------------------------------------------------------------------
    /*!
        Load current OpenGL matrix with this cMatrixGL matrix.
    */
    //-----------------------------------------------------------------------
    inline void glMatrixLoad()
    {
        glLoadMatrixd( (const double *)pMatrix() );
    }


    //-----------------------------------------------------------------------
    /*!
        Multiply current OpenGL matrix with this cMatrixGL matrix.
    */
    //-----------------------------------------------------------------------
    inline void glMatrixMultiply()
    {
        glMultMatrixd( (const double *)pMatrix() );
    }


    //-----------------------------------------------------------------------
    /*!
        Push current OpenGL on the stack and multiply with this cMatrixGL
        matrix.
    */
    //-----------------------------------------------------------------------
    inline void glMatrixPushMultiply()
    {
        glPushMatrix();
        glMultMatrixd( (const double *)pMatrix() );
    }


    //-----------------------------------------------------------------------
    /*!
        Pop current OpenGL matrix off the stack.
    */
    //-----------------------------------------------------------------------
    inline void glMatrixPop()
    {
        glPopMatrix();
    }


    //-----------------------------------------------------------------------
    /*!
        Convert current matric into a string.

		\param    a_string   String where conversion is stored
		\param    a_precision  Number of digits.
    */
    //-----------------------------------------------------------------------
    inline void str(string a_string, int a_precision)
    {
        a_string.append("[ ");
        for (int i=0; i<4; i++)
        {
            a_string.append("( ");
            for (int j=0; j<4; j++)
            {
                cStr(a_string, m[j][i], a_precision);
                if (j<3)
                {
                    a_string.append(", ");
                }
            }
            a_string.append(" ) ");
        }
        a_string.append("]");
    }
};


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
