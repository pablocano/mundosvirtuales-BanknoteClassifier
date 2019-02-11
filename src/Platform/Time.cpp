#include "Platform/Time.h"
#include "Platform/BCAssert.h"

#ifndef WINDOWS
#include <ctime>
#include <pthread.h>

unsigned Time::base = 0;
unsigned long long Time::threadTimebase = 0;

unsigned Time::getCurrentSystemTime()
{
    return getRealSystemTime();
}

unsigned Time::getRealSystemTime()
{
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts); // NTP might change CLOCK_REALTIME on desktop systems
  const unsigned int time = (unsigned int)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000l);
  if(!base)
    base = time - 100000; // avoid time == 0, because it is often used as a marker
  return time - base;
}

unsigned long long Time::getCurrentThreadTime()
{
  clockid_t cid;
  timespec ts;

  VERIFY(pthread_getcpuclockid(pthread_self(), &cid) == 0);
  VERIFY(clock_gettime(cid, &ts) == 0);

  const unsigned long long time = ts.tv_sec * 1000000ll + ts.tv_nsec / 1000;
  if(!threadTimebase)
    threadTimebase = time - 1000000ll;
  return time - threadTimebase;
}

#else

#include <Windows.h>

unsigned Time::base = 0;
unsigned long long Time::threadTimebase = 0;

unsigned Time::getCurrentSystemTime()
{
    return getRealSystemTime();
}

unsigned Time::getRealSystemTime()
{
  const unsigned time = timeGetTime();
  if(!base)
    base = time - 100000; // avoid time == 0, because it is often used as a marker
  return time - base;
}

unsigned long long Time::getCurrentThreadTime()
{
  static LARGE_INTEGER frequency = { 0 };
  if(frequency.QuadPart == 0)
  {
    QueryPerformanceFrequency(&frequency);
  }
  LARGE_INTEGER timeLL;
  QueryPerformanceCounter(&timeLL);

  const unsigned long long time = static_cast<unsigned long long>(timeLL.QuadPart * 1000000ll / frequency.QuadPart);
  if(!threadTimebase)
    threadTimebase = time - 1000000ll;
  return time - threadTimebase;
}
#endif
