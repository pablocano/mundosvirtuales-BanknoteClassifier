#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/ErrorInfo.h"
#include "Representations/Blobs.h"
#include "Representations/BanknotePosition.h"

MODULE(BlobFilter,
{,
 REQUIRES(Blobs),
 USES(ErrorInfo),
 PROVIDES(BestBlob),
 PROVIDES(PreviousBanknotePosition)
});

class BlobFilter : public BlobFilterBase
{
public:

    BlobFilter();

    void update(BestBlob& bestblob);

    void update(PreviousBanknotePosition& previousbanknote);

    Color getColor(int banknote);

    //get that blob
    int i;

    //Aux
    bool existsBlob;
    bool newblob;
};
