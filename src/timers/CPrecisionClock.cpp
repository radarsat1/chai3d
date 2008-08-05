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
    \version    1.1
    \date       01/2004
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "CPrecisionClock.h"
//---------------------------------------------------------------------------

//===========================================================================
/*!
	Constructor of cPrecisionClock. Clock is initialised to zero.

	\fn		cPrecisionClock::cPrecisionClock()
*/
//===========================================================================
cPrecisionClock::cPrecisionClock()
{
	// clock is currently off
	m_on = false;

	// test for high performance timer on the local machine. Some old compters
	// may not offer this feature
	QueryPerformanceFrequency (&m_freq);
	if (m_freq.QuadPart <= 0)
	{
		m_highres  = false;
	}
	else
	{
		m_highres  = true;
	}

	// initialise current time
	m_timeCurrent = 0;

	// initialise timeout
	m_timeoutPeriod = 0;
}


//===========================================================================
/*!
	Destructor of cPrecisionClock.

	\fn		~cPrecisionClock::cPrecisionClock()
*/
//===========================================================================
cPrecisionClock::~cPrecisionClock()
{
}


//===========================================================================
/*!
	Initialize the clock. Current time is set to zero.

	\fn		void cPrecisionClock::initialize()
*/
//===========================================================================
void cPrecisionClock::initialize()
{
	// initialise current time of timer
	m_timeCurrent = 0;
	m_timeStart = getCount();
}


//===========================================================================
/*!
	Start the clock from its current time value. To read the latest time
	from the clock, use getCurrentTime().

	\fn         long cPrecisionClock::start()
	\return     Return time of clock when timer was started.
*/
//===========================================================================
long cPrecisionClock::start()
{
	// store cpu time when timer was started
	m_timeStart = getCount() - m_timeCurrent;

	// timer is now on
	m_on = true;

	// return time when timer was started.
	return (m_timeCurrent);
}


//===========================================================================
/*!
	Stop the timer. To resume counting call start().

	\fn         long cPrecisionClock::stop()
	\return     Return time in \e microseconds.
*/
//===========================================================================
long cPrecisionClock::stop()
{
	// get current time
	long m_currentTime = getCurrentTime();

	// stop timer
	m_on = false;

	// return time when timer was stopped
	return (m_currentTime);
}


//===========================================================================
/*!
	Set the period in \e microseconds before timeout occures. Do not forget
	to set the timer on by calling start()

	\fn         void cPrecisionClock::setTimeoutPeriod(long a_timeoutPeriod);
	\param      a_timeoutPeriod  Timout period in \e microseconds.
*/
//===========================================================================
void cPrecisionClock::setTimeoutPeriod(long a_timeoutPeriod)
{
	m_timeoutPeriod = a_timeoutPeriod;
}


//===========================================================================
/*!
	Check if timer has expired its timeout period. if so return \b true.

	\fn         bool cPrecisionClock::timeoutOccured()
	\return     Return \b true if timeout occured, otherwize \b false.
*/
//===========================================================================
bool cPrecisionClock::timeoutOccured()
{
	// check if timeout has occured
	if (getCurrentTime() > m_timeoutPeriod)
	{
		return true;
	}
	else
	{
		return false;
	}
}


//===========================================================================
/*!
	Read the current time of timer. Result is returned in \e microseconds.

	\fn			long cPrecisionClock::getCurrentTime()
	\return		Return current time in \e microseconds
*/
//===========================================================================
long cPrecisionClock::getCurrentTime()
{
	// if timer is ON compute new time. Else return last current time.
	if (m_on)
	{
		long t_nowcpu = getCount();
		m_timeCurrent = t_nowcpu - m_timeStart;
	}

	// return result
	return (m_timeCurrent);
}


//===========================================================================
/*!
	Read the highresolution clock of the computer

	\fn			long cPrecisionClock::getCount()
	\return     Return cpu clock in \e microseconds.
*/
//===========================================================================
long cPrecisionClock::getCount()
{
	long t_count;

	// if high resolution available
	if (m_highres)
	{
		LARGE_INTEGER t_value;
		QueryPerformanceCounter (&t_value);
		t_count = (long)(1000000.0 * ((double)(t_value.QuadPart)) / ((double)(m_freq.QuadPart)));    
	}

	// else use windows clock (resolution: 10 ms)
	else
	{
		t_count = 1000*GetTickCount();
	}

	return (t_count);
}



//===========================================================================
/*!
	If all you want is something that tells you the time, this is your function...

	\fn			long cPrecisionClock::getCPUtime()
	\return     Return cpu clock in \e seconds.
*/
//===========================================================================

double cPrecisionClock::getCPUtime()
{

    if (m_highres)
    {
        __int64 curtime;
        QueryPerformanceCounter( (LARGE_INTEGER *)&curtime );

        return (double)curtime / (double)m_freq.QuadPart;
    }

    return ((double)(GetTickCount())) / 1000.0;
}

