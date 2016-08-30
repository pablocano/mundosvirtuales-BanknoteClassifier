//
//  Process.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 04-06-16.
//
//

#include "Process.h"
#include "Tools/Global.h"

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
  
  Blackboard::setInstance(blackboard);
}

int Process::procesMain()
{
  if (!initialized) {
    init();
    initialized = true;
  }
  
  return main();
}
