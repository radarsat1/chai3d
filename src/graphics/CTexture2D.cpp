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
#include "cTexture2D.h"
//---------------------------------------------------------------------------
#include "gl/glu.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
    A texture contains a 2D bitmap which can be projected onto the
    polygons of a 3D solid.

    \fn         cTexture2D::cTexture2D()
    \return     Return a pointer to new texture instance.
*/
//===========================================================================
cTexture2D::cTexture2D()
{
    // initialize internal variables
    reset();    
}


//===========================================================================
/*!
    Destructor of cTexture2D.

    \fn         cTexture2D::~cTexture2D()
*/
//===========================================================================
cTexture2D::~cTexture2D()
{

  if (m_textureID != -1) {
    glDeleteTextures(1,&m_textureID);
    m_textureID = -1;
  }

}


//===========================================================================
/*!
    Reset internal variables. This function should be called only by constructors.

    \fn         void cTexture2D::reset()
*/
//===========================================================================
void cTexture2D::reset()
{

    m_texture_environment_mode = -1;
    m_min_filter = -1;
    m_mag_filter = -1;

    // id number provided by OpenGL once texture is stored in graphics
    // card memory
    m_textureID = -1;

    // texture has not yet been rendered
    m_update_texture_flag = true;
    
}



//===========================================================================
/*!
      Enable texturing and set this texture as the current texture

      \fn         void cTexture2D::render()
*/
//===========================================================================
void cTexture2D::render()
{
    if (m_image_loader.initialized() == 0) return;
    
    // Only check residency in memory if we weren't going to 
    // update the texture anyway...
    if (m_update_texture_flag == 0) {
    
      GLboolean texture_is_resident;
      glAreTexturesResident(1,&m_textureID,&texture_is_resident);

      if (texture_is_resident == 0) {
        m_update_texture_flag = 1;
      }
    }

    // is texture being rendered for the first time?
    if (m_update_texture_flag) {
      update();
      m_update_texture_flag = false;
    }

    // enable texturing
    glEnable(GL_TEXTURE_2D);

    // make this the current texture
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // set the texture environment mode
    if (m_texture_environment_mode != -1)
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_texture_environment_mode);

    if (m_mag_filter != -1)
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,m_mag_filter);
    
    if (m_min_filter != -1)
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,m_min_filter);
}


//===========================================================================
/*!
      Load an image file (CHAI currently supports 24-bit .bmp and
      32-bit .tga files)

      \fn         bool cTexture2D::loadFromFile(char*)
*/
//===========================================================================
bool cTexture2D::loadFromFile(char* a_fileName)
{
  
  if (m_image_loader.loadFromFile(a_fileName) < 0) {
    // Failure
    return 0;
  }
  else {
    // Success
    return 1;
  }

}

//===========================================================================
/*!
      Generate texture from memory data, to prepare for rendering.

      \fn         void cTexture2D::update()
*/
//===========================================================================
void cTexture2D::update() {

  if (m_textureID != -1) {

    // Deletion makes for all kinds of new hassles, particularly
    // when re-initializing a whole display context, since opengl
    // automatically starts re-assigning texture ID's.  Not worth it.

    // glDeleteTextures(1,&m_textureID);
    // m_textureID = -1;
  }
  
  // Generate a texture ID and bind to it
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1,&m_textureID); 
	glBindTexture(GL_TEXTURE_2D, m_textureID);

  int components = (m_image_loader.getFormat() == GL_RGB ? 3 : 4);

  gluBuild2DMipmaps(GL_TEXTURE_2D, 
						  components, 
						  m_image_loader.getWidth(),
						  m_image_loader.getHeight(),
						  m_image_loader.getFormat(),
						  GL_UNSIGNED_BYTE,
						  m_image_loader.getData()
              );	
              

}
