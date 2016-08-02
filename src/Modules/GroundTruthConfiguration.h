#pragma once

#include "Representations/ColorModel/ColorCalibration.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ModuleManager/Module.h"

MODULE(GroundTruthConfiguration,
{,
  PROVIDES(ColorModel),
});

class GroundTruthConfiguration : public GroundTruthConfigurationBase
{
private:

  static GroundTruthConfiguration *theInstance;
  
  void update(ColorModel& colorModel);
  
  void readFile(std::string name);
  
  void writeFile(std::string name);
  
  ColorCalibration* theColorCalibration = nullptr;
  
  ColorCalibration colorCalibration;
  
public:
  
  GroundTruthConfiguration();
  
  static void getColorCalibration(ColorCalibration& newColorCalibration);
  
  static void setColorCalibration(const ColorCalibration& newColorCalibration);
  
  static void saveColorCalibration();
};
