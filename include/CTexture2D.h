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
#ifndef CTexture2DH
#define CTexture2DH
//---------------------------------------------------------------------------
#include "CImageLoader.h"
#include "CColor.h"
#include "gl/glu.h"
#include <string>
#include <stdio.h>
//---------------------------------------------------------------------------

//===========================================================================
/*!
      \class      cTexture2D
      \brief      cTexture2D describes a 2D bitmap texture used for OpenGL
                  texture-mapping
*/
//===========================================================================
class cTexture2D
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cTexture2D.
    cTexture2D();

    //! Destructor of cTexture2D.
    ~cTexture2D();

    //! Load an image file (CHAI currently supports 24-bit .bmp and 32-bit .tga files)
    bool loadFromFile(char* a_fileName);

    //! Enable texturing and set this texture as the current texture
    void render();
        
    //! Call this to force texture re-initialization
    void markForUpdate() { m_update_texture_flag = 1; }

    //! OpenGL texture mode (GL_MODULATE, GL_DECAL, GL_BLEND, GL_REPLACE, or -1 for "don't set")
    GLint m_texture_environment_mode;

    //! OpenGL texture quality, or -1 (default) to use the current quality level
    GLint m_min_filter;
    GLint m_mag_filter;

    //! Image loader (use this to get data about the texture itself)
    cImageLoader m_image_loader;
        
  private:
    // METHODS:

    //! Reset internal variables. This function should be called only by constructors/
    void reset();

    //! copy texture to memory video graphics card.
    void update();

    // MEMBERS:

    //! If \b true, texture bitmap has not yet been sent to video card.
    bool m_update_texture_flag;

    //! OpenGL texture ID number.
    GLuint m_textureID;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

