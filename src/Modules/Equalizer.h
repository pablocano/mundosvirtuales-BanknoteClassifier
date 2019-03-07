#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/GpuImage.h"
#include <opencv2/cudaimgproc.hpp>
#include "Representations/FrameInfo.h"
MODULE(Equalizer,
{,
 REQUIRES(GpuGrayImage),
 REQUIRES(FrameInfo),
 PROVIDES(GpuGrayImageEq),
 DEFINES_PARAMETERS(
 {,
  (double)(2.0) clipLimit,
  (int)(6) sizeWindows,
 }),
});

class Equalizer : public EqualizerBase
{
public:
    /**
     * @brief Equalizer : Constructor
     */
    Equalizer();

    /**
     * @brief update
     * @param grayscaleimage
     */
    void update(GpuGrayImageEq &image);

    /**
     * @brief Contrast Local Adaptive Histogram Equalization
     */
    cv::Ptr<cv::cuda::CLAHE> clahe;

};



