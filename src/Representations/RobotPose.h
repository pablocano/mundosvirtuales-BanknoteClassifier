#pragma once
#include "Tools/Math/Pose2D.h"
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>

class RobotsPoses : public Streamable {
public:
  struct RobotPose : public Pose2D
  {
    RobotPose(const float rotation, const Vector2<>& position, const Vector2<int>& positionInImage, int team, int number): Pose2D(rotation,position), centerInImage(positionInImage), team(team),number(number) {}

    Vector2<int> centerInImage;
    int team;
    int number;

  };
  
  void draw() const;
  
  std::vector<RobotPose> robotPoses;
};
