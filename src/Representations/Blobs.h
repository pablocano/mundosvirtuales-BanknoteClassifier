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
    Blob(const Vector2<int>& center, const Vector2<int> &leftUpper, const Vector2<int>& rightBottom, const ColorModel::Colors& color) : center(center), leftUpper(leftUpper), rightBottom(rightBottom), color(color) {}
    Vector2<int> center;
    Vector2<int> leftUpper;
    Vector2<int> rightBottom;
    ColorModel::Colors color;
  };
  
  void draw() const;
  
  std::vector<Blob> blobs;
};
