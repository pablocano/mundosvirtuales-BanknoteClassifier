#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Features.h"
#include "Representations/FrameInfo.h"
#include "Representations/Image.h"

MODULE(BanknotePositionProvider,
{,
 REQUIRES(Features),
 REQUIRES(FrameInfo),
 REQUIRES(ImageBGR),
 PROVIDES(BanknotePosition),
});

class BanknotePositionProvider : public BanknotePositionProviderBase
{
public:
    BanknotePositionProvider();

    cv::Mat coloredImageModel1A;
    cv::Mat imageModel1A;
    std::vector<cv::Point2f> obj_corners;
    std::vector<cv::KeyPoint> keyPointsModel1A;
    cv::Mat descriptorModel1A;

    cv::BFMatcher matcher;

    void update(BanknotePosition& banknotePosition);
};

