#pragma once

#include "Representations/ColorModel/ColorCalibration.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/RobotIdentifier.h"
#include "Tools/ModuleManager/Module.h"

MODULE(GroundTruthConfiguration,
{,
  PROVIDES(ColorModel),
  PROVIDES(RobotsIdentifiers),
});

class GroundTruthConfiguration : public GroundTruthConfigurationBase
{
private:

  static GroundTruthConfiguration *theInstance;
  
  void update(ColorModel& colorModel);
  
  void update(RobotsIdentifiers& robotsIdentifiers);
  
  void readColorCalibration();
  
  void writeColorCalibration();
  
  void readRobotsIdentifiers();
  
  ColorCalibration* theColorCalibration = nullptr;
  
  RobotsIdentifiers* theRobotsIdentifiers = nullptr;
  
  ColorCalibration colorCalibration;
  
public:
  
  GroundTruthConfiguration();
  
  static void getColorCalibration(ColorCalibration& newColorCalibration);
  
  static void setColorCalibration(const ColorCalibration& newColorCalibration);
  
  static void saveColorCalibration();
};
