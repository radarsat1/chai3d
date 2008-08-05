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
    \version    1.1
    \date       01/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CPrecisionTimerH
#define CPrecisionTimerH
//---------------------------------------------------------------------------
#include <windows.h>
#include <mmsystem.h>
#include "celapsed.h"
#include <stdio.h>
#include <string>
//---------------------------------------------------------------------------
using std::string;
//---------------------------------------------------------------------------
#ifdef _MSVC
#include <conio.h>
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*
    This is the way user-defined callbacks will look... for example, the
    callback function that I give to a cPrecisionTimer might look like :

    void my_favorite_callback(void* my_favorite_data);
*/
//---------------------------------------------------------------------------
typedef void(PRECISION_TIMER_CALLBACK)(void* pUserData);


//---------------------------------------------------------------------------
/*
    This is the global function that the multimedia timer will call to trigger
    the user callback.  Users don't need to worry about this; it's just used
    internally.  He'll be a 'friend' of this class, so he can access private
    data (like the user's callback function).
*/
//---------------------------------------------------------------------------
void CALLBACK internal_timer_callback(UINT uTimerID, UINT uMsg, DWORD dwUser,
                                      DWORD dw1, DWORD dw2);


//===========================================================================
/*!
      \class    cPrecisionTimer
      \brief    The cPrecisionTimer class manages high-rate callbacks using win32
                multimedia timers.  Rates up to 1kHz should be supported.

                This class also maintains statistics about how long your callbacks
                are taking to execute, how much time has elapsed since the previous
                callback, etc.
*/
//===========================================================================
class cPrecisionTimer
{
  public:

    // CONSTRUCTOR & DESTRUCTOR:
    //! Constructor of cPrecisionTimer.
    cPrecisionTimer();

    //! Destructor of cPrecisionTimer.  Stops the timer implicitly.
    ~cPrecisionTimer();

    // METHODS:
    //! Request that a callback function be queued for repeated callbacks.
    int set(int a_interval, PRECISION_TIMER_CALLBACK* a_fpCallback, void* a_pUserData = 0);

    //! Stop current timer.
    bool stop();

    // MEMBERS:
    //! This is the average time (in seconds) that your callback function has required for execution
    double m_averageExecutionTime;

    // This is the average time (in seconds) that has elapsed per callback,
    // including the inter-callback interval and the user execution time.
    //
    // In an ideal world, this should be equal to the interval you specified
    // in "iInterval".
    double m_averageCallbackInterval;

    // During a callback, this is the time (in seconds) that elapsed between
    // the beginning of the previous callback and the beginning of the current
    // callback.  This is a useful "delta t" for physics and simulation.
    double m_elapsedSinceLastCallback;

    // Total number of callbacks that have taken place
    double m_totalCallbacksCompleted;

    // Total time taken for all _user_ callbacks
    double m_totalCallbackTime;

    // Time (seconds since callback startup) at which the most recent (or current)
    // callback occurred
    double m_previousCallbackStart;

    // The number of cPrecisionTimer's currently processing
    // callbacks
    static long m_numActiveTimers;

    // The last value passed to timeBeginPeriod().  We need to shrink
    // this if future timers need finer granularity.
    static long m_currentMinimumTimerInterval;

    void* m_userData;

    // Takes care of accurately counting seconds
    CElapsed m_tick_counter;

    // The callback function that the user wants to execute
    PRECISION_TIMER_CALLBACK* m_userCallback;

  private:
    //! Last error message.
    string m_lastErrorMessage;

    // handle to timer
    MMRESULT m_timer;

    // Assign default values to variables
    void defaults();

  // The multimedia timer will need a global function as callback, but we
  // make him a 'friend' so he can access private data.  He'll need to
  // call the user's callback function and modify the timing statistcs.
  friend void CALLBACK m_internal_timer_callback(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

