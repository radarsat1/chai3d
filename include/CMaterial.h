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
#ifndef CMaterialH
#define CMaterialH
//---------------------------------------------------------------------------
#include "CColor.h"
#include "windows.h"
#include "gl/gl.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class      cMaterial
      \brief      cMaterial describes the surface properties of a solid called
                  material properties. A material can be described by five
                  different parameters: ambient, diffuse, specular, emission
                  and shininess.
*/
//===========================================================================
struct cMaterial
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cMaterial.
    cMaterial();
    //! Destructor of cMaterial.
    ~cMaterial() {};

    // METHODS:
    //! Set the level of shininess.
    void setShininess(GLuint a_shininess);
    //! Get the level of shininess
    GLuint getShininess() { return (m_shininess); }
    //! set stiffness level [N/m]
    void setStiffness(double a_stiffness);
    //! get stiffness level [N/m]
    inline double getStiffness() { return (m_stiffness); }
    //! set static friction level
    void setStaticFriction(double a_friction);
    //! set dynamic friction level
    void setDynamicFriction(double a_friction);
    //! get static stiffness level [N/m]
    inline double getStaticFriction() { return (m_static_friction); }
    //! get dynamic stiffness level [N/m]
    inline double getDynamicFriction() { return (m_dynamic_friction); }
    //! set transparency level
    void setTransparencyLevel(float a_levelTransparency);
    //! Render the material in OpenGL.
    void render();


    // MEMBERS:
    //! Ambient color property.
    cColorf m_ambient;
    //! Diffuse color property.
    cColorf m_diffuse;
    //! Specular color property.
    cColorf m_specular;
    //! Emission color property.
    cColorf m_emission;

  protected:
    //! stiffness [netwons per meter]
    double m_stiffness;
    //! Static friction constant.
    double m_static_friction;
    //! Dynamic friction constant.
    double m_dynamic_friction;
    //! Level of shininess.
    GLuint m_shininess;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

