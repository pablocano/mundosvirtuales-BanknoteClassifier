/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#include "Tools/ModuleManager/Blackboard.h"
#include "Tools/ModuleManager/ModuleManager.h"
#include "Tools/Process.h"

class Segmentation : public Process{
  
public:
  
  Segmentation();
  
  void init();
  
  int main();
  
  bool handleKey();
  
  ModuleManager moduleManager;
  
  bool pause, trackBarInitialized;
};

