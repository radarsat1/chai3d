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
    \author:    Francois Conti
    \version    1.0
    \date       03/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CImageLoader.h"
#include "CFileLoaderBMP.h"
#include "CFileLoaderTGA.h"
#include "cMacrosGL.h"

#ifdef _WIN32

// Don't let the ATL headers decide for themselves which lib file to
// link against; let us link here or in client applications...
#define _ATL_NO_DEFAULT_LIBS

// Borland and MSVC put the atl headers in different places
#ifdef _MSVC
#include <atlbase.h>
#else
#include <atl/atlbase.h>
#endif

#endif

//---------------------------------------------------------------------------

// For gl image formats...
#include <gl/gl.h>

//---------------------------------------------------------------------------


//===========================================================================
/*!
    Default onstructor of cImageLoader

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
    Constructor of cImageLoader; loads the specified file.
    Currently 24-bit .bmp and 32-bit .tga files are supported.

    Use the initialized() function to determine whether loading
    was successful.

    \fn     cImageLoader::cImageLoader(char* a_filename)
    \param  a_filename  Image filename
*/
//===========================================================================
cImageLoader::cImageLoader(const char* a_filename)
{
    // init internal variables
    defaults();

    // load image file
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
    // clean up memory
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
    Free memory that was used for image data, and re-initialize
    internal variables.

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
    Loads this image from the specified file.  Returns 0 if all
    goes well, <0 for an error.  Note that regardless of whether
    it succeeds, this over-writes any image that had previously
    been loaded by this object.
    
    Always converts the resulting image to RGBA.    

    \fn     int cImageLoader::loadFromFile(const char* a_filename)
    \param  a_filename  Image filename
*/
//===========================================================================
int cImageLoader::loadFromFile(const char* filename)
{
    // Sometimes we'll make a one-level recursive call into this
    // function; this is an extra safety check to avoid extra
    // recursion...
    static int recursive_call = 0;

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

            // Try again using the windows native loader...
            return loadFromFileOLE(filename);
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

            // Try again using the windows native loader...
            return loadFromFileOLE(filename);
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

            // Try again using the windows native loader...
            return loadFromFileOLE(filename);
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

#ifdef _WIN32

    //--------------------------------------------------------------------
    // Unrecognized file format - use win32 loader
    //--------------------------------------------------------------------
    else {

      return loadFromFileOLE(filename);

    }
      
#else

    //--------------------------------------------------------------------
    // Unrecognized file format an win32 not available - try again as a .bmp
    //--------------------------------------------------------------------
    else if (recursive_call == 0)
    {
        // We don't handle any other file formats, but as a helpful
        // extra, we'll try to open filename.bmp - regardless of the
        // original extension.  This allows users to convert image
        // files in batch, without having to manipulate filenames
        // that might be stored within model files.

        // Make sure we can actually build a replacement extension...
        if (extension == 0) {
            m_initialized = 0;
            return -1;
        }
        // Extra sanity check to avoid deep recursion
        recursive_call = 1;

        char new_filename[MAX_PATH];

        // Replace the extension
        strcpy(new_filename,filename);
        strcpy(new_filename+(extension-filename),"bmp");

        // Try again...
        int result = loadFromFile(new_filename);

        recursive_call = 0;
        return result;
    }

    //--------------------------------------------------------------------
    // Unrecognized file format
    //--------------------------------------------------------------------
    else {
      m_initialized = 0;
      return -1;
    }

#endif

    //--------------------------------------------------------------------
    // Finalize
    //--------------------------------------------------------------------
    m_initialized = 1;

    convertToRGBA();

    return 0;
}


//===========================================================================
/*!
    Finds the extension in a filename and returns a pointer
    to the character after the '.' in the original string,
    or 0 if no '.' is found.

    \fn     char* find_extension(const char* input);
    \param  char* a_input The input filename string
    \return Returns a pointer to the character after the '.', or 0 for an error
*/
//===========================================================================
char* find_extension(const char* a_input)
{
    int length = strlen(a_input);

    char* curpos = (char*)(a_input + length - 1);

    // The last character can never be the '.' preceding a valid
    // extension
    curpos--;

    // Look for the last '.'
    while( (curpos > a_input) && (*curpos != '.')) {
      if (*curpos == '\\') return 0;
      curpos--;
    }

    // No '.' found
    if (curpos == a_input) return 0;

    return curpos + 1;
}


//===========================================================================
/*!
    Convert image to OpenGL-compatible RGBA format

    \fn     void cImageLoader::convertToRGBA()
*/
//===========================================================================
void cImageLoader::convertToRGBA()
{
    if (m_initialized == 0) return;

    // Don't convert if we're already RGBA
    if (m_format == GL_RGBA) return;

    unsigned char* data = new unsigned char[m_width*m_height*4];

    int size = m_width*m_height;
    unsigned char* original_image_pos = m_data;
    unsigned char* new_image_pos = data;
    for(int i=0; i<size; i++) {

      new_image_pos[0] = original_image_pos[0];
      new_image_pos[1] = original_image_pos[1];
      new_image_pos[2] = original_image_pos[2];
      new_image_pos[3] = 255;

      new_image_pos += 4;
      original_image_pos += 3;

    }

    delete [] m_data;
    m_data = data;

    m_format = GL_RGBA;
}


//===========================================================================
/*!
    Finds only the _path_ portion of source, and copies it with
    _no_ trailing '\\'.  If there's no /'s or \\'s, writes an
    empty string

    \fn     void find_directory(char* a_dest, const char* a_source)
    \param  a_dest    String which will contain the directory name
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
    \param  a_dest    Output string
    \param  a_source  Input string    
*/
//===========================================================================
void string_tolower(char* a_dest, const char* a_source)
{
    int len = strlen(a_source);

    for(int i=0; i<len; i++)
    {
        a_dest[i] = tolower(a_source[i]);
    }

    a_dest[len] = '\0';
}



int cImageLoader::loadFromFileOLE(const char* szPathName) {

#ifndef _WIN32
  
    return -1;

#else

    // From: http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=41

    HDC       hdcTemp;              // The DC To Hold Our Bitmap
    HBITMAP   hbmpTemp;             // Holds The Bitmap Temporarily
    IPicture  *pPicture;            // IPicture Interface
    OLECHAR   wszPath[MAX_PATH+1];  // Full Path To Picture (WCHAR)
    char      szPath[MAX_PATH+1];   // Full Path To Picture
    long      lWidth;               // Width In Logical Units
    long      lHeight;              // Height In Logical Units
    
    // Perform any pre-processing necessary for dealing with the
    // path...
    strcpy(szPath, szPathName);

    // Convert From ASCII To Unicode
    MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);

    // Load the picture
    HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

    if(FAILED(hr)) {

      // Try again using .bmp and jpeg extensions, as a nice helpful extra
      // for folks with binary model formats...

      char* extension = find_extension(szPath);
      if (extension == 0) return -1;
      strcpy(extension,"jpg");
      MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);
      hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

      if(FAILED(hr)) {
        char* extension = find_extension(szPath);
        if (extension == 0) return -1;
        strcpy(extension,"bmp");
        MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);
        hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);
      }

      if(FAILED(hr)) {
        return -1;
      }
    }

    // Create The Windows Compatible Device Context
    hdcTemp = CreateCompatibleDC(GetDC(0));

    if(!hdcTemp)														
    {
      pPicture->Release();
      return -1;
    }

    pPicture->get_Width(&lWidth);
    pPicture->get_Height(&lHeight);
    
    // Convert from annoying Windows "logical units" to pixels
    m_width = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
    m_height = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

    //	Create A Temporary Bitmap
    BITMAPINFO	bi = {0};
    DWORD		*pBits = 0;

    bi.bmiHeader.biSize	= sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biWidth = m_width;
    bi.bmiHeader.biHeight = m_height;
    bi.bmiHeader.biCompression	= BI_RGB;
    bi.bmiHeader.biPlanes = 1;

    //	Create a bitmap
    hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);

    if(!hbmpTemp)
    {
      DeleteDC(hdcTemp);
      pPicture->Release();
      return -1;
    }

    // Get ready to render to memory
    SelectObject(hdcTemp, hbmpTemp);

    // Render The IPicture onto the bitmap
    pPicture->Render(hdcTemp, 0, 0, m_width, m_height, 0, 0, lWidth, lHeight, 0);

    m_data = new unsigned char[m_width*m_height*4];

    unsigned char* original_image_pos = (unsigned char*)(pBits);
    unsigned char* new_image_pos = m_data;

    // Convert From BGR To RGBA into our output array
    for(long i = 0; i < m_width* m_height; i++)
    {
        new_image_pos[0] = original_image_pos[2];
        new_image_pos[1] = original_image_pos[1];
        new_image_pos[2] = original_image_pos[0];
        new_image_pos[3] = 255;

        new_image_pos += 4;
        original_image_pos += 4;
    }

    // Clean up
    DeleteObject(hbmpTemp);
    DeleteDC(hdcTemp);
    pPicture->Release();

    m_format = GL_RGBA;
    m_initialized = 1;    

    return 0;
#endif

}
