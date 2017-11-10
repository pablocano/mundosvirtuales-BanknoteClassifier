#pragma once
#include "Representations/Image.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Streamable.h"
#include "Representations/ColorModel/ColorModel.h"

class Blobs : public Streamable
{
public:
  
  struct Blob
  {
    Blob() = default;
    Blob(const Vector2i& center, const std::vector<Vector2i> &borders,const float& area, const ColorModel::Colors& color) : center(center), borders(borders), area(area), color(color) {}
    Vector2i center;
    std::vector<Vector2i > borders;
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

class BestBlob : public Streamable
{
public:
    Blobs::Blob bestblob;

    bool exists;
};
