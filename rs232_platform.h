#ifndef RS232_PLATFORM_H_INCLUDED
#define RS232_PLATFORM_H_INCLUDED

#if !defined(WINDOWS_BUILD)
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) || defined(__CYGWIN__)
#define WINDOWS_BUILD 1
#else
#define WINDOWS_BUILD 0
#endif
#endif

#include <time.h>

#if WINDOWS_BUILD
#include <windows.h>

#ifndef _SSIZE_T_DEFINED
#ifdef  _WIN64
typedef __int64 ssize_t;
#else
typedef _W64 int ssize_t;
#endif /* defined(_WIN64) */
#define _SSIZE_T_DEFINED
#endif /* !_SSIZE_T_DEFINED */

/* You must define RS232_ADD_EXPORTS _only_ when building the DLL. */
#ifdef RS232_ADD_EXPORTS
  #define RS232_ADDAPI __declspec(dllexport)
#else
  #define RS232_ADDAPI __declspec(dllimport)
#endif

#define RS232_ADDCALL __cdecl

typedef HANDLE RS232_FD;

#define RS232_INVALID_FD    INVALID_HANDLE_VALUE

#define msleep(msecs) Sleep(msecs)
#else
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/file.h>
#include <errno.h>

#define RS232_ADDAPI
#define RS232_ADDCALL

typedef int RS232_FD;

#define RS232_INVALID_FD    -1

#define msleep(msecs) usleep(msecs*1000)
#endif

static inline void timerspecsub(const struct timespec *a, const struct timespec *b, struct timespec *result)
{

  result->tv_sec  = a->tv_sec  - b->tv_sec;
  result->tv_nsec = a->tv_nsec - b->tv_nsec;
  if (result->tv_nsec < 0)
  {
    --result->tv_sec;
    result->tv_nsec += 1000000000L;
  }
}

static inline long timespecsub_to_msec(const struct timespec *ts)
{

  return (long)(ts->tv_sec * 1000L + ts->tv_nsec / 1000000L);
}

#endif /* RS232_PLATFORM_H_INCLUDED */
