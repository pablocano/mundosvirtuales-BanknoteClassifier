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
  Global::theDebugOut = &debugOut;
  Global::theSettings = &settings;
  Global::theDrawingManager = &drawingManager;
  Global::theDebugRequestTable = &debugRequestTable;
  
  Blackboard::setInstance(blackboard);
}

int Process::procesMain()
{
  if (!initialized) {
    init();
    initialized = true;
  }
  
#ifndef RELEASE
  debugIn.handleAllMessages(*this);
  debugIn.clear();
#endif
  
  int result = main();
  
#ifndef RELEASE
  if(Global::getDebugRequestTable().poll)
  {
    if(Global::getDebugRequestTable().pollCounter++ > 10)
    {
      Global::getDebugRequestTable().poll = false;
      OUTPUT(idDebugResponse, "pollingFinished");
    }
  }
#endif
  
  return result;
}

bool Process::handleMessage(MessageQueue& message)
{
  switch(message.getMessageID())
  {
    case idDebugRequest:
    {
      DebugRequest debugRequest;
      message >> debugRequest;
      Global::getDebugRequestTable().addRequest(debugRequest);
      return true;
    }
    default:
      return false;
  }
}

