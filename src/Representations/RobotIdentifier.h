#pragma once
#include "Tools/ColorClasses.h"
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>

struct RobotsIdentifiers : public Streamable {
  
  struct Identifier
  {
    Color leftShoulder;
    Color rightShoulder;
    int team;
    int number;
  };
  
  //void draw(cv::Mat& image) const;
  
  std::vector<Identifier> identifiers;
};
