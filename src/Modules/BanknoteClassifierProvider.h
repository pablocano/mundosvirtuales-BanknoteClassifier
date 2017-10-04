#pragma once

#include "Representations/CameraInfo.h"
#include "Representations/Classification.h"
#include "Representations/FrameInfo.h"
#include "Tools/ModuleManager/Module.h"

MODULE(BanknoteClassifierProvider,
{,
  REQUIRES(CameraInfo),
  REQUIRES(FrameInfo),
  PROVIDES(Classification),
});

class BanknoteClassifierProvider : public BanknoteClassifierProviderBase
{
public:

  BanknoteClassifierProvider();
  
  void update(Classification& classification);
};
