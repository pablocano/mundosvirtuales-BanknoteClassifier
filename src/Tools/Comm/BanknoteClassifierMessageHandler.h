#pragma once

#include "Tools/MessageQueue/MessageQueue.h"
#include "Tools/MessageQueue/MessageIDs.h"
#include "Tools/Comm/TcpComm.h"
#include <string.h>
#include <fstream>

#define BANKNOTE_CLASSIFIER_COMM \
MessageQueue theCommReceiver; \
MessageQueue theCommSender; \
BanknoteClassifierMessageHandler theBanknoteClassifierCommHandler

#define INIT_GROUND_TRUTH_COMM \
theBanknoteClassifierCommHandler(theCommReceiver, theCommSender)

#define START_BANKNOTE_CLASSIFIER_COMM \
theBanknoteClassifierCommHandler.start();

#define RECEIVE_BANKNOTE_CLASSIFIER_COMM \
(void) theBanknoteClassifierCommHandler.receive()

#define SEND_BANKNOTE_CLASSIFIER_COMM \
theBanknoteClassifierCommHandler.send()


class BanknoteClassifierMessageHandler : public MessageHandler
{
public:
  
  static BanknoteClassifierMessageHandler *theInstance;
  
  BanknoteClassifierMessageHandler(MessageQueue &in, MessageQueue &out);
  
  ~BanknoteClassifierMessageHandler();

  bool handleMessage(InMessage &message);
  
  void send();
  
  unsigned receive();
  
  static MessageQueue& getOutQueue();
  
  /**
   * The method starts the actual communication.
   * @param ip.
   */
  void start();
  
private:
  
  MessageQueue &theCommIn,&theCommOut;
  
  SocketClientTcp* lpSocket;

  std::string ip;

};
