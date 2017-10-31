#pragma once

#include "Eigen.h"

class Pose2D {
public:
  Pose2D() {}
  
  Pose2D(const float rotation, const Vector2f& position): rotation(rotation), position(position) {}
  
  float rotation;
  Vector2f position;
};
