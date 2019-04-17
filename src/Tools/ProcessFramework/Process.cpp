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

bool DebugSenderBase::terminating = false;

Process::Process(MessageQueue& in,MessageQueue& out)
: debugIn(in),
  debugOut(out)
{
  setGlobals();
  initialized = false;
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

bool Process::processMain()
{
  if (!initialized) {
    init();
    initialized = true;
  }
  
  handleAllMessages(debugIn);
  debugIn.clear();

  bool result = main();

  if(Global::getDebugRequestTable().pollCounter > 0 && --Global::getDebugRequestTable().pollCounter == 0)
      OUTPUT(idDebugResponse, text, "pollingFinished");
  
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

