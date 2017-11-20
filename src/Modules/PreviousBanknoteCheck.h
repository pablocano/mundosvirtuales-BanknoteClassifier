#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/ErrorInfo.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include <opencv2/xfeatures2d.hpp>
#include "Representations/RobotFanuc.h"

MODULE(PreviousBanknoteCheck,
{,
    REQUIRES(GrayScaleImageEq),
    USES(ErrorInfo),
    REQUIRES(RobotFanuc),
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

    //Aux
    int error;
    int lastbanknote;
    int newSearch;
};
