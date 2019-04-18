/**
 * @file Processes/Debug.cpp
 *
 * Implementation of class Debug.
 *
 * @author Martin Lötzsch
 */

#include <iostream>
#include "Debug.h"
#include "Tools/Debugging/Debugging.h"
#include "Platform/Time.h"
#include "Tools/Streams/TypeInfo.h"

Debug::Debug() :
  Process(theDebugReceiver, theDebugSender),
#ifndef CALIBRATION_TOOL
  debugHandler(theDebugReceiver, theDebugSender, MAX_PACKAGE_SEND_SIZE, 0),
#else
  theDebugReceiver(this),
  theDebugSender(this),
#endif
  theBanknoteClassifierReceiver(this),
  theBanknoteCorrectorReceiver(this),
  theBanknoteClassifierSender(this),
  theBanknoteCorrectorSender(this)
{
  theDebugSender.setSize(MAX_PACKAGE_SEND_SIZE - 2000);
  theDebugReceiver.setSize(MAX_PACKAGE_RECEIVE_SIZE - 2000);
  theBanknoteClassifierReceiver.setSize(20200000);
  theBanknoteClassifierSender.setSize(2800000);

  theBanknoteCorrectorReceiver.setSize(20200000);
  theBanknoteCorrectorSender.setSize(2800000);
  //if(SystemCall::getMode() == SystemCall::physicalRobot)
  //  setPriority(1);
}

bool Debug::main()
{
  DEBUG_RESPONSE_ONCE("automated requests:TypeInfo") OUTPUT(idTypeInfo, bin, TypeInfo(true));

  // Copying messages from debug queues from BanknoteClassifier and BanknoteCorrector
  switch(outQueueMode.behavior)
  {
    case QueueFillRequest::sendCollected:
    case QueueFillRequest::discardNew:
    case QueueFillRequest::discardAll:
      // Discard new messages
      theBanknoteClassifierReceiver.clear();
      theBanknoteCorrectorReceiver.clear();
      break;

    default:
      // Move the messages from other processes' debug queues to the outgoing queue
      if(!theBanknoteClassifierReceiver.isEmpty())
      {
        theBanknoteClassifierReceiver.moveAllMessages(theDebugSender);
      }
      if(!theBanknoteCorrectorReceiver.isEmpty())
      {
        theBanknoteCorrectorReceiver.moveAllMessages(theDebugSender);
      }
  }

  // Handing behaviour
  bool sendNow = false;
  bool sendToGUI = false;
  switch(outQueueMode.behavior)
  {
    case QueueFillRequest::sendAfter:
      if(Time::getCurrentSystemTime() > sendTime)
      {
        // Send messages that are in the queue (now matter how long it takes), but don't take new messages
        sendNow = true;
        outQueueMode.behavior = QueueFillRequest::sendCollected;
      }
      break;

    case QueueFillRequest::sendEvery:
      if(Time::getCurrentSystemTime() > sendTime)
      {
        // Send now (if the network is busy, this send time is effectively skipped)
        sendNow = true;

        // Compute time for next sending
        sendTime = Time::getCurrentSystemTime() + outQueueMode.timingMilliseconds;
      }
      break;

    case QueueFillRequest::collect:
    case QueueFillRequest::discardNew:
      // Don't send now
      break;

    case QueueFillRequest::discardAll:
      // Clear output queue
      theDebugSender.clear();
      break;

    case QueueFillRequest::sendImmediately:
    case QueueFillRequest::sendCollected:
    default:
      sendNow = true;
  }

  if(sendNow)
  {
    // Apply filter
    switch(outQueueMode.filter)
    {
      case QueueFillRequest::latestOnly:
        // Send only latest of each type
        theDebugSender.removeRepetitions();
        break;

      case QueueFillRequest::sendEverything:
        ; // Do nothing
    }

    // Send or save
    switch(outQueueMode.target)
    {
      case QueueFillRequest::writeToStick:
        if(!theDebugSender.isEmpty())
        {
          if(!fout)
          {
            fout = std::make_unique<OutBinaryFile>("logfile.log");
            theDebugSender.writeAppendableHeader(*fout);
          }
          // Append the outgoing queue to the file on the memory stick
          theDebugSender.append(*fout);
          theDebugSender.clear();
        }
        break;

      case QueueFillRequest::sendViaNetwork:
        sendToGUI = true;
        break;
    }
  }

  // Send messages to the processes
#ifdef CALIBRATION_TOOL
  theBanknoteClassifierSender.send(true);
  theBanknoteCorrectorSender.send(true);
  if(sendToGUI)
    theDebugSender.send();
#else
  theBanknoteClassifierSender.send(false);
  theBanknoteCorrectorSender.send(false);
  debugHandler.communicate(sendToGUI);
#endif

  return true;
}

void Debug::init()
{
  BH_TRACE_INIT("Debug");

  // read requests.dat
  InBinaryFile stream("requests.dat");
  if(stream.exists() && !stream.eof())
    stream >> theDebugReceiver;

  theDebugReceiver.handleAllMessages(*this);
  theDebugReceiver.clear();
}

bool Debug::handleMessage(InMessage& message)
{
  switch(message.getMessageID())
  {
    case idText: // loop back to GUI
      message >> theDebugSender;
      return true;

    // messages to BanknoteClassifier
    case idColorCalibration:
      message >> theBanknoteClassifierSender;
      return true;

    // messages to BanknoteCorrector
    //case idBanknoteCorrectorNet:
    //  message >> theBanknoteCorrectorSender;
    //  return true;

    // messages to Debug
    case idQueueFillRequest:
      // Read message queue settings and compute time when next to send (if in a timed mode)
      message.bin >> outQueueMode;
      sendTime = Time::getCurrentSystemTime() + outQueueMode.timingMilliseconds;
      fout = nullptr;
      return true;

    // messages to BanknoteClassifier and BanknoteCorrector
    case idModuleRequest:
    case idDebugDataChangeRequest:
    case idTypeInfo:
      message >> theBanknoteClassifierSender;
      message >> theBanknoteCorrectorSender;
      return true;

    // messages to all processes
    case idDebugRequest:
      message >> theBanknoteClassifierSender;
      message >> theBanknoteCorrectorSender;
      return Process::handleMessage(message);

    case idProcessBegin:
      message.bin >> processIdentifier;
      message.resetReadPosition();
      // no break

    default:
      if(processIdentifier == 'c')
        message >> theBanknoteCorrectorSender;
      else
        message >> theBanknoteClassifierSender;

      return true;
  }
}

#if defined CALIBRATION_TOOL || !defined NDEBUG
MAKE_PROCESS(Debug);
#endif
