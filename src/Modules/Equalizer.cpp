#include "Equalizer.h"
#include "Tools/Debugging/DebugDrawings.h"

MAKE_MODULE(Equalizer, BaslerCamera)

Equalizer::Equalizer()
{
    clahe_ = cv::createCLAHE(2.0, cv::Size(6,6));
}

void Equalizer::update(GrayScaleImageEq& grayscaleimageEq)
{
    clahe_->apply(theGrayScaleImage,grayscaleimageEq);
    //DRAW_IMAGE("equalizer", (cv::Mat) grayscaleimageEq, theFrameInfo.time);
}
