#pragma once

#include "Tools/Comm/GroundTruthMessageHandler.h"
#include "CommunicationHandler.h"

class VisionTool{
  
private:
  GROUND_TRUTH_COMM;
  
public:
  
  void init();
  
  int execute();
  
public:
  
  VisionTool();
  
  CommunicationHandler communicationHandler;

};