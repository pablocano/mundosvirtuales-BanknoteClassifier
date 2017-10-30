#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include <opencv2/xfeatures2d.hpp>

MODULE(PreviousBanknoteCheck,
{,
    REQUIRES(GrayScaleImageEq),
    USES(BanknotePosition),
    PROVIDES(PreviousBanknotePosition),
});

class PreviousBanknoteCheck : public PreviousBanknoteCheckBase
{
public:

    PreviousBanknoteCheck();

    void update(PreviousBanknotePosition& previousBanknotePosition);

    cv::Ptr<cv::xfeatures2d::SURF> surf_;

    cv::Mat mask;

    Features features;
};
