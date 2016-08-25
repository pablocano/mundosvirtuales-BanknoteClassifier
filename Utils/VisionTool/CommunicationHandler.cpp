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
  printf("Receive Id:%s\n",getName(message.getMessageID()));
  GroundTruthRobot robot;
  GroundTruthBall ball;
  switch (message.getMessageID()) {
    case idGroundTruthMessageBall:
      message >> &ball;
      balls.push_back(ball);
      printf("Ball Pos X:%f,Ball Pos Y:%f\n",ball.ballPosition.x,ball.ballPosition.y);
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