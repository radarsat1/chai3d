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
#ifndef CVector3dH
#define CVector3dH
//---------------------------------------------------------------------------
#include "CString.h"
#include <math.h>
//---------------------------------------------------------------------------

#ifdef _MSVC
#include <conio.h>
#define CHAI_DEBUG_PRINT _cprintf
#else 
#define CHAI_DEBUG_PRINT printf
#endif

//===========================================================================
/*!
      \struct   cVector3d
      \brief    cVector3d represents a 3D vector in 3 dimensional
				space with components X, Y and Z.
*/
//===========================================================================
struct cVector3d
{
    public:
    // MEMBERS:
    //! Component X of vector.
    double x;
    //! Component Y of vector.
    double y;
    //! Component Z of vector.
    double z;


    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------
    /*!
      Constructor of cVector3d.
    */
    //-----------------------------------------------------------------------
    cVector3d() {}

      
    //-----------------------------------------------------------------------
    /*!
      Constructor of cVector3d. Components X, Y and Z are passed
      as parameter.

      \param  a_x  X component.
      \param  a_y  Y component.
      \param  a_z  Z component.
    */
    //-----------------------------------------------------------------------
    cVector3d(const double& a_x, const double& a_y, const double& a_z)
    {
      x = a_x;
      y = a_y;
      z = a_z;
      };


      // METHODS:
      //-----------------------------------------------------------------------
      /*!
          Clear vector with zeros.
      */
      //-----------------------------------------------------------------------
    inline void zero()
    {
      x = 0.0;
      y = 0.0;
      z = 0.0;
    }


    //-----------------------------------------------------------------------
    /*!
        Return the i th component of the vector. a_component = 0 return x,
        a_component = 1 returns y, a_component = 2 returns z.

        \param  a_component  component number
    */
    //-----------------------------------------------------------------------
    inline double get(const unsigned int& a_component) const
    {
        return ((double*)(this))[a_component];
    }
    inline double operator[] (const unsigned int& a_component) const
    {
        return ((double*)(this))[a_component];
    }

    /*!
        An overloaded /= operator for vector/scalar division
    */
    inline void operator/= (const double& a_val)
    {
      x /= a_val;
      y /= a_val;
      z /= a_val;
    }

    /*!
        An overloaded *= operator for vector/scalar multiplication
    */
    inline void operator*= (const double& a_val)
    {
      x *= a_val;
      y *= a_val;
      z *= a_val;
    }

    /*!
        An overloaded += operator for vector/vector addition
    */
    inline void operator+= (const cVector3d& a_input)
    {
      x += a_input.x;
      y += a_input.y;
      z += a_input.z;      
    }

    /*!
        An overloaded -= operator for vector/vector subtraction
    */
    inline void operator-= (const cVector3d& a_input)
    {
      x -= a_input.x;
      y -= a_input.y;
      z -= a_input.z;      
    }

    //-----------------------------------------------------------------------
    /*!
        Return the i th component of the vector. a_component = 0 return x,
        a_component = 1 returns y, a_component = 2 returns z.

        This is the non-const version of this function, so its return value
        can be used as an l-value.

        \param  a_component  component number
    */
    //-----------------------------------------------------------------------
    inline double& operator[] (const unsigned int& a_component)
    {
        return ((double*)(this))[a_component];
    }

    //-----------------------------------------------------------------------
    /*!
      Initialize 3 dimensional vector with parameters \c a_X, \c a_Y
      and \c a_Z.

      \param  a_x  X component.
      \param  a_y  Y component.
      \param  a_z  Z component.
    */
    //-----------------------------------------------------------------------
    inline void set(const double& a_x, const double& a_y, const double& a_z)
    {
      x = a_x;
      y = a_y;
      z = a_z;
    }


    //-----------------------------------------------------------------------
    /*!
      Copy current vector to external vector as parameter.

      \param  a_destination  Destination vector.
    */
    //-----------------------------------------------------------------------
    inline void copyto(cVector3d& a_destination) const
    {
      a_destination.x = x;
      a_destination.y = y;
      a_destination.z = z;
    }


    //-----------------------------------------------------------------------
    /*!
      Copy external vector as parameter to current vector.

      \param  a_source  Source vector.
    */
    //-----------------------------------------------------------------------
    inline void copyfrom(const cVector3d &a_source)
    {
      x = a_source.x;
      y = a_source.y;
      z = a_source.z;
    }


    //-----------------------------------------------------------------------
    /*!
      Addition between current vector and  external vector passed as
      parameter. \n
      Result is stored in current vector.

      \param  a_vector  This vector is added to current one.
    */
    //-----------------------------------------------------------------------
    inline void add(const cVector3d& a_vector)
    {
      x = x + a_vector.x;
      y = y + a_vector.y;
      z = z + a_vector.z;
    }


