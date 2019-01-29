
#pragma once

#include "Representations/Image.h"
#include "Representations/BanknotePosition.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Regions.h"
#include "Tools/ModuleManager/Module.h"

MODULE(Regionizer,
{,
  REQUIRES(ColorModel),
  REQUIRES(Image),
  REQUIRES(PreviousBanknotePosition),
  PROVIDES(Regions),
});


class Regionizer : public RegionizerBase
{
public:
  
  Regionizer() : step(5) {}
  
  void update(Regions& regions);
  
  void findRightBound(const Vector2i& initPoint, Vector2i& result, ColorModel::Colors color);
  
  int step;
  
};
