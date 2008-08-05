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

    The sound card output is based on a tutorial by David Overton, at
    http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=4422&lngWId=3.
    Sound generation is based on the algorithm by Kies Van den Doel K and
    Dinesh Pai in "The sounds of physical shapes". Presence 1998, 7(4): 382-395,
    and "The AHI: An Audio and Haptic Interface For Contact Interactions" by
    DiFilippo and Pai.

    \author:    <http://www.chai3d.org>
    \author:    Chris Sewell
    \version    1.1
    \date       06/2005
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CSoundH
#define CSoundH
//---------------------------------------------------------------------------
#include <mmsystem.h>
#include <windowsx.h>
#include <fstream.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include "CMatrix3d.h"
#include "CVector3d.h"

// Materials for which .sy files with sound parameter values are available
enum sounds { BELL, TEAPOT };

//---------------------------------------------------------------------------


//===========================================================================
//  - CLASS DEFINITION -
/*!
      \class      cSound
      \brief      cSound plays sounds based on frequencies, amplitudes,
                  and attenuations from a data file, based on the algorithm
                  by Kies Van den Doel K and Dinesh Pai in "The sounds of
                  physical shapes". Presence 1998, 7(4): 382-395, and
                  "The AHI: An Audio and Haptic Interface For Contact
                  Interactions" by DiFilippo and Pai.  Sample
                  data files for a bell and a teapot are in this directory.
*/
//===========================================================================
class cSound
{
  public:
    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cSound.
     cSound();
    //! Destructor of cSound.
    ~cSound();

    // METHODS:
    //! Set material sound parameters
    void setParams(sounds a_choice);
    //! Render the sound
    void play(cVector3d a_force);

  private:
    // PROPERTIES:
    //! Size of wave data section
    DWORD dwDataSize;
    //! Handle for sound device
    HWAVEOUT hWaveOut;
    //! Wave format struct
    WAVEFORMATEX wfx;
    //! Normal force magnitude from previous iteration
    double prevMag;
    //! Number of modes
    int n;
    //! Maximum number of samples to play for an impact sound
    int soundSize;
    //! Frequency, amplitude, and decay coefficient material sound properties
    vector<double> a, f, d;
    //! Arrays for calculating sound data
    double *yreal, *yimag, *treal, *timag, *tyreal, *tyimag;
    //! Number of samples generated since last impulse force
    long cnt;
    //! Value to scale sound samples by to be in 0-256 range
    double scale;
    //! Index in the current buffer being written
    unsigned int pos;
    //! Should this object be writing sound now?
    int state;
    //! Sound data buffer
    char* buffer;
    //! Id of this sound object
    int id;
};

#endif
//===========================================================================
//  END OF FILE
//===========================================================================
