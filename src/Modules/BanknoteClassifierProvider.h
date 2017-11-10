#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Blobs.h"
#include "Representations/Classification.h"
#include <deque>

MODULE(BanknoteClassifierProvider,
{,
  REQUIRES(BestBlob),
  REQUIRES(PreviousBanknotePosition),
  PROVIDES(Classification),
});

class BanknoteClassifierProvider : public BanknoteClassifierProviderBase
{

public:

    /**
     * @brief update the classification representation
     * @param classification the result of this module
     */
    void update(Classification& classification);
};
