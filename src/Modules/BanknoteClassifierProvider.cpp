#include "BanknoteClassifierProvider.h"
#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "Tools/Math/Transformation.h"
#include "Tools/Comm/Comm.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/calib3d.hpp>
#include <map>
#include <iostream>

MAKE_MODULE(BanknoteClassifierProvider, BanknoteClassifier)

void BanknoteClassifierProvider::update(Classification &classification)
{
    if(thePreviousBanknotePosition.banknote != Classification::NONE)
        return;

    classification.result = Classification::NONE;

    if(!theBestBlob.exists)
        return;

    ColorModel::Colors blobColor = theBestBlob.bestblob.color;

    if(blobColor.is(green))
        classification.result = Classification::UNO_C;
    else if(blobColor.is(yellow))
        classification.result = Classification::DOS_C;
    else if(blobColor.is(red))
        classification.result = Classification::CINCO_C;
    else if(blobColor.is(blue))
        classification.result = Classification::DIEZ_C;
    else if(blobColor.is(orange))
        classification.result = Classification::VEINTE_C;
}

