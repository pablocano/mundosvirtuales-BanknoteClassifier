#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Image.h"
#include "Representations/GpuImage.h"


MODULE(GrayImageProvider,
{,
  REQUIRES(Image),
  PROVIDES(GpuGrayImage),
});

class GrayImageProvider : public GrayImageProviderBase
{
public:
  GrayImageProvider() {}

  void update(GpuGrayImage& image);

private:

  unsigned lastImageRecived;
};
