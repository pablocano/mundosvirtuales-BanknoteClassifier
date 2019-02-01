#include "BanknoteClassifierProvider.h"
#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "Tools/Math/Transformation.h"
#include "Tools/Comm/Comm.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/calib3d.hpp>
#include <map>
#include <iostream>

MAKE_MODULE(BanknoteClassifierProvider, BanknoteClassifier)

BanknoteClassifierProvider* BanknoteClassifierProvider::theInstance = 0;

BanknoteClassifierProvider::BanknoteClassifierProvider()
{
    theInstance = this;
}

void BanknoteClassifierProvider::update(Classification &classification)
{
    if(thePreviousBanknotePosition.banknote != Classification::NONE)
        return;

    classification.result = Classification::NONE;

    if(!theBestBlob.exists)
        return;

    classification.result = getClassification(theBestBlob.bestblob);
}

Classification::Banknote BanknoteClassifierProvider::getClassification(const Blobs::Blob &blob)
{
    if(!theInstance)
        return Classification::NONE;

    ColorModel::Colors blobColor = blob.color;

    if(blobColor.is(green))
        return Classification::UNO_C;
    else if(blobColor.is(yellow))
        return Classification::DOS_C;
    else if(blobColor.is(red))
        return Classification::CINCO_C;
    else if(blobColor.is(blue))
        return Classification::DIEZ_C;
    else if(blobColor.is(orange))
        return Classification::VEINTE_C;
}

