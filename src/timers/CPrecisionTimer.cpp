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
#include "CPrecisionTimer.h"
//---------------------------------------------------------------------------

// Initialize static variables
long cPrecisionTimer::m_numActiveTimers = 0;
long cPrecisionTimer::m_currentMinimumTimerInterval = 0;

// This was omitted from the VC6 SDK; it ensures that timers
// don't tick once you've called timeKillTimer()

// This flag appears to only work on XP, so we'll skip it for now...
// #define USE_TIME_KILL_SYNCHRONOUS

#ifdef USE_TIME_KILL_SYNCHRONOUS
#ifndef TIME_KILL_SYNCHRONOUS
#define TIME_KILL_SYNCHRONOUS 0x0100
#endif
#endif




//===========================================================================
/*!
      Constructor of cPrecisionTimer.

      \fn       cPrecisionTimer::cPrecisionTimer()
*/
//===========================================================================
cPrecisionTimer::cPrecisionTimer()
{
  // initialization
  defaults();
}


//===========================================================================
/*!
      Destructor of cPrecisionTimer.

      \fn         ~cPrecisionTimer::cPrecisionTimer()
*/
//===========================================================================
cPrecisionTimer::~cPrecisionTimer()
{
  // stop timer
  stop();

}


//===========================================================================
/*!
      Assign default values to variables; used for internal initialization
      only.

      \fn         void cPrecisionTimer::defaults()
*/
//===========================================================================
void cPrecisionTimer::defaults()
{
    m_averageCallbackInterval = 0;
    m_averageExecutionTime = 0;
    m_elapsedSinceLastCallback = 0;

    m_userData = 0;
    m_userCallback = 0;
    m_timer = 0;
    m_totalCallbacksCompleted = 0;
    m_totalCallbackTime = 0;
    m_previousCallbackStart = 0;
}


//===========================================================================
/*!
    Request that a callback function be queued for repeated callbacks.
    Calling this function replaces any timer previously associated with
    this object (i.e., you need two cPrecisionTimers if you want two
    separate callbacks).

    \fn       int cPrecisionTimer::set(int iInterval,  PRECISION_TIMER_CALLBACK*
              fpCallback, void* pUserData)

    \param    a_interval   The interval (in milliseconds) between successive
                          callbacks.

                          If iInterval is zero, the callback occurs
                          at the fastest rate supported by the system.

    \param    a_fpCallback  Pointer to a callback function.  The callback
                          should be a function of type PRECISION_TIMER_CALLBACK,
                          which is defined above.  Your function should look like :

                          void callback(void* user_data);

                          When your function gets called, you'll get back whatever
                          value you supplied as the user_data value in the set(...)
                          function.

    \param    a_ pUserData   This value is handed back to your callback function
                          every time it gets called.

                          One useful thing to supply here might be the timer itself,
                          so you can easily access timing statistics during your
                          callback.

    \return   Returns 0 if all goes well.  If the specified interval
              is not supported by the system, the return value is the
              minimum interval (in milliseconds) supported by the system.
              Any other error results in a return value of -1.

*/
//===========================================================================
int cPrecisionTimer::set(int a_interval,  PRECISION_TIMER_CALLBACK* a_fpCallback, void* a_pUserData)
{

    TIMECAPS tc;

    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
    {
        m_lastErrorMessage = "Could not get timer resolution.\n";
        return -1;
    }

    int min_interval = tc.wPeriodMin;
    int interval = a_interval;


    if ((a_interval == 0) || (a_interval < min_interval))
    {
        interval = min_interval;
    }

    // If we're the _first_ timer to be activated, or if this is a reduction in
    // the minimum timer interval, we need to pump up the system timer resolution.
    int timer_count = InterlockedIncrement(&m_numActiveTimers);

    if ( (timer_count == 1) || (interval < m_currentMinimumTimerInterval) )
    {
        // Erase the previous system timer period...
        if (timer_count > 1) timeEndPeriod(m_currentMinimumTimerInterval);

        // Set a new system timer period...
        m_currentMinimumTimerInterval = interval;
        MMRESULT result = timeBeginPeriod(interval);

        // Make sure it actually worked
        if (result != TIMERR_NOERROR)
        {
            m_lastErrorMessage = "Error setting system timer period.";
            return -1;
        }
    }

    // Okay, the timing subsystem is set up, let's set up _this_ timer

    // Reset timing variables
    defaults();

    m_userData = a_pUserData;
    m_userCallback = a_fpCallback;

    // Start the timer
    m_timer = timeSetEvent(interval,
                           0,
                           internal_timer_callback,
                           (unsigned long)(this),
                           TIME_PERIODIC | TIME_CALLBACK_FUNCTION
#ifdef USE_TIME_KILL_SYNCHRONOUS
                           | TIME_KILL_SYNCHRONOUS
#endif
                           );

    if (m_timer == 0)
    {
        m_lastErrorMessage = "Error starting timer.";
        return -1;
    }

    return 0;
}


//===========================================================================
/*!
      Stop an active timer.

      \fn       bool cPrecisionTimer::stop()
*/
//===========================================================================
bool cPrecisionTimer::stop()
{
    // Make sure there _is_ a timer to stop
    if (m_timer == 0) return (false);

    // Stop the timer
    timeKillEvent(m_timer);

    int active_timers = InterlockedDecrement(&m_numActiveTimers);

    // If we're the only timer around, shut down the multimedia timers
    if (active_timers == 0)
    {
        timeEndPeriod(m_currentMinimumTimerInterval);
        m_currentMinimumTimerInterval = 0;
    }

    return (true);
}


//===========================================================================
/*!
      Our global callback function, which triggers the user-defined callback
      function for a given cPrecisionTimer, and maintains timing stats

      \fn       void CALLBACK internal_timer_callback(UINT uTimerID,
                UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
*/
//===========================================================================
void CALLBACK internal_timer_callback(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    cPrecisionTimer* timer = (cPrecisionTimer*)(dwUser);

    // The current time, in seconds, since this timer started running
    double curtime;

    // If this is our first callback, initialize some variables
    if (timer->m_totalCallbacksCompleted == 0)
    {
        timer->m_tick_counter.Begin();
        timer->m_elapsedSinceLastCallback = 0;
    }

    // Otherwise update the "elapsed" time and the average timer interval
    else
    {
        curtime = timer->m_tick_counter.End();
        timer->m_elapsedSinceLastCallback = curtime - timer->m_previousCallbackStart;
        timer->m_averageCallbackInterval =  curtime / timer->m_totalCallbacksCompleted;
    }

    // Call the user-defined callback and time it
    double start_time = timer->m_tick_counter.GetTime();

    if (timer->m_userCallback)
    {
        timer->m_userCallback(timer->m_userData);
    }
    double end_time = timer->m_tick_counter.GetTime();

    // Compute some statistics about user callbacks
    timer->m_totalCallbackTime += end_time - start_time;
    timer->m_totalCallbacksCompleted++;
    timer->m_averageExecutionTime = timer->m_totalCallbackTime / timer->m_totalCallbacksCompleted;
}

