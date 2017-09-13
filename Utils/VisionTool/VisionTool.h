#pragma once

#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "CommunicationHandler.h"

class VisionTool{
  
private:
  BANKNOTE_CLASSIFIER_COMM;
  
public:
  
  void init();
  
  int execute();
  
public:
  
  VisionTool();
  
  CommunicationHandler communicationHandler;

};
