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

    // PROPERTIES:
    //! Address of a WAVEFORMATEX structure
    WAVEFORMATEX *pFormat;

    // METHODS:
    //! Set material sound parameters
    void setParams(sounds choice);
    //! Render the sound
    void play(cVector3d forcei, double velocity);
    //! Callback function for writing to sound card
    static void CALLBACK writeCallback(HWAVEOUT hwo, UINT uMsg, cSound* dwInstance,
          DWORD param1, DWORD param2);
    //! Write data to the sound card
    void writeSound();

  private:
    // PROPERTIES:
    //! Size of wave format section
    DWORD dwFmtSize;
    //! Size of wave data section
    DWORD dwDataSize;
    //! Handle for wave format section
    HANDLE hFormat;
    //! Handle for wave header
    HGLOBAL hWaveHdr;
    //! Handle for sound device
    HWAVEOUT hWaveOut;
    //! Array of addresses of WAVEHDR structures
    LPWAVEHDR *lpWaveHdrs;
    //! Sound wave data
    HPSTR lpData;
    //! Number of buffers to use
    int numBuffers;
    //! Has the sound output device already been written to?
    bool started;
    //! Should continuous contact sound like rolling?
    bool roll;
    //! Normal force magnitude from previous iteration
    double prevMag;
    //! Number of modes
    int n, m;
    //! Maximum number of samples to play for an impact sound
    int soundSize;
    //! Frequency, amplitude, and decay coefficient material sound properties
    vector<double> a, f, d, fs;
    //! Arrays for calculation sound data
    double *yreal, *yimag, *treal, *timag, *tyreal, *tyimag;
    //! Current buffer being written to
    int tog;
    //! Number of samples generated since last impulse force
    long cnt;
    //! Number of samples generated since last movement along object surface
    long lastMoved;
    //! Sound force attenuation magnitude
    double attMag;
    //! Value to scale sound samples by to be in 0-256 range
    double scale;
    //! Audio force
    double audioForce;
    //! Total distance moved since last reset
    double vSum;
    //! Highest frequency used for rolling sounds
    double cutOff;
    //! Does the buffer have sound data to be played?
    bool *valid;
    //! Should the sound device be reset to immediately start playing this buffer?
    bool *reset;
    //! Index in the current buffer being written
    unsigned int pos;
    //! Is sound playback running?
    bool running;
    //! Is sound playback ready?
    int ready;
    //! Is sound playback active?
    int active;
    //! Is this an initial contact?
    bool firstHit;
    //! Force magnitude
    double normMag;
    //! Volume level
    int vol;
    //! Is control in the callback function?
    static bool inCallBack;
};

#endif
//===========================================================================
//  END OF FILE
//===========================================================================
