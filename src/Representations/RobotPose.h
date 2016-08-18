#pragma once
#include "Tools/Math/Pose2D.h"
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>

class RobotsPoses : public Streamable {
public:
  struct RobotPose : public Pose2D
  {
    RobotPose(const float rotation, const Vector2<>& position): Pose2D(rotation,position),team(0),number(0) {}

    int team;
    int number;

  };
  
  void draw(cv::Mat& image) const;
  
  std::vector<RobotPose> robotPoses;
};
