/**
 * @file  Platform/SimRobotQt/SystemCall.cpp
 *
 * Implementation of system calls and access to thread local storage.
 * Only for use inside the simulator.
 */

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "SystemCall.h"

#  ifndef WINDOWS
#    ifdef LINUX
#      include <pthread.h>
#    endif
#    include <unistd.h>
#  else
#    include <Windows.h>
#  endif
#  include <cstring>

#ifndef WINDOWS
#  ifdef OSX
#    include <mach/mach_time.h>
#    include <sys/param.h>
#    include <sys/mount.h>
#  else
#    include <sys/sysinfo.h>
#    include <sys/statvfs.h>
#  endif
#  include <ctime>
#  include <netdb.h>
#  include <arpa/inet.h>
#endif

unsigned SystemCall::getCurrentSystemTime()
{
    return getRealSystemTime();
}

unsigned SystemCall::getRealSystemTime()
{
#ifdef WINDOWS
  unsigned time = timeGetTime();
#elif defined(OSX)
  static mach_timebase_info_data_t info = {0, 0};
  if(info.denom == 0)
    mach_timebase_info(&info);
  unsigned int time = unsigned(mach_absolute_time() * (info.numer / info.denom) / 1000000);
#else
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  unsigned int time = (unsigned int)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000l);
#endif
  static unsigned base = 0;
  if(!base)
    base = time - 10000; // avoid time == 0, because it is often used as a marker
  return time - base;
}

/*
unsigned long long SystemCall::getCurrentThreadTime()
{
#if defined(WINDOWS)
  static LARGE_INTEGER frequency = { 0 };
  if (frequency.QuadPart == 0)
  {
    QueryPerformanceFrequency(&frequency);
  }
  LARGE_INTEGER timeLL;
  QueryPerformanceCounter(&timeLL);
  return static_cast<unsigned long long>(timeLL.QuadPart * 1000000 / frequency.QuadPart);
#elif defined(OSX) // FIXME
  return (unsigned long long) getRealSystemTime() * 1000;
#else
  clockid_t cid;
  struct timespec ts;
  
  //pthread_getcpuclockid(pthread_self(), &cid);
  clock_gettime(cid, &ts);
  
  unsigned long long time = ts.tv_sec * 1000000ll + ts.tv_nsec / 1000;
  
  static unsigned long long base = 0;
  if(!base)
    base = time - 1000000;
  return time - base;
#endif
}
*/

const char* SystemCall::getHostName()
{
  static const char* hostname = 0;
  if(!hostname)
  {
    static char buf[100] = {0};
    gethostname(buf, sizeof(buf));
    hostname = buf;
  }
  return hostname;
}

const char* SystemCall::getHostAddr()
{
  static const char* hostaddr = 0;
  if(!hostaddr)
  {
    static char buf[100];
    hostent* hostAddr = (hostent*) gethostbyname(getHostName());
    if(hostAddr && *hostAddr->h_addr_list)
      strcpy(buf, inet_ntoa(*(in_addr*) *hostAddr->h_addr_list));
    else
      strcpy(buf, "127.0.0.1");
    hostaddr = buf;
  }
  return hostaddr;
}

void SystemCall::sleep(unsigned int ms)
{
#ifdef WINDOWS
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
}

std::string SystemCall::get_date(void)
{
   time_t now;
   char the_date[12];

   the_date[0] = '\0';

#ifndef WINDOWS

   now = time(NULL);

   if (now != -1)
   {
	   strftime(the_date, 12, "%d_%m_%Y", gmtime(&now));
   }

#endif // !WINDOWS

   return std::string(the_date);
}
