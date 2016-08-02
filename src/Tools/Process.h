#pragma once
#include "Tools/ModuleManager/Blackboard.h"

class Process {
  
public:
  
  Process() : initialized(false)
  {
    Blackboard::setInstance(blackboard);
  }
  
  int procesMain();
  
  virtual int main() = 0;
  
  virtual void init() {};
  
  Blackboard blackboard;
  
  bool initialized;
};