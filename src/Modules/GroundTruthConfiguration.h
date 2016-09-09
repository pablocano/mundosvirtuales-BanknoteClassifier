#pragma once

#include "Representations/ColorModel/ColorCalibration.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/FrameInfo.h"
#include "Representations/RobotIdentifier.h"
#include "Tools/ModuleManager/Module.h"
#include "Tools/Messages/MessageQueue.h"

MODULE(GroundTruthConfiguration,
{,
  PROVIDES(ColorModel),
  PROVIDES(RobotsIdentifiers),
  PROVIDES(FrameInfo),
});

class GroundTruthConfiguration : public GroundTruthConfigurationBase
{
private:

  static GroundTruthConfiguration *theInstance;
  
  void update(ColorModel& colorModel);
  
  void update(RobotsIdentifiers& robotsIdentifiers);
  
  void update(FrameInfo& frameInfo);
  
  void readColorCalibration();
  
  void writeColorCalibration();
  
  void readRobotsIdentifiers();
  
  ColorCalibration* theColorCalibration = nullptr;
  
  RobotsIdentifiers* theRobotsIdentifiers = nullptr;
  
  ColorCalibration colorCalibration;
  
  unsigned last;
  
public:
  
  GroundTruthConfiguration();
  
  static void getColorCalibration(ColorCalibration& newColorCalibration);
  
  static void setColorCalibration(const ColorCalibration& newColorCalibration);
  
  static void saveColorCalibration();
  
  static bool handleMessage(MessageQueue& message);
};
