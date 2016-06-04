#pragma once

#include "Representations/BallPerception.h"
#include "Representations/CameraInfo.h"
#include "Representations/GroundTruthMessages.h"
#include "Representations/FrameInfo.h"
#include "Representations/RobotPercept.h"
#include "Tools/ModuleManager/Module.h"
#include <fstream>

MODULE(GroundTruthProvider,
{,
  REQUIRES(CameraInfo),
  REQUIRES(FrameInfo),
  REQUIRES(RobotPercept),
  REQUIRES(BallPerception),
  PROVIDES(GroundTruthMessageOutput),
});

class GroundTruthProvider : public GroundTruthProviderBase
{
public:
  
  void update(GroundTruthMessageOutput& groundTruthMessageOutput);
};
