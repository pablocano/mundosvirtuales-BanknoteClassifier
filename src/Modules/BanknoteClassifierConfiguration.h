#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/CameraInfo.h"
#include "Representations/ColorModel/ColorCalibration.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/FrameInfo.h"
#include "Tools/MessageQueue/InMessage.h"
#include "memory.h"

MODULE(BanknoteClassifierConfiguration,
{,
  PROVIDES(ColorModel),
  PROVIDES(FrameInfo),
  //PROVIDES(CameraInfo),
});

class BanknoteClassifierConfiguration : public BanknoteClassifierConfigurationBase
{
private:

    static BanknoteClassifierConfiguration *theInstance;

    void update(ColorModel& colorModel);

    void update(FrameInfo& frameInfo);
  
    void readColorCalibration();
  
    void writeColorCalibration();
  
    ColorCalibration* theColorCalibration = nullptr;
  
    ColorCalibration colorCalibration;
  
    unsigned last;
  
public:
  
  BanknoteClassifierConfiguration();
  
  static void saveColorCalibration();
  
  static bool handleMessage(InMessage& message);
};
