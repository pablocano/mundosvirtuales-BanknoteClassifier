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
    Blob(const Vector2<int>& center, const std::vector<Vector2<int>> &borders, const ColorModel::Colors& color) : center(center), borders(borders), color(color) {}
    Vector2<int> center;
    std::vector<Vector2<int> > borders;
    ColorModel::Colors color;
  };
  
  void draw() const;
  
  std::vector<Blob> blobs;
};
