

#pragma once
#include "Representations/Blobs.h"
#include "Representations/CameraInfo.h"
#include "Representations/RobotPercept.h"
#include "Representations/RobotPose.h"
#include "Tools/ModuleManager/Module.h"

MODULE(RobotPoseProvider,
{,
  REQUIRES(CameraInfo),
  REQUIRES(Blobs),
  REQUIRES(RobotPercept),
  PROVIDES(RobotsPoses),
});

class RobotPoseProvider : public RobotPoseProviderBase {
  
public:
  
  RobotPoseProvider() : minDistance(10) {}
  
  void update(RobotsPoses& robotPose);
  
  struct PosibleRobot{
    Vector2<int> leftShoulder;
    Vector2<int> rightShoulder;
    bool valid;
  };
  
private:
  
  void analizeRobot(const RobotPercept::Robot& robot);
  
  void calculatePose(RobotsPoses& robotPose);
  
  int minDistance;
  
  PosibleRobot posibleRobot;
};