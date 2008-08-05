///////////////////////////////////////////////////////////////////////////////
//
//  (C) 2002 - Force Dimension LLC
//  All Rights Reserved.
//
//  TimeGuard (tm) feature
//  Header file  
//
///////////////////////////////////////////////////////////////////////////////



#ifndef __TIMEGUARD_H__
#define __TIMEGUARD_H__


#include "dhd.h"
#include "dhdOSDep.h"


// timer constants
#define DHD_MAX_TIMER                       6
#define DHD_TIMEGUARD_POS                   0
#define DHD_TIMEGUARD_FORCE                 1
#define DHD_TIMEGUARD_ANG                   2
#define DHD_TIMEGUARD_TORQUE                3
#define DHD_TIMEGUARD_STATUS                4
#define DHD_TIMEGUARD_BUTTONS               5

#define DHD_TIMEGUARD_DEFAULT_TIMEOUT     250
#define DHD_TIMEGUARD_BUTTONS_TIMEOUT    2000



class dhdTimeGuard
{
private:
  bool  on;
  ulong timer[DHD_MAX_TIMER];
  ulong timeout[DHD_MAX_TIMER];

public:

  dhdTimeGuard            ();
  ~dhdTimeGuard           ();

  int   enableTimeGuard   ();
  int   enableTimeGuard   (int us);
  int   disableTimeGuard  ();

  ulong getSystemCounter  ();
  ulong getTimer          (int index);
  bool  getTimeState      ();

  int   setTimeout        (long timeout);

  bool  queryTimer        (int index);
  ulong resetTimer        (int index);
};


#endif
