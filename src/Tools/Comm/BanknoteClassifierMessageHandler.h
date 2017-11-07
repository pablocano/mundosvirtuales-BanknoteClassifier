#pragma once

#include "Tools/Messages/MessageQueue.h"
// #include "Tools/Comm/UdpComm.h"
#include "Tools/MessageIDs.h"
#include <fstream>

#define BANKNOTE_CLASSIFIER_COMM \
MessageQueue theCommReceiver; \
MessageQueue theCommSender; \
BanknoteClassifierMessageHandler theBanknoteClassifierCommHandler

#define INIT_GROUND_TRUTH_COMM \
theBanknoteClassifierCommHandler(theCommReceiver, theCommSender)

#define START_BANKNOTE_CLASSIFIER_COMM \
std::string bcastAddr = UdpComm::getWifiBroadcastAddress(); \
theBanknoteClassifierCommHandler.start(Global::getSettings()->teamPort, bcastAddr.c_str())

#define RECEIVE_BANKNOTE_CLASSIFIER_COMM \
(void) theBanknoteClassifierCommHandler.receive()

#define SEND_BANKNOTE_CLASSIFIER_COMM \
theBanknoteClassifierCommHandler.send()


class BanknoteClassifierMessageHandler
{
public:
  
  static BanknoteClassifierMessageHandler *theInstance;
  
  BanknoteClassifierMessageHandler(MessageQueue &in, MessageQueue &out);
  
  ~BanknoteClassifierMessageHandler();
  
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
  
  //UdpComm socket;
};
