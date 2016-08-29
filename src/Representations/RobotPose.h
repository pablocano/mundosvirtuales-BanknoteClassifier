#pragma once
#include "Tools/Math/Pose2D.h"
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>

class RobotsPoses : public Streamable {
public:
  struct RobotPose : public Pose2D
  {
    RobotPose(const float rotation, const Vector2<>& position, const Vector2<int>& positionInImage): Pose2D(rotation,position), centerInImage(positionInImage), team(0),number(0) {}

    Vector2<int> centerInImage;
    int team;
    int number;

  };
  
  void draw(cv::Mat& image) const;
  
  std::vector<RobotPose> robotPoses;
};
