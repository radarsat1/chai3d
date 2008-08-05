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
#include <vcl.h>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#pragma hdrstop
#include "CSound.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

// Change these values if your computer is slow and the sound is jerky:

// Sample rate; decrease (perhaps to 22000 or so) if sound is jerky
#define SAMPLE_RATE 44100

// Buffer size; decrease (perhaps to 128 or so) if there is a perceptable pause
// between impact and initiation of sound
#define BUFFER_SIZE 1024

// Number of samples generated per impact; decrease in proportion to SAMPLE_RATE
// if you decrease that; increase (decrease) if sound fades too quickly (too slowly)
#define MAX_SAMPLES_PER_IMPACT 200000


#define BLOCK_SIZE 8192
#define BLOCK_COUNT 20

// audio function prototypes
static void CALLBACK waveOutProc(HWAVEOUT, UINT, DWORD, DWORD, DWORD);
static WAVEHDR* allocateBlocks(int size, int count);
static void freeBlocks(WAVEHDR* blockArray);
static void writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size);

// global variables
// counter incremented each time sound object is created to give each unique id
int g_ids = 0;
// the id of the object currently generating sound (only one is active at a time
// in this implementation)
int g_active = 0;

// module level variables for sound card output
static CRITICAL_SECTION waveCriticalSection;
static WAVEHDR* waveBlocks;
static volatile int waveFreeBlockCount;
static int waveCurrentBlock;
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
  id = g_ids++;
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      Destructor of cSound.

      \fn         void cSound::~cSound()
*/
//===========================================================================
cSound::~cSound()
{
  int i;
  while(waveFreeBlockCount < BLOCK_COUNT)
  Sleep(10);

  // unprepare any blocks that are still prepared
  for(i = 0; i < waveFreeBlockCount; i++)
  if(waveBlocks[i].dwFlags & WHDR_PREPARED)
  waveOutUnprepareHeader(hWaveOut, &waveBlocks[i], sizeof(WAVEHDR));
  DeleteCriticalSection(&waveCriticalSection);
  freeBlocks(waveBlocks);
  waveOutClose(hWaveOut);
  return;
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
void cSound::setParams(sounds a_choice)
{
  // initialise the module variables
  waveBlocks = allocateBlocks(BLOCK_SIZE, BLOCK_COUNT);
  waveFreeBlockCount = BLOCK_COUNT;
  waveCurrentBlock= 0;
  InitializeCriticalSection(&waveCriticalSection);

  // set up the WAVEFORMATEX structure.
  wfx.nSamplesPerSec = SAMPLE_RATE; // sample rate
  wfx.wBitsPerSample = 8; // sample size
  wfx.nChannels = 1; // channels
  wfx.cbSize = 0; // size of _extra_ info
  wfx.wFormatTag = WAVE_FORMAT_PCM;
  wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) >> 3;
  wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

  // try to open the default wave device.
  if(waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD_PTR)waveOutProc,
     (DWORD_PTR)&waveFreeBlockCount, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
  {
    ExitProcess(1);
  }

  // initialize buffer for writing to sound card
  dwDataSize = BUFFER_SIZE;
  buffer = new char[dwDataSize];
  for (int i=0; i<dwDataSize; i++)
    buffer[i] = 128;

  // initialize duration of impact sound
  soundSize = MAX_SAMPLES_PER_IMPACT;

  // select .sy data file based on input sound option
  FILE*  dfile;
  switch (a_choice)
  {
    case BELL:
      dfile = fopen("resources\\sounds\\handbell.sy", "r");
      scale = 1.0;
      break;
    case TEAPOT:
      dfile = fopen("resources\\sounds\\teapot.sy", "r");
      scale = 6.0;
      break;
  }

  // read in the number of modes, amplitudes, and decay coefficients
  char file_buffer[32];
  fgets(file_buffer, 256, dfile);
  fgets(file_buffer, 256, dfile);
  n = atoi(file_buffer);

  a.clear();
  f.clear();
  d.clear();

  // ignore header
  for (int i=0; i<11; i++)
    fgets(file_buffer, 256, dfile);

  // read in the modes
  for (int i=0; i<n; i++)
  {
    fgets(file_buffer, 256, dfile);
    f.push_back(atof(file_buffer));
  }

  // read in the decay coefficients
  fgets(file_buffer, 256, dfile);
  for (int i=0; i<n; i++)
  {
    fgets(file_buffer, 256, dfile);
    d.push_back(atof(file_buffer));
  }

  // read in the amplitudes
  fgets(file_buffer, 256, dfile);
  for (int i=0; i<n; i++)
  {
    fgets(file_buffer, 256, dfile);
    a.push_back(atof(file_buffer));
  }
  fclose(dfile);

  // allocate space for sound calculations
  treal = new double[n];
  timag = new double[n];
  yreal = new double[n];
  yimag = new double[n];
  tyreal = new double[n];
  tyimag = new double[n];

  // precompute real and imaginary parts of e^(i*omegan/Fs)
  for (int j=0; j<n; j++)
  {
    treal[j] = exp(-d[j]/wfx.nSamplesPerSec)*cos(2*3.14159*f[j]/wfx.nSamplesPerSec);
    timag[j] = exp(-d[j]/wfx.nSamplesPerSec)*sin(2*3.14159*f[j]/wfx.nSamplesPerSec);
  }

  // equation 2 from "Active Haptics Interfaces" paper: yn(0) = an
  for (int j=0; j<n; j++)
  {
    yreal[j] = 0.0;
    yimag[j] = 0.0;
  }

  // initialize values
  pos = 0;
  cnt = 0;
  state = 0;
  prevMag = 0.0;
}