    //-----------------------------------------------------------------------
    /*!
      Addition between current vector and external vector passed as
      parameter. \n
      Result is stored in current vector.

      \param  a_x  X component.
      \param  a_y  Y component.
      \param  a_z  Z component.
    */
    //-----------------------------------------------------------------------
    inline void add(const double& a_x, const double& a_y, const double& a_z)
    {
      x = x + a_x;
      y = y + a_y;
      z = z + a_z;
    }


    //-----------------------------------------------------------------------
    /*!
      Addition between current vector and external vector passed as
      parameter.\n  Result is stored in external \e a_result vector.

      \param  a_vector  Vector which is added to current one.
      \param  a_result  Vector where result is stored.
    */
    //-----------------------------------------------------------------------
    inline void addr(const cVector3d& a_vector, cVector3d& a_result) const
    {
      a_result.x = x + a_vector.x;
      a_result.y = y + a_vector.y;
      a_result.z = z + a_vector.z;
    }


    //-----------------------------------------------------------------------
    /*!
      Addition between current vector and vector passed by parameter.\n
      Result is stored in \e a_result vector.

      \param  a_x  X component.
      \param  a_y  Y component.
      \param  a_z  Z component.
      \param  a_result  Vector where result is stored.
    */
    //-----------------------------------------------------------------------
    inline void addr(const double& a_x, const double& a_y, const double& a_z, cVector3d& a_result) const
    {
      a_result.x = x + a_x;
      a_result.y = y + a_y;
      a_result.z = z + a_z;
    }


    //-----------------------------------------------------------------------
    /*!
      Subtraction between current vector and an external vector
      passed as parameter.\n
      Result is stored in current vector.

      \param  a_vector  Vector which is subtracted from current one.
    */
    //-----------------------------------------------------------------------
    inline void sub(const cVector3d& a_vector)
    {
      x = x - a_vector.x;
      y = y - a_vector.y;
      z = z - a_vector.z;
    }


    //-----------------------------------------------------------------------
    /*!
      Subtract an external vector passed as parameter from current
      vector. \n Result is stored in current vector.

      \param  a_x  X component.
      \param  a_y  Y component.
      \param  a_z  Z component.
    */
    //-----------------------------------------------------------------------
    inline void sub(const double& a_x, const double& a_y, const double& a_z)
    {
      x = x - a_x;
      y = y - a_y;
      z = z - a_z;
    }


    //-----------------------------------------------------------------------
    /*!
      Subtraction between current vector and external vector passed as
      parameter.\n  Result is stored in external \e a_result vector.

      \param  a_vector  Vector which is subtracted from current one.
      \param  a_result  Vector where result is stored.
    */
    //-----------------------------------------------------------------------
    inline void subr(const cVector3d& a_vector, cVector3d& a_result) const
    {
      a_result.x = x - a_vector.x;
      a_result.y = y - a_vector.y;
      a_result.z = z - a_vector.z;
    }


    //-----------------------------------------------------------------------
    /*!
      Subtract current vector from vector passed by parameter.\n
      Result is stored in \e a_result vector.

      \param  a_x  X component.
      \param  a_y  Y component.
      \param  a_z  Z component.
      \param  a_result  Vector where result is stored.
    */
    //-----------------------------------------------------------------------
    inline void subr(const double& a_x, const double& a_y, const double& a_z,
                  cVector3d &a_result) const
    {
      a_result.x = x - a_x;
      a_result.y = y - a_y;
      a_result.z = z - a_z;
    }


    //-----------------------------------------------------------------------
    /*!
      Multiply current vector by a scalar. \n
      Result is stored in current vector.

      \param  a_scaler  Scalar value.
    */
    //-----------------------------------------------------------------------
    inline void mul(const double &a_scalar)
    {
      x = a_scalar * x;
      y = a_scalar * y;
      z = a_scalar * z;
    }


    //-----------------------------------------------------------------------
    /*!
      Multiply current vector by a scalar. \n
      Result is stored in \e a_result vector.

      \param  a_scalar  Scalar value.
      \param  a_result  Result vector.
    */
    //-----------------------------------------------------------------------
    inline void mulr(const double& a_scalar, cVector3d& a_result) const
    {
      a_result.x = a_scalar * x;
      a_result.y = a_scalar * y;
      a_result.z = a_scalar * z;
    }


    //-----------------------------------------------------------------------
    /*!
      Divide current vector by a scalar. \n
      \b Warning: Do not passed the value zero (0) or a division
      by zero error will occur. \n
      Result is stored in current vector.

      \param  a_scalar  Scalar value.
    */
    //-----------------------------------------------------------------------
    inline void div(const double &iValue)
    {
      x = x / iValue;
      y = y / iValue;
      z = z / iValue;
    }


