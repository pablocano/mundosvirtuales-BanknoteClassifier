//
//  CommunicationHandler.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 23-08-16.
//
//

#include "CommunicationHandler.h"
#include "MessageIDs.h"
#include <iostream>

CommunicationHandler* CommunicationHandler::theInstance = 0;

CommunicationHandler::CommunicationHandler()
{
  theInstance = this;
}

bool CommunicationHandler::handleMessage(MessageQueue &message)
{
  GroundTruthRobot robot;
  GroundTruthBall ball;
  switch (message.getMessageID()) {
    case idGroundTruthMessageBall:
      message >> &ball;
      balls.push_back(ball);
      return true;
      
    case idGroundTruthMessageRobot:
      message >> &robot;
      robots.push_back(robot);
      return true;
      
    default:
      return false;
  }
}

void CommunicationHandler::handleAllMessages(MessageQueue &receiver)
{
  if(theInstance){
    theInstance->robots.clear();
    theInstance->balls.clear();
    receiver.handleAllMessages(*theInstance);
  }
  
  receiver.clear();
}