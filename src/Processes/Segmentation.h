/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#include "Tools/ModuleManager/ModuleManager.h"
#include "Tools/Process.h"

class Segmentation : public Process{
  
public:
  
  Segmentation();
  
  void init();
  
  int main();
  
  ModuleManager moduleManager;
  
  bool trackBarInitialized;
};

