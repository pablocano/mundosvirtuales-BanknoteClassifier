
#pragma once

#include "Representations/ColorModel/ColorModel.h"
#include "Tools/Math/Range.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Streams/Streamable.h"
#include <opencv2/core/core.hpp>

class Regions : public Streamable
{
public:
  using Color = ColorModel::Colors;
  struct Line {
    Vector2i left;
    Vector2i right;
    int depth;
    Color color;
    Line(Vector2i left, Vector2i right,int depth, Color color) : left(left), right(right), depth(depth), color(color) {}
    
    Vector2i getCenter() const
    {
      return (left + right) / 2;
    }
  };
  
  void draw() const;
  
  std::vector<Line> regions;
};
