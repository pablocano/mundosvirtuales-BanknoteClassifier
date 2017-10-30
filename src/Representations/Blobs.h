#pragma once
#include "Representations/Image.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Streamable.h"
#include "Representations/ColorModel/ColorModel.h"

class Blobs : public Streamable
{
public:
  
  struct Blob
  {
    Blob(const Vector2<int>& center, const std::vector<Vector2<int>> &borders,const float& area, const ColorModel::Colors& color) : center(center), borders(borders), area(area), color(color) {}
    Vector2<int> center;
    std::vector<Vector2<int> > borders;
    float area;
    ColorModel::Colors color;

    bool operator<(const Blob& other) const
    {
        return area > other.area;
    }

  };
  
  void draw() const;
  
  std::vector<Blob> blobs;
};
