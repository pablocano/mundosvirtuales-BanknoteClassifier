#include "BlobFilter.h"
#include "Representations/Classification.h"
#include <iostream>

MAKE_MODULE(BlobFilter, BanknoteClassifier)

BlobFilter::BlobFilter()
{
    i = 1;
    existsBlob = false;
    newblob = false;
}

void BlobFilter::update(BestBlob &bestblob){

    if(!theBlobs.blobs.empty()){
        bestblob.bestblob = theBlobs.blobs[0];
        bestblob.exists = true;
    }

    return;

    if (!theBlobs.blobs.empty()){
        if (theErrorInfo.error == 1){
            for (int j=i; j< theBlobs.blobs.size(); j++){
                if (!theBlobs.blobs[j].color.is(BlobFilter::getColor(theErrorInfo.lastbanknote))){
                    bestblob.bestblob = theBlobs.blobs[j];
                    std::cout<<"numero de blob"<<std::endl;
                    std::cout<<j<<std::endl;
                    i = j;
                    bestblob.exists = true;
                    newblob = true;
                    break;
                }
            }
        }
        else{
            i = 0;
            bestblob.bestblob = theBlobs.blobs[0];
            bestblob.exists = true;
            newblob = false;
        }

    }
    else{
        bestblob.exists = false;
    }

}

void BlobFilter::update(PreviousBanknotePosition &previousbanknote){
    if (newblob)
            previousbanknote.banknote = Classification::NONE;
}

Color BlobFilter::getColor(int banknote){
    Color color;
    switch (banknote) {
    case Classification::UNO_C:
        color = Color::green;
        break;
    case Classification::UNO_S:
        color = Color::green;
        break;
    case Classification::DOS_C:
        color = Color::yellow;
        break;
    case Classification::DOS_S:
        color = Color::yellow;
        break;
    case Classification::CINCO_C:
        color = Color::red;
        break;
    case Classification::CINCO_S:
        color = Color::red;
        break;
    case Classification::DIEZ_C:
        color = Color::blue;
        break;
    case Classification::DIEZ_S:
        color = Color::blue;
        break;
    case Classification::VEINTE_C:
        color = Color::orange;
        break;
    case Classification::VEINTE_S:
        color = Color::orange;
        break;
    }
    return color;
}