    //-----------------------------------------------------------------------
    /*!
      Divide current vector by a scalar. \n
      Result is stored in \e a_result vector.

      \param  a_scalar  Scalar value.
      \param  a_result  Result vector.
    */
    //-----------------------------------------------------------------------
    inline void divr(const double& a_scalar, cVector3d& a_result) const
    {
      a_result.x = x / a_scalar;
      a_result.y = y / a_scalar;
      a_result.z = z / a_scalar;
    }


    //-----------------------------------------------------------------------
    /*!
      Negate current vector. \n
      Result is stored in current vector.
    */
    //-----------------------------------------------------------------------
    inline void negate()
    {
      x = -x;
      y = -y;
      z = -z;
    }


    //-----------------------------------------------------------------------
    /*!
      Negate current vector. \n
      Result is stored in \e a_result vector.

      \param  a_result  Result vector.
    */
    //-----------------------------------------------------------------------
    inline void negater(cVector3d& a_result) const
    {
      a_result.x = -x;
      a_result.y = -y;
      a_result.z = -z;
    }


    //-----------------------------------------------------------------------
    /*!
      Compute the cross product between current vector and an external
      vector. \n Result is stored in current vector.

      \param  a_vector  Vector with which cross product is computed with.
    */
    //-----------------------------------------------------------------------
    inline void cross(const cVector3d& a_vector)
    {
      // compute cross product
      double a =  (y * a_vector.z) - (z * a_vector.y);
      double b = -(x * a_vector.z) + (z * a_vector.x);
      double c =  (x * a_vector.y) - (y * a_vector.x);

      // store result in current vector
      x = a;
      y = b;
      z = c;
    }


    //-----------------------------------------------------------------------
    /*!
      Compute the cross product between current vector and an
      external vector passed as parameter. \n
      Result is stored in \e a_result vector.

      \param  a_vector  Vector with which cross product is computed.
      \param  a_result  Vector where result is stored.
    */
    //-----------------------------------------------------------------------
    inline void crossr(const cVector3d& a_vector, cVector3d& a_result) const
    {
      a_result.x =  (y * a_vector.z) - (z * a_vector.y);
      a_result.y = -(x * a_vector.z) + (z * a_vector.x);
      a_result.z =  (x * a_vector.y) - (y * a_vector.x);
    }


    //-----------------------------------------------------------------------
    /*!
      Compute the dot product between current vector and an external vector
      passed as parameter.

      \param  a_vector  Vector with which dot product is computed.
      \return  Returns dot product computed between both vectors.
    */
    //-----------------------------------------------------------------------
    inline double dot(const cVector3d& a_vector) const
    {
      return((x * a_vector.x) + (y * a_vector.y) + (z * a_vector.z));
    }

    //-----------------------------------------------------------------------
    /*!
    Compute the element-by-element product between current vector and an external
    vector and store in the current vector.
    
    \param  a_vector  Vector with which product is computed.
    */
    //-----------------------------------------------------------------------
    inline void elementMul(const cVector3d& a_vector)
    {
      x*=a_vector.x;
      y*=a_vector.y;
      z*=a_vector.z;
    }


    //-----------------------------------------------------------------------
    /*!
    Compute the element-by-element product between current vector and an external
    vector and store in the supplied output vector.

    \param  a_vector  Vector with which product is computed.
    */
    //-----------------------------------------------------------------------
    inline void elementMul(const cVector3d& a_vector, cVector3d& a_result) const
    {
      a_result.x = x*a_vector.x;
      a_result.y = y*a_vector.y;
      a_result.z = z*a_vector.z;
    }


    //-----------------------------------------------------------------------
    /*!
      Compute the length of current vector.

      \return   Returns length of current vector.
    */
    //-----------------------------------------------------------------------
    inline double length() const
    {
      return(sqrt((x * x) + (y * y) + (z * z)));
    }


    //-----------------------------------------------------------------------
    /*!
      Compute the square length of current vector.

      \return   Returns square length of current vector.
    */
    //-----------------------------------------------------------------------
    inline double lengthsq(void) const
    {
      return((x * x) + (y * y) + (z * z));
    }


    //-----------------------------------------------------------------------
    /*!
      Normalize current vector to become a vector of length one.\n
      \b Warning: Vector should not be of equal to (0,0,0) or a division
      by zero error will occur. \n
      Result is stored in current vector.
    */
    //-----------------------------------------------------------------------
    inline void normalize()
    {
      // compute length of vector
      double length = sqrt((x * x) + (y * y) + (z * z));

      // divide current vector by its length
      x = x / length;
      y = y / length;
      z = z / length;
    }


