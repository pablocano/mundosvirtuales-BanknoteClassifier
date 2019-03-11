//
//  Process.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 04-06-16.
//
//

#include "Process.h"
#include "Tools/Global.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Debugging/DebugRequest.h"

Process::Process(MessageQueue& in,MessageQueue& out)
: initialized(false),
  debugIn(in),
  debugOut(out)
{
}

void Process::setGlobals()
{
  Global::theDebugOut = &debugOut.out;
  Global::theSettings = &settings;
  Global::theDrawingManager = &drawingManager;
  Global::theDebugRequestTable = &debugRequestTable;
  Global::theDebugDataTable = &debugDataTable;
  Global::theTimingManager = &timingManager;
  
  Blackboard::setInstance(blackboard);
}

int Process::procesMain()
{
  if (!initialized) {
    init();
    initialized = true;
  }
  
  handleAllMessages(debugIn);
  debugIn.clear();

  int result = main();
  
  return result;
}

void Process::handleAllMessages(MessageQueue& messageQueue)
{
  debugIn.handleAllMessages(*this);
}

bool Process::handleMessage(InMessage& message)
{
  switch(message.getMessageID())
  {
    case idDebugRequest:
    {
      DebugRequest debugRequest;
      message.bin >> debugRequest;
      Global::getDebugRequestTable().addRequest(debugRequest);
      return true;
    }
    case idDebugDataChangeRequest:
    {
      Global::getDebugDataTable().processChangeRequest(message);
      return true;
    }
    default:
      return false;
  }
}

