#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

MODULE(FeaturesProvider,
{,
    REQUIRES(GrayScaleImage),
    PROVIDES(Features),
});

class FeaturesProvider : public FeaturesProviderBase
{
public:
    FeaturesProvider();

    cv::Ptr<cv::FastFeatureDetector> detector;
    cv::Ptr<cv::xfeatures2d::FREAK> extractor;

    void update(Features & features);
};