    //-----------------------------------------------------------------------
    /*!
      Normalize current vector to become a vector of length one. \n
      \b WARNING: Vector should not be of equal to (0,0,0) or a division
      by zero error will occur. \n
      Result is stored in \e a_result vector.

      \param  a_result  Vector where result is stored.
    */
    //-----------------------------------------------------------------------
    inline void normalizer(cVector3d& a_result) const
    {
      // compute length of vector
      double length = sqrt((x * x) + (y * y) + (z * z));

      // divide current vector by its length
      a_result.x = x / length;
      a_result.y = y / length;
      a_result.z = z / length;
    }


    //-----------------------------------------------------------------------
    /*!
      Compute the distance between current point and an external point
      passed as parameter.

      \param  a_vector  Point with which distance is measured.
      \return  Returns distance between both points.
    */
    //-----------------------------------------------------------------------
    inline double distance(const cVector3d& a_vector) const
    {
      // compute distance between both points
      double dx = x - a_vector.x;
      double dy = y - a_vector.y;
      double dz = z - a_vector.z;

      // return result
      return(sqrt( dx * dx + dy * dy + dz * dz ));
    }


    //-----------------------------------------------------------------------
    /*!
      Compute the square distance between current point and an external
      point passed as parameter.

      \param  a_vector  Point with which square distance is measured.
      \return  Returns square distance between both points.
    */
    //-----------------------------------------------------------------------
    inline double distancesq(const cVector3d& a_vector) const
    {
      // compute distance between both points
      double dx = x - a_vector.x;
      double dy = y - a_vector.y;
      double dz = z - a_vector.z;

      // return result
      return( dx * dx + dy * dy + dz * dz );
    }


    //-----------------------------------------------------------------------
    /*!
      Compare if current vector and external vector passed as parameter
      are equal.

      \param    a_vector  Vector with which equality is checked.
      \return   Returns \c true if both vectors are equal, otherwise
            returns \c false.
    */
    //-----------------------------------------------------------------------
    inline bool equals(const cVector3d& a_vector) const
    {
      if ( (x == a_vector.x) && (y == a_vector.y) && (z == a_vector.z) )
      {
        return (true);
      }
      else
      {
        return (false);
      }
    }


    //-----------------------------------------------------------------------
    /*!
      Convert current vector into a string.

      \param    a_string   String where conversion is stored
      \param    a_precision  Number of digits.
    */
    //-----------------------------------------------------------------------
    inline void str(string& a_string, const unsigned int a_precision=2) const
    {
      a_string.append("( ");
      cStr(a_string, x, a_precision);
      a_string.append(", ");
      cStr(a_string, y, a_precision);
      a_string.append(", ");
      cStr(a_string, z, a_precision);
      a_string.append(" )");
    }

    //-----------------------------------------------------------------------
    /*!
    Convert current vector into a string, which is returned on the stack.
    \param    a_precision  Number of digits.
    */
    //-----------------------------------------------------------------------
    inline string str(const unsigned int a_precision=2) const
    {
      string a_string;
      str(a_string,a_precision);
      return a_string;
    }

    //-----------------------------------------------------------------------
    /*!
    Print the current vector using the CHAI_DEBUG_PRINT macro.

    \param    a_string   String where conversion is stored
    \param    a_precision  Number of digits.
    */
    //-----------------------------------------------------------------------    
    inline void print(const unsigned int a_precision=2) const
    {
      string s;
      str(s,a_precision);
      CHAI_DEBUG_PRINT("%s\n",s.c_str());
    }    
};

/*!
An overloaded * operator for vector/scalar multiplication
*/
inline cVector3d operator*(const cVector3d& v, const double a_input)
{
  return cVector3d(v.x*a_input,v.y*a_input,v.z*a_input);
}


/*!
An overloaded / operator for vector/scalar division
*/
inline cVector3d operator/(const cVector3d& v, const double a_input)
{
  return cVector3d(v.x/a_input,v.y/a_input,v.z/a_input);
}


/*!
An overloaded * operator for scalar/vector multiplication
*/
inline cVector3d operator*(const double a_input, const cVector3d& v)
{
    return cVector3d(v.x*a_input,v.y*a_input,v.z*a_input);
}


/*!
An overloaded + operator for vector/vector addition
*/
inline cVector3d operator+(const cVector3d& v1, const cVector3d& v2)
{
    return cVector3d(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);
}


/*!
An overloaded - operator for vector/vector subtraction
*/
inline cVector3d operator-(const cVector3d& v1, const cVector3d& v2)
{
    return cVector3d(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);
}


/*!
An overloaded * operator for vector/vector dotting
*/
inline double operator*(const cVector3d& v1, const cVector3d& v2)
{
    return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
