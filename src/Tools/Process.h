#pragma once
#include "Tools/Messages/MessageQueue.h"
#include "Tools/ModuleManager/Blackboard.h"
#include "Tools/Settings.h"

#define DEBUGGING \
MessageQueue theDebugIn; \
MessageQueue theDebugOut;

#define INIT_DEBUGGING \
Process(theDebugIn,theDebugOut)

class Process {
  
public:
  
  Process(MessageQueue& in,MessageQueue& out);
  
  int procesMain();
  
  void setGlobals();
  
protected:
  
  virtual int main() = 0;
  
  virtual void init() {};
  
private:
  bool initialized;
  Settings settings;
  Blackboard blackboard;
  
  MessageQueue& debugIn;
  MessageQueue& debugOut;
  
};