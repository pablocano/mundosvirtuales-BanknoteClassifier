#pragma once

#include "RobotConsole.h"

class LocalRobot : public RobotConsole
{
  Receiver<MessageQueue> theDebugReceiver;
  DebugSender<MessageQueue> theDebugSender;

  Semaphore updateSignal; /**< A signal used for synchronizing main() and update(). */
  Semaphore updatedSignal; /**< A signal used for yielding processing time to main(). */

  unsigned timeStamp;
  unsigned lastTimeStampSent;
  unsigned timeDelay;


public:
  LocalRobot();

  /**
   * The function is called from the framework once in every frame
   */
  bool main() override;

  /**
   * The function must be called to exchange data with SimRobot.
   * It sends the motor commands to SimRobot and acquires new sensor data.
   */
  void update() override;
};
