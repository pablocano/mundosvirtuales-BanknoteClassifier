/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#pragma once

#include "Representations/ColorModel/ColorCalibration.h"
#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "Tools/ModuleManager/ModuleManager.h"
#include "Tools/Process.h"
#include <opencv2/core/core.hpp>
#include <string>

#include "BanknoteClassifier_EXPORTS.h"

class BANCKNOTECLASSIFIER_EXPORT BanknoteClassifier : public Process{
  
public:
  DEBUGGING;
  
private:
  BANKNOTE_CLASSIFIER_COMM;
  
protected:
  
  void init();
  
  int main();
  
  ModuleManager moduleManager;
  
  bool pause;
  
public:
  
  BanknoteClassifier();
  
  /**
   * The function handles incoming debug messages.
   * @param message the message to handle.
   * @return Has the message been handled?
   */
  virtual bool handleMessage(InMessage &message);
};

