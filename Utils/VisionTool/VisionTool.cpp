//
//  VisionTool.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 23-08-16.
//
//

#include "VisionTool.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

VisionTool::VisionTool(): INIT_GROUND_TRUTH_COMM
{
}

void VisionTool::init()
{
	START_BANKNOTE_CLASSIFIER_COMM;
}

int VisionTool::execute()
{
  RECEIVE_BANKNOTE_CLASSIFIER_COMM;
  CommunicationHandler::handleAllMessages(theCommReceiver);
  return 0;
}

