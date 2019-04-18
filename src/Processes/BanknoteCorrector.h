#pragma once

#include "Tools/ModuleManager/ModulePackage.h"
#include "Tools/ProcessFramework/Process.h"
#include <opencv2/core/core.hpp>
#include <string>

class BanknoteCorrector : public Process{

private:
  Receiver<DebugToBanknoteCorrector> theDebugReceiver;
  DebugSender<BanknoteCorrectorToDebug> theDebugSender;
  Receiver<BanknoteClassifierToBanknoteCorrector> theBanknoteClassifierReceiver;
  Sender<BanknoteCorrectorToBanknoteClassifier> theBanknoteClassifierSender;

  ModuleManager moduleManager;
  int numberOfMessages;
  bool pause;

public:

  BanknoteCorrector();

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
