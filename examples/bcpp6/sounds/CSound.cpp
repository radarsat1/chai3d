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
#include <vcl.h>
#pragma hdrstop
#include "CSound.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

bool cSound::inCallBack = false;
//---------------------------------------------------------------------------

//===========================================================================
//  - PUBLIC METHOD -
/*!
      Constructor of cSound.

      \fn         cSound::cSound()
      \return     Return a pointer to new sound instance.
*/
//===========================================================================
cSound::cSound()
{
  started = false;
  active = 0;
  running = false;
  ready = 0;

  // setup wave header struct
  numBuffers = 8;
  lpWaveHdrs = NULL;
  lpData = NULL;
  dwFmtSize = sizeof(WAVEFORMATEX);
  hFormat = LocalAlloc(LMEM_MOVEABLE, LOWORD(dwFmtSize));
  pFormat = (WAVEFORMATEX*) LocalLock(hFormat);
  pFormat->wFormatTag = WAVE_FORMAT_PCM;
  pFormat->nChannels = 0x0001;
  pFormat->nSamplesPerSec = 44000;
  pFormat->wBitsPerSample = 0x0008;
  pFormat->nBlockAlign = (pFormat->nChannels)*(pFormat->wBitsPerSample)/8;
  pFormat->nAvgBytesPerSec = (pFormat->nSamplesPerSec)*(pFormat->nBlockAlign);
  pFormat->cbSize = 0;
  hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, (DWORD) sizeof(WAVEHDR));
  lpWaveHdrs = new LPWAVEHDR[numBuffers];
  valid = new bool[numBuffers];
  reset = new bool[numBuffers];
  for (int i=0; i<numBuffers; i++)
  {
    lpWaveHdrs[i] = (LPWAVEHDR)GlobalLock(hWaveHdr);
    lpWaveHdrs[i]->dwFlags = 0L;
    lpWaveHdrs[i]->dwLoops = 0L;
    lpWaveHdrs[i]->dwFlags = lpWaveHdrs[i]->dwFlags | WHDR_DONE;
  }

  // make sure there is a device available to play sounds; if so, open it
  if (waveOutOpen(&hWaveOut, WAVE_MAPPER, pFormat, 0, 0L, WAVE_FORMAT_QUERY))
    Application->MessageBox("Error in Format Query", "Error");
  if (waveOutOpen(&hWaveOut, WAVE_MAPPER, pFormat, (DWORD)(writeCallback), (DWORD)this, CALLBACK_FUNCTION))
    Application->MessageBox("Error in Opening Device", "Error");
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Destructor of cSound.

      \fn         void cSound::~cSound()
      \return     nothing
