#include "DummyImages.h"
#include "Tools/File.h"
#include "Tools/Debugging/Debugging.h"
#include <opencv2/opencv.hpp>

MAKE_MODULE(DummyImages, BanknoteClassifier);

DummyImages::DummyImages()
{
    currentImage = cv::imread(std::string(File::getGTDir()) + "/Data/database/0002.bmp", cv::IMREAD_COLOR);
}

void DummyImages::update(Image& image)
{
    cv::cvtColor(currentImage,image,cv::COLOR_BGR2YCrCb);

    currentImage.timeStamp = theFrameInfo.time;

    DEBUG_RESPONSE("representation:ImageBGR")
    {
        OUTPUT(idImage,bin,currentImage);
    };
}

void DummyImages::update(GrayScaleImage &grayScaleImage)
{
    cv::extractChannel(theImage,grayScaleImage,0);
}

void DummyImages::update(ImageBGR &imageBGR)
{
    imageBGR = currentImage.clone();
}
