#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include <opencv2/xfeatures2d.hpp>

#ifdef BC_WITH_CUDA
#include "opencv2/core/cuda.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/xfeatures2d/cuda.hpp"
#endif

MODULE(FeaturesProvider,
{,
    REQUIRES(GrayScaleImageEq),
    PROVIDES(Features),
});

class FeaturesProvider : public FeaturesProviderBase
{
public:
    FeaturesProvider();

#ifndef BC_WITH_CUDA
    cv::Ptr<cv::xfeatures2d::SURF> surf_;
#else
    cv::cuda::SURF_CUDA surf_;
#endif

    cv::Mat mask;

    void update(Features & features);
};
