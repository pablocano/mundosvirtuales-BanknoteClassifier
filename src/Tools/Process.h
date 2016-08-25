#pragma once
#include "Tools/ModuleManager/Blackboard.h"
#include "Tools/Settings.h"

class Process {
  
public:
  
  Process();
  
  int procesMain();
  
  void setGlobals();
  
protected:
  
  virtual int main() = 0;
  
  virtual void init() {};
  
private:
  bool initialized;
  Settings settings;
  Blackboard blackboard;
  
};