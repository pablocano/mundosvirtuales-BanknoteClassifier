#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Blobs.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include <opencv2/xfeatures2d.hpp>

MODULE(FeaturesProvider,
{,
    REQUIRES(Blobs),
    REQUIRES(GrayScaleImageEq),
    PROVIDES(Features),
});

class FeaturesProvider : public FeaturesProviderBase
{
public:
    FeaturesProvider();

    cv::Ptr<cv::xfeatures2d::SURF> surf_;

    cv::Mat mask;

    std::vector<cv::Point> auxPolygon;

    void update(Features & features);
};
