
#pragma once

#include "Representations/ColorModel/ColorModel.h"
#include "Tools/Math/Range.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>

class Regions : public Streamable
{
public:
  using Color = ColorModel::Colors;
  struct Line {
    Vector2<int> left;
    Vector2<int> right;
    int depth;
    Color color;
    Line(Vector2<int> left, Vector2<int> right,int depth, Color color) : left(left), right(right), depth(depth), color(color) {}
    
    Vector2<int> getCenter() const
    {
      return (left + right) / 2;
    }
  };
  
  void draw() const;
  
  std::vector<Line> regions;
};
