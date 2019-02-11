#include "BlobFilter.h"
#include "Representations/Classification.h"

MAKE_MODULE(BlobFilter, BanknoteClassifier)

BlobFilter::BlobFilter()
{
    i = 0;
    existsBlob = false;
}

void BlobFilter::update(BestBlob &bestblob){

    bestblob.exists = false;

    /*if(!theBlobs.blobs.empty()){
        bestblob.bestblob = theBlobs.blobs[0];
        bestblob.exists = true;
    }

    return;*/

    if (!theBlobs.blobs.empty()){
        if (theErrorInfo.error == 1){
            for (int j=i; j< theBlobs.blobs.size(); j++){
                if (!theBlobs.blobs[j].color.is(BlobFilter::getColor(theErrorInfo.lastbanknote))){
                    bestblob.bestblob = theBlobs.blobs[j];
                    i = j;
                    bestblob.exists = true;
                    bestblob.newblob = true;
                    break;
                }
            }
            if (i == theBlobs.blobs.size())
                i = 0;

        }
        else{
            bestblob.bestblob = theBlobs.blobs[i];
            bestblob.exists = true;
            bestblob.newblob = false;
        }

    }
    else{
        bestblob.exists = false;
    }

}


ColorClasses::Color BlobFilter::getColor(int banknote){
    ColorClasses::Color color;
    switch (banknote) {
    case Classification::UNO_C:
        color = ColorClasses::Color::green;
        break;
    case Classification::UNO_S:
        color = ColorClasses::Color::green;
        break;
    case Classification::DOS_C:
        color = ColorClasses::Color::yellow;
        break;
    case Classification::DOS_S:
        color = ColorClasses::Color::yellow;
        break;
    case Classification::CINCO_C:
        color = ColorClasses::Color::red;
        break;
    case Classification::CINCO_S:
        color = ColorClasses::Color::red;
        break;
    case Classification::DIEZ_C:
        color = ColorClasses::Color::blue;
        break;
    case Classification::DIEZ_S:
        color = ColorClasses::Color::blue;
        break;
    case Classification::VEINTE_C:
        color = ColorClasses::Color::orange;
        break;
    case Classification::VEINTE_S:
        color = ColorClasses::Color::orange;
        break;
    }
    return color;
}