//===========================================================================
//  - PUBLIC METHOD -
/*!
      This method generates a sound wave based on the sound properties
      of the material and contact interactions, and writes it to a sound
      device to be played.  The sound model is based on "The AHI: An Audio
      and Haptic Interface For Contact Interactions" by DiFilippo and Pai.

      \fn         void cSound::play(cVector3d a_force)
*/
//===========================================================================
void cSound::play(cVector3d a_force)
{
  // calculate magnitude of the normal force
  double normMag = a_force.length();

  // if there is a new contact, start generating a sound
  if ((normMag >= 0.0001) && (prevMag < 0.0001))
  {

    // equation 2 from "Active Haptics Interfaces" paper: yn(0) = an
    for (int j=0; j<n; j++)
    {
      yreal[j] = a[j];
      yimag[j] = 0.0;
    }

    // reset the device
    if (waveOutReset(hWaveOut))
      Application->MessageBox("Error in Resetting Device", "Error");

    // this sound object is now the active one
    g_active = id;

    // start writing at beginning of data buffer
    pos = 0;

    // this sound object is writing sound data
    state = 1;

    // zero samples have been written so far for this impact
    cnt = 0;
  }

  // if we have generated enough samples since the last impact, we can stop
  if ((normMag < 0.0001) && (cnt > soundSize))
  {
    if (waveOutReset(hWaveOut))
      Application->MessageBox("Error in Resetting Device", "Error");
    pos = 0;
    state = 0;
    cnt = 0;
  }

  // if this object is active, calculate sound data
  if ((state == 1) && (g_active == id))
  {

    // convolve the haptic normal force with the sound modes; see the Pai papers
    // for an explanation of this calculation
    double lpTemp = 0.0;
    for (int j=0; j<n; j++)
    {
      tyreal[j] = treal[j]*yreal[j] - timag[j]*yimag[j];
      tyimag[j] = treal[j]*yimag[j] + timag[j]*yreal[j];
      yreal[j] = tyreal[j];
      yimag[j] = tyimag[j];
      yreal[j] += normMag*a[j];
      lpTemp += yreal[j];
    }

    // when the buffer is full, call writeAudio to write it to a data block
    // for the sound card
    buffer[pos++] = (int)(lpTemp/scale) + 128;
    if (pos == dwDataSize-1)
    {
      pos = 0;
      writeAudio(hWaveOut, buffer, dwDataSize);
    }
    cnt++;
  }

  // set the previous magnitude for the next iteration
  prevMag = normMag;

  return;
}


// This function writes data to data blocks and prepares them for the
// sound card, handling a multiple-buffer scheme to provide continuous
// sound data.  See the Overton tutorial referenced at the top of this
// file.
void writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size)
{
  WAVEHDR* current;
  int remain;
  current = &waveBlocks[waveCurrentBlock];

  while(size > 0)
  {
    // first make sure the header we're going to use is unprepared
    if(current->dwFlags & WHDR_PREPARED)
      waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));

    if(size < (int)(BLOCK_SIZE - current->dwUser))
    {
      memcpy(current->lpData + current->dwUser, data, size);
      current->dwUser += size;
      break;
    }
    remain = BLOCK_SIZE - current->dwUser;
    memcpy(current->lpData + current->dwUser, data, remain);
    size -= remain;
    data += remain;
    current->dwBufferLength = BLOCK_SIZE;
    waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));
    EnterCriticalSection(&waveCriticalSection);
    waveFreeBlockCount--;
    LeaveCriticalSection(&waveCriticalSection);

    // wait for a block to become free
    while(!waveFreeBlockCount)
      Sleep(10);

    // point to the next block
    waveCurrentBlock++;
    waveCurrentBlock %= BLOCK_COUNT;
    current = &waveBlocks[waveCurrentBlock];
    current->dwUser = 0;
  }
}


// This function allocates data blocks to be used for sending data to the sound
// card.
WAVEHDR* allocateBlocks(int size, int count)
{
  char* buffer;
  int i;
  WAVEHDR* blocks;
  DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;

  // allocate memory for the entire set in one go
  buffer = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBufferSize);

  // and set up the pointers to each bit
  blocks = (WAVEHDR*)buffer;
  buffer += sizeof(WAVEHDR) * count;

  for(i = 0; i < count; i++)
  {
    blocks[i].dwBufferLength = size;
    blocks[i].lpData = buffer;
    buffer += size;
  }
  return blocks;
}


// This function frees the data blocks that were used for sendind data to the
// sound card.
void freeBlocks(WAVEHDR* blockArray)
{
  // and this is why allocateBlocks works the way it does
  HeapFree(GetProcessHeap(), 0, blockArray);
}


// This is the callback function that the system calls each time the sound
// card finishes playing a data block.
static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance,
    DWORD dwParam1, DWORD dwParam2)
{
  // pointer to free block counter
  int* freeBlockCounter = (int*)dwInstance;

  // ignore calls that occur due to openining and closing the device.
  if(uMsg != WOM_DONE) return;
  EnterCriticalSection(&waveCriticalSection);
  (*freeBlockCounter)++;
  LeaveCriticalSection(&waveCriticalSection);
}


//===========================================================================
//  END OF FILE
//===========================================================================

