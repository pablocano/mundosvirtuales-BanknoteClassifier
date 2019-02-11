//
//  CommunicationHandler.hpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 23-08-16.
//
//

#pragma once
#include "Tools/MessageQueue/MessageQueue.h"
#include "Representations/BanknoteClassifierMessages.h"
#include <vector>

class CommunicationHandler : public MessageHandler{
private:
  
  static CommunicationHandler* theInstance;
  
  bool handleMessage(InMessage& message);
  
public:
  
  CommunicationHandler();
  
  static void handleAllMessages(MessageQueue& receiver);
  
  int numOfMessagesReceived;
  
  //std::vector<GroundTruthRobot> robots;
  //std::vector<GroundTruthBall> balls;
  
};
