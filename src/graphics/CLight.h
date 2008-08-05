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
#ifndef CLightH
#define CLightH
//---------------------------------------------------------------------------
#include "windows.h"
#include "gl/gl.h"
#include "CColor.h"
#include "CMaths.h"
#include "CMacrosGL.h"
#include "CGenericObject.h"
//---------------------------------------------------------------------------
class cWorld;
//---------------------------------------------------------------------------


//===========================================================================
/*!
      \class      cLight
      \brief      cLight describes an OpenGL light source. 8 light sources
                  are create inside the world (see cWorld) and can be enabled
                  and tuned independently.
*/
//===========================================================================
class cLight : public cGenericObject
{
  friend class cWorld;

  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cLight.
    cLight(cWorld* a_world);
    //! Destructor of cLight.
    ~cLight();

    // MEMBERS:
    //! Ambient light component.
    cColorf m_ambient;
    //! Diffuse light component.
    cColorf m_diffuse;
    //! Specular light component.
    cColorf m_specular;

    // METHODS:
    //! Set direction of light beam.
    void setDir(const cVector3d& a_direction);
    //! Set direction of light beam.
    void setDir(const double a_x, const double a_y, const double a_z);
    //! Read direction of light beam
    cVector3d getDir() const { return (m_localRot.getCol0()); }
    //! Set constant attenuation parameter.
    void setAttConstant(const GLfloat& a_value) { m_attConstant = cClamp(a_value, 0.0f, 1.0f); }
    //! Read constant attenuation parameter.
    GLfloat getAttConstant() const { return (m_attConstant); }
    //! Set linear attenuation parameter.
    void setAttLinear(const GLfloat& a_value) { m_attLinear = cClamp(a_value, 0.0f, 1.0f); }
    //! Read linear attenuation parameter.
    GLfloat getAttLinear() const { return (m_attLinear); }
    //! Set quadratic attenuation parameter.
    void setAttQuadratic(const GLfloat& a_value) { m_attQuadratic = cClamp(a_value, 0.0f, 1.0f); }
    //! Read quadratic attenuation parameter.
    GLfloat getAttQuadratic() const { return (m_attQuadratic); }
    //! Set concentration level of the light.
    void setSpotExponent(const GLfloat& a_value) { m_spotExponent = cClamp(a_value, 0.0f, 100.0f); }
    //! Read concentration level of the light.
    GLfloat getSpotExponent() const { return (m_spotExponent); }
    //! Set Cut off angle (for spot lights, values ranges between [0, 90]) otherwize set 180.
    void setCutOffAngle(const GLfloat& a_value);
    //! Read Cut off angle.
    GLfloat getCutOffAngle() const { return (m_cutOffAngle); }
    //! Set enable status of light source.
    void setEnabled(const bool& a_enabled) { m_enabled = a_enabled; }
    //! Read enable status of light source.
    bool getEnabled() const { return (m_enabled); }

  protected:
    // MEMBERS:
    //! Parent world
    cWorld* m_parentWorld;
    //! Direction of the spot beam.
    cVector3d m_dir;
    //! Constant attenuation parameter.
    GLfloat m_attConstant;
    //! Linear attenuation parameter.
    GLfloat m_attLinear;
    //! Quadratic attenuation parameter.
    GLfloat m_attQuadratic;
    //! Concentration of the light.
    GLfloat m_spotExponent;
    //! Cut off angle (for spot lights only). Only values in the range [0, 90], and the special value 180, are accepted
    GLfloat m_cutOffAngle;
    //! Enable light source (on/off)
    bool m_enabled;
    //! Identity number of the light (1-8) for OpenGL purpose only.
    GLint m_glLightNumber;

    // METHODS:
    //! Render the light in OpenGL.
    void renderLightSource();
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

