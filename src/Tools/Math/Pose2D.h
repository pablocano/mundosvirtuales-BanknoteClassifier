#pragma once
#include "Tools/Math/Vector2.h"

class Pose2D {
public:
  Pose2D() {}
  
  Pose2D(const float rotation, const Vector2<>& position): rotation(rotation), position(position) {}
  
  float rotation;
  Vector2<> position;
};
