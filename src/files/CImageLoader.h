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
    \author:    Dan Morris
    \version    1.0
    \date       03/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CImageLoaderH
#define CImageLoaderH
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------

// Global utility functions:

// Finds the extension in a filename and returns a pointer
// to the character after the '.' in the original string,
// or 0 if no '.' is found.
char* find_extension(const char* input);

// converts a string to lower-case
void string_tolower(char* dest,const char* source);

// Finds only the _path_ portion of source, and copies it with
// _no_ trailing path separator to dest.  If there's no /'s or \'s,
// writes null
void find_directory(char* dest, const char* source);


//===========================================================================
/*!
      \class    cFileLoaderBMP
      \brief    cFileLoaderBMP provides a class to load BMp bitmap
                images into memory.
*/
//===========================================================================
class cImageLoader
{
  public:

    cImageLoader();
    cImageLoader(char* filename);

    virtual ~cImageLoader();

    //! Get a pointer to the actual image data... use with care...
    inline unsigned char* getData() { return m_data;   }

    //! Get width of image.
    inline unsigned int getWidth()  { return m_width;  }

    //! Get height of image.
    inline unsigned int getHeight() { return m_height; }

    //! Get the format (GL_RGB or GL_RGBA) of the image
    inline unsigned int getFormat() { return m_format; }

    //! Get the number of bits per pixel used to store this image
    inline unsigned int getBitsPerPixel() { return m_bits_per_pixel; }

    //! Returns 1 if a file has been successfully loaded, 0 otherwise
    inline unsigned int initialized() { return m_initialized; }

    //! Loads this image from the specified file.  Returns 0 if all
    //! goes well, <0 for an error.  Note that regardless of whether
    //! it succeeds, this over-writes any image that had previously
    //! been loaded.
    //!
    //! Always converts the resulting image to RGBA.
    int loadFromFile(const char* filename);

  private:

    void defaults();
    void cleanup();

    char m_filename[_MAX_PATH];
    int m_width, m_height;

    // Either GL_RGB or GL_RGBA
    unsigned int m_format;
    unsigned int m_bits_per_pixel;

    void convert_to_rgba();

    unsigned char* m_data;

    bool m_initialized;
};

#endif