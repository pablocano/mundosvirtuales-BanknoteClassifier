/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#pragma once

#include "Representations/ColorModel/ColorCalibration.h"
#include "Tools/Comm/GroundTruthMessageHandler.h"
#include "Tools/ModuleManager/ModuleManager.h"
#include "Tools/Process.h"
#include <opencv2/core/core.hpp>
#include <string>

class GroundTruth : public Process{
  
public:
  DEBUGGING;
  
private:
  GROUND_TRUTH_COMM;
  
protected:
  
  void init();
  
  int main();
  
  ModuleManager moduleManager;
  
  bool pause;
  
public:
  
  GroundTruth();
  
  /**
   * The function handles incoming debug messages.
   * @param message the message to handle.
   * @return Has the message been handled?
   */
  virtual bool handleMessage(MessageQueue& message);
};

