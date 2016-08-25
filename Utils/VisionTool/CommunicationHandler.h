//
//  CommunicationHandler.hpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 23-08-16.
//
//

#pragma once
#include "Tools/Messages/MessageQueue.h"
#include "Representations/GroundTruthMessages.h"
#include <vector>

class CommunicationHandler : public MessageHandler{
private:
  
  static CommunicationHandler* theInstance;
  
  bool handleMessage(MessageQueue& message);
  
public:
  
  CommunicationHandler();
  
  static void handleAllMessages(MessageQueue& receiver);
  
  std::vector<GroundTruthRobot> robots;
  std::vector<GroundTruthBall> balls;
  
};