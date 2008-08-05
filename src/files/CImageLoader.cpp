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
#include "CImageLoader.h"
#include "CFileLoaderBMP.h"
#include "CFileLoaderTGA.h"
//---------------------------------------------------------------------------
#include <gl/gl.h>
//---------------------------------------------------------------------------


//===========================================================================
/*!
    Constructor of cImageLoader

    \fn     cImageLoader::cImageLoader()
*/
//===========================================================================
cImageLoader::cImageLoader()
{
    // init internal variables
    defaults();
}


//===========================================================================
/*!
    Constructor of cImageLoader. a filename is passed by argument

    \fn     cImageLoader::cImageLoader(char* a_filename)
    \param  a_filename  Image filename
*/
//===========================================================================
cImageLoader::cImageLoader(char* a_filename)
{
    // init internal variables
    defaults();

    // load imeage file
    loadFromFile(a_filename);
}


//===========================================================================
/*!
    Destructor of cImageLoader

    \fn     cImageLoader::~cImageLoader()
*/
//===========================================================================
cImageLoader::~cImageLoader()
{
    // cleanup memory
    cleanup();
}


//===========================================================================
/*!
    Initialize internal variables

    \fn     void cImageLoader::defaults()
*/
//===========================================================================
void cImageLoader::defaults()
{
    m_filename[0] = '\0';
    m_width = m_height = -1;
    m_data = 0;
    m_initialized = 0;
}


//===========================================================================
/*!
    Free memory of all image data.

    \fn     void cImageLoader::defaults()
*/
//===========================================================================
void cImageLoader::cleanup()
{
    if (m_data) delete [] m_data;
    defaults();
}


//===========================================================================
/*!
    Load image file by passing image path and name as argument

    \fn     int cImageLoader::loadFromFile(const char* a_filename)
    \param  a_filename  Image filename
*/
//===========================================================================
int cImageLoader::loadFromFile(const char* filename)
{
    // cleanup previous image
    cleanup();

    strncpy(m_filename,filename,_MAX_PATH);
    m_filename[_MAX_PATH-1] = '\0';

    char* extension = find_extension(filename);

    // We need a file extension to figure out file type
    if (extension == 0) {
      cleanup();
      return -1;
    }

    char lower_extension[1024];
    string_tolower(lower_extension,extension);

    //--------------------------------------------------------------------
    // Load a .tga image
    //--------------------------------------------------------------------
    if (strcmp(lower_extension,"tga")==0)
    {
        cFileLoaderTGA targa_image;

        // Load the targa file from disk
        int result = targa_image.LoadFromFile(m_filename);
        if (result == 0)
        {
            cleanup();
            return -1;
        }

        m_width = targa_image.GetImageWidth();
        m_height = targa_image.GetImageHeight();

        // Find the correct openGL format for this .tga file
        GLenum format = (targa_image.GetImageType());

        if (format == itRGB)
        {
            m_bits_per_pixel = 24;
            m_format = GL_RGB;
        }

        else if (format == itRGBA)
        {
            m_bits_per_pixel = 32;
            m_format = GL_RGBA;
        }

        else
        {
            // Unrecognized format...
            cleanup();
            return -1;
        }

        m_data = new unsigned char[m_width*m_height*(m_bits_per_pixel/8)];

        // Copy tga data into our internal data record
        memcpy(m_data,targa_image.GetPixels(),(m_bits_per_pixel/8)*m_width*m_height);
    }

    //--------------------------------------------------------------------
    // Load a .bmp image
    //--------------------------------------------------------------------
    else if (strcmp(lower_extension,"bmp")==0)
    {
        cFileLoaderBMP bmp_image;

        int result = bmp_image.loadBMP(m_filename);
        if (result == 0)
        {
            cleanup();
            return -1;
        }

        m_width = bmp_image.getWidth();
        m_height = bmp_image.getHeight();

        // The bitmap loader forces everything into 24-bit RGB
        GLenum format = GL_RGB;
        m_bits_per_pixel = 24;

        m_data = new unsigned char[m_width*m_height*(m_bits_per_pixel/8)];

        // Copy bmp data into our internal data record
        memcpy(m_data,bmp_image.pBitmap(),(m_bits_per_pixel/8)*m_width*m_height);
    }

    //--------------------------------------------------------------------
    // Unrecognized file format
    //--------------------------------------------------------------------
    else
    {
        m_initialized = 0;
        return -1;
    }

    //--------------------------------------------------------------------
    // Finalize
    //--------------------------------------------------------------------
    m_initialized = 1;

    convert_to_rgba();

    return 0;
}


//===========================================================================
/*!
    Find the extension of a filename

    \fn     char* find_extension(const char* a_input)
    \param  a_input Filename
    \return Return extension of filename
*/
//===========================================================================
char* find_extension(const char* input)
{
    int length = strlen(input);

    char* curpos = (char*)(input + length - 1);

    // The last character can never be the '.' preceding a valid
    // extension
    curpos--;

    // Look for the last '.'
    while( (curpos > input) && (*curpos != '.')) curpos--;

    // No '.' found
    if (curpos == input) return 0;

    return curpos + 1;
}


//===========================================================================
/*!
    Convert image to OpenGL compatible RGBA format

    \fn     void cImageLoader::convert_to_rgba()
*/
//===========================================================================
void cImageLoader::convert_to_rgba()
{
    if (m_initialized == 0) return;

    // Don't convert if we're already RGBA
    if (m_format == GL_RGBA) return;

    char* data = new char[m_width*m_height*4];

    int size = m_width*m_height;
    char* original_image_pos = (char*)(m_data);
    char* new_image_pos = data;
    for(int i=0; i<size; i++) {

      new_image_pos[0] = original_image_pos[0];
      new_image_pos[1] = original_image_pos[1];
      new_image_pos[2] = original_image_pos[2];

      new_image_pos += 4;
      original_image_pos += 3;

    }

    delete [] m_data;
    m_data = (unsigned char*)data;

    m_format = GL_RGBA;
}


//===========================================================================
/*!
    Extract directory

    \fn     void find_directory(char* a_dest, const char* a_source)
    \param  a_dest  String which will contain the directory
    \return a_source  Input string containing path and filename
*/
//===========================================================================
void find_directory(char* a_dest, const char* a_source)
{
    strcpy(a_dest,a_source);

    int len = strlen(a_dest);
    int last_separator_index = 0;

    for(int i=0; i<len; i++) {
      if (a_dest[i] == '/' || a_dest[i] == '\\') last_separator_index = i;
    }

    a_dest[last_separator_index] = '\0';
}


//===========================================================================
/*!
    Convert a string into lower case.

    \fn    void string_tolower(char* dest,const char* source)
    \param  a_dest  Output string.
    \return a_source  Input string.
*/
//===========================================================================
void string_tolower(char* dest,const char* source)
{
    int len = strlen(source);

    for(int i=0; i<len; i++)
    {
        dest[i] = tolower(source[i]);
    }

    dest[len] = '\0';
}

