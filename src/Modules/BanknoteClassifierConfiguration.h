#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/CameraInfo.h"
#include "Representations/ColorModel/ColorCalibration.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/FrameInfo.h"
#include "Representations/RobotIdentifier.h"

#include "Tools/Messages/MessageQueue.h"

MODULE(BanknoteClassifierConfiguration,
{,
  PROVIDES(ColorModel),
  PROVIDES(RobotsIdentifiers),
  PROVIDES(FrameInfo),
  //PROVIDES(CameraInfo),
});

class BanknoteClassifierConfiguration : public BanknoteClassifierConfigurationBase
{
private:

  static BanknoteClassifierConfiguration *theInstance;
  
  void update(ColorModel& colorModel);
  
  void update(RobotsIdentifiers& robotsIdentifiers);
  
  void update(FrameInfo& frameInfo);

  //void update(CameraInfo& cameraInfo);
  
  void readColorCalibration();
  
  void writeColorCalibration();
  
  void readRobotsIdentifiers();
  
  ColorCalibration* theColorCalibration = nullptr;
  
  RobotsIdentifiers* theRobotsIdentifiers = nullptr;
  
  ColorCalibration colorCalibration;
  
  unsigned last;
  
public:
  
  BanknoteClassifierConfiguration();
  
  static void saveColorCalibration();
  
  static bool handleMessage(MessageQueue& message);
};
