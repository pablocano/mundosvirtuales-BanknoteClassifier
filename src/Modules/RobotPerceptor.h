
#pragma once
#include "Representations/Blobs.h"
#include "Representations/CameraInfo.h"
#include "Representations/RobotPercept.h"
#include "Tools/ModuleManager/Module.h"

MODULE(RobotPerceptor,
{,
  REQUIRES(CameraInfo),
  REQUIRES(Blobs),
  PROVIDES(RobotPercept),
});


class RobotPerceptor : public RobotPerceptorBase
{
public:
    void update(RobotPercept& robotPercept);
};