*/
//===========================================================================
cSound::~cSound()
{
  // reset sound device, unprepare wave header, and close device
  if (waveOutReset(hWaveOut))
    Application->MessageBox("Error in Resetting Device", "Error");

  for (int i=0; i<numBuffers; i++)
    if (waveOutUnprepareHeader(hWaveOut, lpWaveHdrs[i], sizeof(WAVEHDR)))
      Application->MessageBox("Error in Unpreparing Handle", "Error");

  if (waveOutClose(hWaveOut))
    Application->MessageBox("Error in Closing Device", "Error");

  // free wave header and data
  for (int i=0; i<numBuffers; i++)
  {
    GlobalFreePtr(lpWaveHdrs[i]->lpData);
    GlobalFreePtr(lpWaveHdrs[i]);
  }
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      This method takes as an input one of the pre-defined sound types
      (enumerated in CSound.h), and reads in the corresponding .sy file
      which contains the modes and their amplitudes and decay coefficents
      (data from Kies van den Doel's JASS program).  It also precomputes
      some values.  Reference: "The AHI: An Audio and Haptic Interface
      for Contact Interactions" by DiFilippo and Pai.

      \fn         void cSound::setParams(sounds choice)
*/
//===========================================================================
void cSound::setParams(sounds choice)
{
  char buffer[32];
  prevMag = 0.0;
  soundSize = 100000;
  scale = 1.0;
  m = 100;
  dwDataSize = 1000000;
  FILE*  dfile;

  // select .sy data file based on input sound option
  switch (choice)
  {
    case BELL:
      dfile = fopen("..//handbell.sy", "r");
      scale = 1.0;
      break;
    case TEAPOT:
      dfile = fopen("..//teapot.sy", "r");
      scale = 6.0;
      break;
  }

  // allocate space for data for all buffers
  for (int i=0; i<numBuffers; i++)
  {
    lpWaveHdrs[i]->lpData = (char*)GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE, 5*dwDataSize);
    for (unsigned int j=0; j<5*dwDataSize; j++)
      lpWaveHdrs[i]->lpData[j] = 128.0;
  }

  // read in the modes, amplitudes, and decay coefficients
  fgets(buffer, 256, dfile);
  fgets(buffer, 256, dfile);
  n = atoi(buffer);
  if (n < m)
    m = n;
  a.clear();
  f.clear();
  d.clear();
  for (int i=0; i<11; i++)
    fgets(buffer, 256, dfile);
  for (int i=0; i<n; i++)
  {
    fgets(buffer, 256, dfile);
    if (i<m)
      f.push_back(atof(buffer));
  }
  fgets(buffer, 256, dfile);
  for (int i=0; i<n; i++)
  {
    fgets(buffer, 256, dfile);
    if (i<m)
      d.push_back(atof(buffer));
  }
  fgets(buffer, 256, dfile);
  for (int i=0; i<n; i++)
  {
    fgets(buffer, 256, dfile);
    if (i<m)
      a.push_back(atof(buffer));
  }
  fclose(dfile);

  // allocate space for sound calculations
  treal = new double[m];
  timag = new double[m];
  yreal = new double[m];
  yimag = new double[m];
  tyreal = new double[m];
  tyimag = new double[m];

  // precompute real and imaginary parts of e^(i*omegan/Fs)
  for (int j=0; j<m; j++)
  {
    treal[j] = exp(-d[j]/pFormat->nSamplesPerSec)*cos(2*3.14159*f[j]/pFormat->nSamplesPerSec);
    timag[j] = exp(-d[j]/pFormat->nSamplesPerSec)*sin(2*3.14159*f[j]/pFormat->nSamplesPerSec);
  }

  // set up array and header for sound wave data
  for (int i=0; i<numBuffers; i++)
  {
    lpWaveHdrs[i]->dwBufferLength = dwDataSize;
    lpWaveHdrs[i]->dwFlags = 0L;
    lpWaveHdrs[i]->dwLoops = 0L;
    valid[i] = false;
    reset[i] = false;
  }

  // equation 2 from "Active Haptics Interfaces" paper: yn(0) = an
  for (int j=0; j<m; j++)
  {
    yreal[j] = 0.0;
    yimag[j] = 0.0;
  }

  // initialize values
  pos = 0;
  tog = 0;
  cnt = 0;
  attMag = 0.0;
  lastMoved = 0;
  vSum = 0.0;
  roll = false;

  // reset sound device
  if ((started) && (waveOutReset(hWaveOut)))
    Application->MessageBox("Error in Resetting Device", "Error");

  inCallBack = false;
  active = 1;
  firstHit = true;
  running = false;
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      This method generates a sound wave based on the sound properties
      of the material and contact interactions, and writes it to a sound
      device to be played.  The sound model is based on "The AHI: An Audio
      and Haptic Interface For Contact Interactions" by DiFilippo and Pai.

      \fn         void cSound::play(xVector3d iforce, double velocity)
*/
//===========================================================================
void cSound::play(cVector3d iforce, double velocity)
{

  if ((!active) || (pos > 5*dwDataSize))
    return;

  // calculate magnitude of the normal force
  normMag = iforce.length();

  if (firstHit)
  {
    normMag = 0.1;
    velocity = 0.01;
  }

  // if there is a new contact, start generating a sound
  if ((normMag >= 0.0001) && (prevMag < 0.0001))
  {

    // calculate a volume based on the velocity
    vol = 20 + 20000.0*velocity;
    if (vol > 250)
      vol = 250;
    waveOutSetVolume(hWaveOut, (vol << 8) + vol);

    attMag = 1.0;
    for (int j=0; j<m; j++)
    {
      yreal[j] = attMag*a[j];
      yimag[j] = 0.0;
    }
    reset[tog] = true;
    running = false;
    ready = 0;
    while (inCallBack)
      Sleep(1);
    if (waveOutReset(hWaveOut))
      Application->MessageBox("Error in Resetting Device", "Error");

    ready = 1;
    if (!firstHit)
      writeSound();

    firstHit = false;
    cnt = 0;
  }

  // exponentially attenuate impact force over time
  if (cnt > 0)
    audioForce = attMag*pow(0.85,cnt/100);
  else
    audioForce = 0.0;

  // convolve all modes of the sound profile with contact force
  double lpTemp = 0.0;

  prevMag = normMag;
  for (int j=0; j<m; j++)
  {
    tyreal[j] = treal[j]*yreal[j] - timag[j]*yimag[j];
    tyimag[j] = treal[j]*yimag[j] + timag[j]*yreal[j];
    yreal[j] = tyreal[j];
    yimag[j] = tyimag[j];
    yreal[j] += audioForce*a[j];

    if (j<n)
      lpTemp += yreal[j];
  }

  // if contact force has stopped, stop generating sound
  if ((normMag < 0.0001) && (cnt > soundSize))
  {
    lpWaveHdrs[tog]->lpData[pos] = 128.0;
    if (started)
    {
      vSum = 0.0;
      ready = 0;
      pos = 0;
      if (running)
      {
        while (inCallBack)
          Sleep(1);
        if (waveOutReset(hWaveOut))
          Application->MessageBox("Error in Resetting Device", "Error");
      }
      running = false;
      started = false;
    }
  }
  else
  {
    if (ready)
    {
      // otherwise write sound data to buffer
      lpWaveHdrs[tog]->lpData[pos] = lpTemp/scale + 128.0;
      valid[tog] = true;
      cnt++;
    }
  }

  if (ready)
    pos += 1;

  if ((pos > dwDataSize) && (!running) && (ready))
  {
    if ((cnt > soundSize) && (normMag > 0.00001))
    {
      vol = 128;
      waveOutSetVolume(hWaveOut, (vol << 8) + vol);
    }
    vSum = 0.0;
    writeSound();
  }
}


void CALLBACK cSound::writeCallback(HWAVEOUT hwo, UINT uMsg, cSound* dwInstance, DWORD param1, DWORD param2)
{
  inCallBack = true;
  dwInstance->writeSound();
  inCallBack = false;
}


void cSound::writeSound()
{

  if ((!active) || (!ready) || (!valid[tog]))
  {
    running = false;
    return;
  }
  running = true;

  // if buffer is full and has sound data, send it to sound device
  lpWaveHdrs[tog]->dwBufferLength = dwDataSize;
  lpWaveHdrs[tog]->dwFlags = 0L;
  lpWaveHdrs[tog]->dwLoops = 0L;
  vSum = 0.0;

  if (waveOutPrepareHeader(hWaveOut, lpWaveHdrs[tog], sizeof(WAVEHDR)))
    Application->MessageBox("Error in Preparing Handle", "Error");
  if (waveOutWrite(hWaveOut, lpWaveHdrs[tog], sizeof(WAVEHDR)))
    Application->MessageBox("Error in Writing Output", "Error");
  started = true;
  valid[tog] = false;
  tog = (tog + 1) % numBuffers;
  reset[tog] = false;
  pos = 0;
}


//===========================================================================
//  END OF FILE
//===========================================================================

