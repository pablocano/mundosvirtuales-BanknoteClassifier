#pragma once

#include "Eigen.h"
#include "Angle.h"

class Pose2D {
public:
  Pose2D() {}
  
  Pose2D(const float rotation, const Vector2f& position): rotation(rotation), translation(position) {}
  
  Pose2D(const Vector2f& position) :
      translation(position),
      rotation(0)
  {}

  Angle rotation;
  Vector2f translation;
};
