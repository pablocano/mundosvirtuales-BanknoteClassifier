#include "Equalizer.h"

MAKE_MODULE(Equalizer, BanknoteClassifier)

Equalizer::Equalizer()
{
    clahe_ = cv::createCLAHE(2.0, cv::Size(6,6));
}

void Equalizer::update(GrayScaleImageEq& grayscaleimageEq)
{
    clahe_->apply(theGrayScaleImage,grayscaleimageEq);
}
