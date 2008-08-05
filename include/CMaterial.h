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
      \brief      cMaterial describes the graphic and haptic properties of a
                  solid.
                  
                  Graphic properties include the OpenGL favorites:
                  
                  ambient color, diffuse color, specular color, emissive color, and shininess

                  Haptic properties currently include stiffness, dynamic friction, and static friction
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

    //! Render the material in OpenGL
    virtual void render();


    // METHODS - GRAPHIC PROPERTIES:

    //! Set shininess (the exponent used for specular lighting)
    void setShininess(GLuint a_shininess);
    //! Get shininess
    GLuint getShininess() { return (m_shininess); }

    //! set transparency level (sets the alpha value for all color properties)
    void setTransparencyLevel(float a_levelTransparency);
    
    
    // METHODS - HAPTIC PROPERTIES:

    //! set stiffness level [N/m]
    void setStiffness(double a_stiffness);
    //! get stiffness level [N/m]
    inline double getStiffness() const { return (m_stiffness); }

    //! set static friction level
    void setStaticFriction(double a_friction);
    //! set dynamic friction level
    void setDynamicFriction(double a_friction);
    //! get static friction level
    inline double getStaticFriction() const { return (m_static_friction); }
    //! get dynamic friction level
    inline double getDynamicFriction() const { return (m_dynamic_friction); }
    

    // MEMBERS:

    //! Ambient color
    cColorf m_ambient;
    //! Diffuse color
    cColorf m_diffuse;
    //! Specular color
    cColorf m_specular;
    //! Emissive color
    cColorf m_emission;


  protected:

    //! Stiffness [netwons per meter]
    double m_stiffness;
    //! Static friction constant
    double m_static_friction;
    //! Dynamic friction constant
    double m_dynamic_friction;
    //! OpenGL shininess
    GLuint m_shininess;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

