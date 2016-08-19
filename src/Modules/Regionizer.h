
#pragma once

#include "Representations/Image.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Regions.h"
#include "Tools/ModuleManager/Module.h"

MODULE(Regionizer,
{,
  REQUIRES(ColorModel),
  REQUIRES(Image),
  REQUIRES(MovementImage),
  PROVIDES(Regions),
});


class Regionizer : public RegionizerBase
{
public:
  
  Regionizer() : step(2) {}
  
  void update(Regions& regions);
  
  void findRightBound(const Vector2<int>& initPoint, Vector2<int>& result, ColorModel::Colors color);
  
  int step;
  
};
