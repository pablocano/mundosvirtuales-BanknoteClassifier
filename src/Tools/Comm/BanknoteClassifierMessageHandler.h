#pragma once

#include "Tools/Messages/MessageQueue.h"
#include "Tools/Comm/UdpComm.h"
#include "Tools/MessageIDs.h"
#include "Tools/Comm/TcpComm.h"

#include <fstream>

#define BANKNOTE_CLASSIFIER_COMM \
MessageQueue theCommReceiver; \
MessageQueue theCommSender; \
BanknoteClassifierMessageHandler theBanknoteClassifierCommHandler

#define INIT_GROUND_TRUTH_COMM \
theBanknoteClassifierCommHandler(theCommReceiver, theCommSender)

#define START_BANKNOTE_CLASSIFIER_COMM \
theBanknoteClassifierCommHandler.start("10.0.42.8")

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
   * The method starts the actual communication.
   * @param ip.
   */
  void start(const char* ip);
  
private:
  
  MessageQueue &in,&out;
  
  SocketClientTcp* lpSocket;

};
