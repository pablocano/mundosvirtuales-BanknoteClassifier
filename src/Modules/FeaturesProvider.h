#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Features.h"
#include "Representations/GpuImage.h"
#include <opencv2/xfeatures2d.hpp>

#ifdef BC_WITH_CUDA
#include "opencv2/core/cuda.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/xfeatures2d/cuda.hpp"
#endif

MODULE(FeaturesProvider,
{,
 REQUIRES(GpuGrayImageEq),
 PROVIDES(Features),
 DEFINES_PARAMETERS(
 {,
  (double)(100.0) hessianThreshold,
  (int)(4) nOctaves,
  (int)(4) nOctaveLayers,
  (bool)(true) extended,
  (int)(200) maskX,
  (int)(150) maskY,
  (int)(1500) maskWidth,
  (int)(1800) maskHeight,
 }),
});

class FeaturesProvider : public FeaturesProviderBase
{
public:
    FeaturesProvider();

    cv::cuda::SURF_CUDA surf;

    cv::cuda::GpuMat mask;

    void update(Features & features);
};
