#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
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

    cv::Ptr<cv::FeatureDetector> detector;
    cv::Ptr<cv::DescriptorExtractor> extractor;


    void update(Features & features);
};
