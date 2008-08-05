#ifndef __DHDOSDEP_H__
#define __DHDOSDEP_H__


#ifndef DHD_MAX_DEVICE
#ifdef WIN32
#pragma message("FD compilation message: DHD_MAX_DEVICE is redefined here")
#endif
#define DHD_MAX_DEVICE    8
#endif



/****************************************************************************
 *
 *  WINDOWS
 *
 ****************************************************************************/

#ifdef WIN32

#include <windows.h>
#include <winioctl.h>

#include "dhdusb.h"
#include "dhdpci.h"

typedef HANDLE   Lock;

typedef struct {
  MMRESULT ID;
  UINT     resolution;
  int      index;
  void     (*callback)(void);
} Timer;

inline unsigned long 
GetTimeMs() 
{ 
  return (unsigned long) GetTickCount(); 
}

inline unsigned long 
GetTimeUs ()
{
  static LARGE_INTEGER freq;
  static BOOL          hr    = QueryPerformanceFrequency (&freq);

  LARGE_INTEGER val;

  if (hr) {
    QueryPerformanceCounter (&val);
    return (unsigned long)(1000000.0f*(float)(val.QuadPart)/(freq.QuadPart)); 
  }
  else {
    return (unsigned long) (1000*GetTickCount());
  }
}

inline Lock
LockNew () {
  return CreateMutex(NULL, false, NULL);
}

inline void
LockDelete (Lock lock)
{
  CloseHandle (lock);
}

inline bool
LockAcquire (Lock lock)
{
  return (WaitForSingleObject (lock, INFINITE) != WAIT_FAILED);
}

inline void
LockRelease (Lock lock)
{
  ReleaseMutex (lock);
}

inline bool 
LockAcquireBefore (Lock mutex, 
                   int  timeout) 
{
  int res = WaitForSingleObject(mutex, timeout);
  return ((res != WAIT_TIMEOUT) && (res != WAIT_FAILED));
}

#endif


/****************************************************************************
 *
 *  LINUX
 *
 ****************************************************************************/

#ifdef LINUX

#include <sys/time.h>
#include <sys/io.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <usb.h>

#include "pci.h"

typedef pthread_mutex_t* Lock;

typedef struct {
  int    ID;
  int    resolution;
  int    index;
  void   (*callback)(void);
} Timer;

inline unsigned long 
GetTimeUs ()
{
  struct timeval cTime;		// cur time
  gettimeofday (&cTime, NULL);
  return ((unsigned long)(1000000*(cTime.tv_sec) + cTime.tv_usec));
} 

inline unsigned long 
GetTimeMs ()
{
  struct timeval cTime;		// cur time
  gettimeofday (&cTime, NULL);
  return (unsigned long)(((cTime.tv_sec)*1000)+((cTime.tv_usec)/1000));
} 

inline Lock 
LockNew ()
{
  pthread_mutex_t* lock;	
  pthread_mutexattr_t *mattr = (pthread_mutexattr_t *) malloc (sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init (mattr);
  pthread_mutexattr_settype (mattr, PTHREAD_MUTEX_RECURSIVE_NP);
  lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
  if(lock == NULL) return NULL;
  if(pthread_mutex_init (lock, mattr)) return NULL;
  return lock;
}

inline void 
LockDelete (Lock lock)
{
  pthread_mutex_destroy (lock);
  free(lock);
}

inline bool 
LockAcquire (Lock lock) 
{
  return (pthread_mutex_lock (lock) == 0);
}

inline bool 
LockAcquireBefore (Lock lock, 
                   int  timeout)
{
  unsigned long endTime;

  if(pthread_mutex_trylock (lock) == 0) return true;
  endTime = GetTimeMs() + timeout;
  do {
    if(GetTimeMs() >= endTime) return false;
    usleep(10 * 1000); //us
  } while(pthread_mutex_trylock (lock) != 0);	
  return true;
}

inline void
LockRelease (Lock lock) 
{
  pthread_mutex_unlock (lock);
}

#endif


/****************************************************************************
 *
 *  APPLE
 *
 ****************************************************************************/

#ifdef APPLE

#include <mach/mach_time.h>
#include <mach/kern_return.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <usb.h>

// #include "AccesDIO24Lib.h"

typedef pthread_mutex_t* Lock;

typedef struct {
  int    ID;
  int    resolution;
  int    index;
  void   (*callback)(void);
} Timer;

inline double 
GetTimeNs()
{
  static double _timeBaseFactor = 0.0;

  if(!_timeBaseFactor) {
    mach_timebase_info_data_t		info;
    if(mach_timebase_info(&info) == KERN_SUCCESS)
      _timeBaseFactor = (double) info.numer / (double) info.denom;
  }

  return (double) mach_absolute_time() * _timeBaseFactor;
}


inline unsigned long
GetTimeUs () 
{
  return ((unsigned long) (GetTimeNs() / (double) 1000.0));
}

inline unsigned long 
GetTimeMs ()
{
  return ((unsigned long) (GetTimeNs() / (double) 1000000.0));
}

inline Lock 
LockNew()
{
  pthread_mutex_t* lock;	
  pthread_mutexattr_t *mattr = (pthread_mutexattr_t *) malloc (sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init (mattr);
  pthread_mutexattr_settype (mattr, PTHREAD_MUTEX_RECURSIVE_NP);
  lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
  if(lock == NULL) return NULL;
  if(pthread_mutex_init (lock, NULL)) return NULL;
  return lock;
}

inline void 
LockDelete (Lock lock)
{
  pthread_mutex_destroy (lock);
  free(lock);
}

inline bool 
LockAcquire (Lock lock) {
  return (pthread_mutex_lock (lock) == 0);
}

inline bool 
LockAcquireBefore (Lock lock, 
                   int  timeout)
{
  struct timeval endTime;

  if(pthread_mutex_trylock(lock) == 0) return true;
  endTime.tv_usec = GetTimeUs() + timeout*1000000;
  do {
    if(GetTimeUs() >= (uint)(endTime.tv_usec)) return false;
    usleep(10 * 1000); //us
  } while(pthread_mutex_trylock (lock) != 0);	
  return true;
}

inline void
LockRelease (Lock lock) 
{
  pthread_mutex_unlock (lock);
}

#endif



/* PROTOS */
int           TimerSet          (int interval, void  (*callback)(void), Timer *timer = NULL);
int           TimerStart        (Timer *timer);
int           TimerStop         (Timer *timer);

#endif	
