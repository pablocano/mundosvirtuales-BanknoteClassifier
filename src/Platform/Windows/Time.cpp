#include "Platform/Time.h"
#include "Platform/BCAssert.h"

#ifdef CALIBRATION_TOOL
#include "Utils/CalibratorTool/CalibratorToolCtrl.h"
#endif

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
