#pragma once

#include "Representations/BallPerception.h"
#include "Representations/CameraInfo.h"
#include "Representations/GroundTruthMessages.h"
#include "Representations/FrameInfo.h"
#include "Representations/RobotPose.h"
#include "Tools/ModuleManager/Module.h"

MODULE(GroundTruthProvider,
{,
  REQUIRES(CameraInfo),
  REQUIRES(FrameInfo),
  REQUIRES(RobotsPoses),
  REQUIRES(BallPerception),
  PROVIDES(GroundTruthMessageOutput),
});

class GroundTruthProvider : public GroundTruthProviderBase
{
public:
  
  void update(GroundTruthMessageOutput& groundTruthMessageOutput);
};
