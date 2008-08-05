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
#include "CLight.h"
#include "CMacrosGL.h"
#include "CWorld.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      When creating an OpenGL light source, an identity number between 0 and 7
      is attributed to the light. This number is used during the
      rendering process. Lights are automatically created inside class cWorld.

      \fn       cLight::cLight(const cWorld& a_world)
      \param    a_world parent world in which light source will be located
*/
//===========================================================================
cLight::cLight(cWorld* a_world)
{
    // read light number in current world
    m_parentWorld = a_world;

    if (m_parentWorld != NULL)
    {
        m_parentWorld->addLightSource(this);
    }

    // set default position and orientation
    m_dir.set(0.0, 0.0, -1.0);

    // set default color parameters
    m_ambient.set(0.4f, 0.4f, 0.4f, 1.0f);
    m_diffuse.set(0.7f, 0.7f, 0.7f, 1.0f);
    m_specular.set(0.1f, 0.1f, 0.1f, 1.0f);

    // set default cutoff angle
    m_cutOffAngle   = 90.0;

    // set default attenuation parameters  ATT = 1 / (Kc + Kl*d + Kq*d^2)
    // Attenuation Constant.
    m_attConstant   = 1.0;

    // Attenuation Linear.
    m_attLinear     = 0.0;

    // Attenuation Quadratic.
    m_attQuadratic  = 0.0;

    // set default spot exponent. Repartition of Light in CutOff Angle (GAUSSIAN)
    m_spotExponent = 100.0;

    // Is the light source enabled?
    m_enabled = false;
}


//===========================================================================
/*!
      Destructor of cLight.

      \fn       cLight::~cLight()
*/
//===========================================================================
cLight::~cLight()
{
  // disable light source
  glDisable(m_glLightNumber);
  m_enabled = false;
  m_parentWorld->removeLightSource(this);
}


//===========================================================================
/*!
      Set direction of light beam.

      \fn       void cLight::setDir(cVector3d& a_direction)
      \param    a_direction  Direction of light beam
*/
//===========================================================================
void cLight::setDir(const cVector3d& a_direction)
{
    cVector3d curDirection = m_localRot.getCol0();

    double angle = 0;//cAngle(curDirection, a_direction);
    if (angle > 0)
    {
        cVector3d axis = cNormalize(cCross(curDirection, a_direction));
        m_localRot.rotate(axis, angle);
    }
}


//===========================================================================
/*!
      Set direction of light beam.

      \fn       void cLight::setDir(const double a_x, const double a_y,
      \         const double a_z)
      \param    a_x   X Coordinate of light beam.
      \param    a_y   Y Coordinate of light beam.
      \param    a_z   Z Coordinate of light beam.
*/
//===========================================================================
void cLight::setDir(const double a_x, const double a_y, const double a_z)
{
    setDir(cVector3d(a_x, a_y, a_z));
}


//===========================================================================
/*!
      Set the cutoff angle of the light beam.

      \fn       void cLight::setCutOffAngle(const GLfloat& a_value)
      \param    a_value  Cutoff angle of light beam.
*/
//===========================================================================
void cLight::setCutOffAngle(const GLfloat& a_value)
{
    // temp variable
    GLfloat t_newAngle;

    // check if a negative value was given in which case light is set to
    // a non spot configuratation
    if (a_value < 0) { t_newAngle = 180.0; }

    // check if angle is equal to 180.0 degrees. This corresponds to a non spot light.
    else if (a_value == 180.0) { t_newAngle = 180.0; }

    // check if value ranges between 0 and 90. This corresponds to a spot light.
    else if ((a_value >= 0.0) && (a_value <= 90.0)) { t_newAngle = a_value; }

    // value is incorrect. Light is set to a non spot light configuration.
    else { t_newAngle = 180.0; }

    // assign new value
    m_cutOffAngle = t_newAngle;
}


//===========================================================================
/*!
      Render the light source in OpenGL.

      \fn       void cLight::renderLightSource()
*/
//===========================================================================
void cLight::renderLightSource()
{
    // check if light source enabled
    if (m_enabled == true)
    {
        // enable light in OpenGL
        glEnable(m_glLightNumber);

        // set lighting components
        glLightfv(m_glLightNumber, GL_AMBIENT,  m_ambient.pColor());
        glLightfv(m_glLightNumber, GL_DIFFUSE, m_diffuse.pColor() );
        glLightfv(m_glLightNumber, GL_SPECULAR, m_specular.pColor());

        // set lighting model
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

        // position light source in space
        glPushMatrix();

        float position[4];
        position[0] = (float)m_globalPos.x;
        position[1] = (float)m_globalPos.y;
        position[2] = (float)m_globalPos.z;
        position[3] = 0.0f;
        glLightfv(m_glLightNumber, GL_POSITION, (const float *)&position);

        glPopMatrix();

        // set cutoff angle
        glLightf(m_glLightNumber, GL_SPOT_CUTOFF, m_cutOffAngle);

        // set direction of light beam
        if (m_cutOffAngle != 180.0)
        {
            cVector3d dir = m_globalRot.getCol0();
            float direction[4];
            direction[0] = (float)dir.x;
            direction[1] = (float)dir.y;
            direction[2] = (float)dir.z;
            direction[3] = 0.0f;
            glLightfv(m_glLightNumber, GL_SPOT_DIRECTION, (const float *)&direction);
        }

        // set attenuation factors
        glLightf(m_glLightNumber, GL_CONSTANT_ATTENUATION, m_attConstant);
        glLightf(m_glLightNumber, GL_LINEAR_ATTENUATION, m_attLinear);
        glLightf(m_glLightNumber, GL_QUADRATIC_ATTENUATION, m_attQuadratic);

        // set exponent factor
        glLightf(m_glLightNumber, GL_SPOT_EXPONENT, m_spotExponent);
    }
    else
    {
        // disable OpenGL light source
        glDisable(m_glLightNumber);
    };
}


