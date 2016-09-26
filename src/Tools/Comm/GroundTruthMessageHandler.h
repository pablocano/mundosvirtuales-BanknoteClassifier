#pragma once

#include "Tools/Messages/MessageQueue.h"
#include "Tools/Comm/UdpComm.h"
#include "MessageIDs.h"
#include <fstream>

#define GROUND_TRUTH_COMM \
MessageQueue theCommReceiver; \
MessageQueue theCommSender; \
GroundTruthMessageHandler theGroundTruthCommHandler

#define INIT_GROUND_TRUTH_COMM \
theGroundTruthCommHandler(theCommReceiver, theCommSender)

#define START_GROUND_TRUTH_COMM \
std::string bcastAddr = UdpComm::getWifiBroadcastAddress(); \
theGroundTruthCommHandler.start(Global::getSettings()->teamPort, bcastAddr.c_str())

#define RECEIVE_GROUND_TRUTH_COMM \
(void) theGroundTruthCommHandler.receive()

#define SEND_GROUND_TRUTH_COMM \
theGroundTruthCommHandler.send()


class GroundTruthMessageHandler
{
public:
  
  static GroundTruthMessageHandler *theInstance;
  
  GroundTruthMessageHandler(MessageQueue &in, MessageQueue &out);
  
  ~GroundTruthMessageHandler();
  
  void send();
  
  unsigned receive();
  
  static MessageQueue& getOutQueue();
  
  /**
   * The method starts the actual communication on the given port.
   * @param port The UDP port this handler is listening to.
   * @param subnet The subnet the handler is broadcasting to.
   */
  void start(int port, const char* subnet);
  
private:
  
  MessageQueue &in,&out;
  int port;
  
  UdpComm socket;
};
