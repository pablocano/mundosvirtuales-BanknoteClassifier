/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#pragma once

#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "Tools/ModuleManager/ModulePackage.h"
#include "Tools/ProcessFramework/Process.h"
#include <opencv2/core/core.hpp>
#include <string>

class BanknoteClassifier : public Process{
  
private:
  BANKNOTE_CLASSIFIER_COMM;

  Receiver<DebugToBanknoteClassifier> theDebugReceiver;
  DebugSender<BanknoteClassifierToDebug> theDebugSender;
  Receiver<ConfirmerToBanknoteClassifier> theMotionReceiver;
  Sender<BanknoteClassifierToConfirmer> theMotionSender;
  
  ModuleManager moduleManager;
  int numberOfMessages;
  bool pause;
  
public:
  
  BanknoteClassifier();

  /**
   * The method is called from the framework once in every frame.
   */
  bool main() override;

  /**
   * The method is called directly before the first call of main().
   */
  void init() override;
  
  /**
   * The function handles incoming debug messages.
   * @param message the message to handle.
   * @return Has the message been handled?
   */
  virtual bool handleMessage(InMessage &message) override;
};

