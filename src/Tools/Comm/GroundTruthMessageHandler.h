#pragma once

#include "Tools/Messages/MessageQueue.h"
#include "Tools/Comm/UdpComm.h"
#include "MessageIDs.h"
#include <fstream>

#define SEND_MESSAGE(type,expression) \
{ GroundTruthMessageHandler::getOutQueue() << &expression;\
GroundTruthMessageHandler::getOutQueue().finishMessage(type); }


class GroundTruthMessageHandler
{
public:
  
  static GroundTruthMessageHandler *theInstance;
  
  GroundTruthMessageHandler();
  
  ~GroundTruthMessageHandler();
  
  void send();
  
  static MessageQueue& getOutQueue();
  
  MessageQueue queue;
  
  UdpComm socket;
};
