

#pragma once
#include "Representations/Blobs.h"
#include "Representations/CameraInfo.h"
#include "Representations/RobotIdentifier.h"
#include "Representations/RobotPercept.h"
#include "Representations/RobotPose.h"
#include "Tools/ModuleManager/Module.h"

MODULE(RobotPoseProvider,
{,
  REQUIRES(CameraInfo),
  REQUIRES(Blobs),
  REQUIRES(RobotsIdentifiers),
  REQUIRES(RobotPercept),
  PROVIDES(RobotsPoses),
});

class RobotPoseProvider : public RobotPoseProviderBase {
  
public:
  
  RobotPoseProvider() : minDistance(10) {}
  
  void update(RobotsPoses& robotPose);
  
  struct PosibleRobot{
    std::vector<Blobs::Blob> shoulders;
    Vector2<int> leftShoulder;
    Vector2<int> rightShoulder;
    bool valid;
    int team;
    int number;
    void clear()
    {
        shoulders.clear();
        valid = false;
    }
  };
  
private:
  
  void analizeBlob(const RobotPercept::Robot& robot);

  void analizePosibleRobot();
  
  void calculatePose(RobotsPoses& robotPose);
  
  int minDistance;
  
  PosibleRobot posibleRobot;
};
