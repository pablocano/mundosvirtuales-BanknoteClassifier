/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#include "Tools/Comm/GroundTruthMessageHandler.h"
#include "Tools/ModuleManager/Blackboard.h"
#include "Tools/ModuleManager/ModuleManager.h"
#include "Tools/Process.h"

class GroundTruth : public Process{
  
public:
  
  GroundTruth();
  
  void init();
  
  int main();
  
  bool handleKey();
  
  ModuleManager moduleManager;
  GroundTruthMessageHandler groundTruthMessageHandler;
  
  bool pause;
};

