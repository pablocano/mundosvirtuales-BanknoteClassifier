#pragma once

#include "Representations/ColorModel/ColorCalibration.h"
#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "Tools/ModuleManager/ModuleManager.h"
#include "Tools/Process.h"
#include <opencv2/core/core.hpp>
#include <string>

class GripperCommunication : public Process{

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

  GripperCommunication();

  /**
   * The function handles incoming debug messages.
   * @param message the message to handle.
   * @return Has the message been handled?
   */
  virtual bool handleMessage(InMessage &message);
};
