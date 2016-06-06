/**
 * @file Platform/SimRobotQt/SystemCall.h
 *
 * Implementation of system calls and access to thread local storage.
 * Only for use inside the simulator.
 */

#pragma once

#include <cstdlib>

/**
 * static class for system calls
 * @attention the implementation is system specific!
 */
class SystemCall
{
public:
  
  /** returns the current system time in milliseconds*/
  static unsigned getCurrentSystemTime();
  
  /** returns the real system time in milliseconds (never the simulated one)*/
  static unsigned getRealSystemTime();
  
  /**
   * The function returns the thread cpu time of the calling thread in microseconds.
   * return thread cpu time of the calling thread
   */
  //static unsigned long long getCurrentThreadTime();
  
  /** returns the time since aTime*/
  static int getTimeSince(unsigned aTime)
  {
    return (int)(getCurrentSystemTime() - aTime);
  }
  
  /** returns the real time since aTime*/
  static int getRealTimeSince(unsigned aTime)
  {
    return (int)(getRealSystemTime() - aTime);
  }
  
  /** returns the name of the local machine*/
  static const char* getHostName();
  
  /** returns the first ip address of the local machine*/
  static const char* getHostAddr();
  
  /** Sleeps for some milliseconds.
   * \param ms The amout of milliseconds.
   */
  static void sleep(unsigned int ms);
};
