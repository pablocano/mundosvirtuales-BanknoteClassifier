/**
 * @file Controller/LocalRobot.cpp
 *
 * Implementation of LocalRobot.
 *
 * @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
 * @author <A href="mailto:kspiess@tzi.de">Kai Spiess</A>
 */

#include "LocalRobot.h"
#include "Representations/FrameInfo.h"
#include "Utils/CalibratorTool/ConsoleController.h"
#include "Platform/Time.h"

LocalRobot::LocalRobot() :
  RobotConsole(theDebugReceiver, theDebugSender),
  theDebugReceiver(this),
  theDebugSender(this),
  updatedSignal(1),
  timeStamp(0),
  lastTimeStampSent(0),
  timeDelay(20)
{
  addViews();
}

bool LocalRobot::main()
{
  if(updateSignal.tryWait())
  {
    {
      // Only one thread can access *this now.
      SYNC;
      if(timeStamp != lastTimeStampSent)
      {
        debugOut.out.bin << 'e';
        debugOut.out.finishMessage(idProcessBegin);
        FrameInfo frameInfo;
        frameInfo.time = timeStamp;
        debugOut.out.bin << frameInfo;
        debugOut.out.finishMessage(idFrameInfo);
        debugOut.out.bin << 'e';
        debugOut.out.finishMessage(idProcessFinished);
        lastTimeStampSent = timeStamp;
      }

      theDebugSender.send(true);
    }

    updatedSignal.post();
  }
  return true;
}

void LocalRobot::update()
{
  RobotConsole::update();

  updatedSignal.wait();

  // Only one thread can access *this now.
  {
    SYNC;
    unsigned now = Time::getCurrentSystemTime();

    if(now - timeStamp > timeDelay)
      timeStamp = now;

    QString statusText;

    if(pollingFor)
    {
      statusText += statusText != "" ? ", polling for " : "polling for ";
      statusText += pollingFor;
    }

    if(statusText.size() > 0)
      ((ConsoleController*)ConsoleController::controller)->printStatusText((robotName + ": " + statusText).toUtf8());
  }

  updateSignal.post();
  trigger(); // invoke a call of main()
}
