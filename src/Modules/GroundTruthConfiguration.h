#pragma once

#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ModuleManager/Module.h"

MODULE(GroundTruthConfiguration,
{,
  PROVIDES(ColorModel),
});

class GroundTruthConfiguration : public GroundTruthConfigurationBase
{
public:
  
  void update(ColorModel& colorModel);
};
