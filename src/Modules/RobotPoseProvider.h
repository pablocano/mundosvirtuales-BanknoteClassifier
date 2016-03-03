#pragma once
#include "Representations/Blackboard.h"
#include "Tools/ModuleManager/Module.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Image.h"
#include "Representations/RobotPercept.h"
#include "Representations/RobotPose.h"

MODULE(RobotPoseProvider)
    REQUIRES(ColorModel)
    REQUIRES(Image)
    REQUIRES(RobotPercept)
    PROVIDES(RobotPose)
END_MODULE

class RobotPoseProvider : public RobotPoseProviderBase
{
public:
    void update(RobotPose *robotPose);
};
