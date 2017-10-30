#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Blobs.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include <opencv2/xfeatures2d.hpp>

MODULE(FeaturesProvider,
{,
    REQUIRES(Blobs),
    REQUIRES(GrayScaleImageEq),
    REQUIRES(PreviousBanknotePosition),
    PROVIDES(Features),
});

class FeaturesProvider : public FeaturesProviderBase
{
public:
    FeaturesProvider();

    cv::Ptr<cv::xfeatures2d::SURF> surf_;

    cv::Mat mask;

    void update(Features & features);
};
