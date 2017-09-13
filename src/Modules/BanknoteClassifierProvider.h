#pragma once

#include "Representations/CameraInfo.h"
#include "Representations/BanknoteClassifierMessages.h"
#include "Representations/FrameInfo.h"
#include "Tools/ModuleManager/Module.h"

MODULE(BanknoteClassifierProvider,
{,
  REQUIRES(CameraInfo),
  REQUIRES(FrameInfo),
  PROVIDES(BanknoteClassifierMessageOutput),
});

class BanknoteClassifierProvider : public BanknoteClassifierProviderBase
{
public:
  
  void update(BanknoteClassifierMessageOutput& banknoteClassifierMessageOutput);
};
