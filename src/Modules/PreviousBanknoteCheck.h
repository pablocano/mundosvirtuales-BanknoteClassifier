#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/ErrorInfo.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include <opencv2/xfeatures2d.hpp>
#include "Representations/RobotFanuc.h"
#include "Representations/RegState.h"
#include "Representations/Modeling/WorldCoordinatesPose.h"

#ifdef BC_WITH_CUDA
#include "opencv2/core/cuda.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/xfeatures2d/cuda.hpp"
#endif

MODULE(PreviousBanknoteCheck,
{,
    REQUIRES(GrayScaleImageEq),
    REQUIRES(RegState),
    USES(ErrorInfo),
    USES(BanknotePosition),
    USES(BanknotePositionFiltered),
    USES(WorldCoordinatesPose),
    PROVIDES(PreviousBanknotePosition),
});

class PreviousBanknoteCheck : public PreviousBanknoteCheckBase
{
public:

    PreviousBanknoteCheck();

    void update(PreviousBanknotePosition& previousBanknotePosition);

#ifndef BC_WITH_CUDA
    cv::Ptr<cv::xfeatures2d::SURF> surf_;
#else
    cv::cuda::SURF_CUDA surf_;
#endif

    cv::Mat mask;

    Features features;

    //Aux
    int error;
    int lastbanknote;
    int newSearch;

};
