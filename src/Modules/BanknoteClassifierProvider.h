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

    static BanknoteClassifierProvider* theInstance;

public:

    /**
     * @brief BanknoteClassifierProvider Constructor
     */
    BanknoteClassifierProvider();

    /**
     * @brief update the classification representation
     * @param classification the result of this module
     */
    void update(Classification& classification);

    static Classification::Banknote getClassification(const Blobs::Blob& blob);
};
