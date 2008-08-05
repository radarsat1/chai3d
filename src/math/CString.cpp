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
#include "CString.h"
#include "CMacrosGL.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Convert a \e boolean into a \e string.

    \fn     void cStr(string& a_string, const bool& a_value)
    \param    a_string  Input string.
    \param    a_value  Input value of type \e boolean.
*/
//===========================================================================
void cStr(string& a_string, const bool& a_value)
{
    if (a_value) a_string.append("true");
    else a_string.append("false");    
}


//===========================================================================
/*!
    Convert an \e integer into a \e string.

    \fn     void cStr(string& a_string, const int& a_value)
    \param    a_string  Input string.
    \param    a_value  Input value of type \e integer.
*/
//===========================================================================
void cStr(string& a_string, const int& a_value)
{
  char buffer[255];
  sprintf(buffer, "%d", a_value);
  a_string.append(buffer);
}


//===========================================================================
/*!
    Convert a \e float into a \e string.

    \fn       void cStr(string& a_string, const float& a_value, const unsigned int a_precision=2)
    \param    a_string  Input string.
    \param    a_value  Input value of type \e integer.
    \param    a_precision  Number of digits displayed after the decimal pt.
*/
//===========================================================================
void cStr(string& a_string, const float& a_value, const unsigned int a_precision)
{
    int digits = (a_precision > 20) ? 20 : a_precision;
    char buffer[255];
    sprintf(buffer, "%0.*f", digits, a_value);
    a_string.append(buffer);
}


//===========================================================================
/*!
    Convert a \e double into a \e string.

    \fn     void cStr(string& a_string, const double& a_value,
                  const unsigned int a_precision=2);
    \param    a_string  Input string.
    \param    a_value  Input value of type \e integer.
    \param    a_precision  Number of digits displayed after the decimal pt.
*/
//===========================================================================
void cStr(string& a_string, const double& a_value, const unsigned int a_precision)
{
    int digits = (a_precision > 20) ? 20 : a_precision;
    char buffer[255];
    sprintf(buffer, "%0.*f", digits, a_value);
    a_string.append(buffer);
}
