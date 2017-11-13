#include "Equalizer.h"

MAKE_MODULE(Equalizer,BanknoteClassifier)

Equalizer::Equalizer()
{
    clahe_ = cv::createCLAHE(2.0, cv::Size(10,10));
}

void Equalizer::update(GrayScaleImageEq& grayscaleimageEq)
{
    clahe_->apply(theGrayScaleImage,grayscaleimageEq);
}
