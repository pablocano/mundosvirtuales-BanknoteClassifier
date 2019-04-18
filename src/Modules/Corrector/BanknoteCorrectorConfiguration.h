#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/CameraInfo.h"
#include "Representations/ColorModel/ColorCalibration.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/FrameInfo.h"
#include "Tools/MessageQueue/InMessage.h"
#include "memory.h"

MODULE(BanknoteCorrectorConfiguration,
{,
  PROVIDES(CorrectorFrameInfo),
});

class BanknoteCorrectorConfiguration : public BanknoteCorrectorConfigurationBase
{
private:

    static BanknoteCorrectorConfiguration *theInstance;

    void update(CorrectorFrameInfo& frameInfo);
  
    unsigned last;
  
public:
  
  BanknoteCorrectorConfiguration();
  
  static bool handleMessage(InMessage& message);
};
