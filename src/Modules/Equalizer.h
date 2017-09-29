#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Image.h"
#include <opencv2/imgproc.hpp>

MODULE(Equalizer,
{,
 REQUIRES(GrayScaleImage),
 PROVIDES(GrayScaleImageEq),
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
    void update(GrayScaleImageEq& grayscaleimageEq);

    /**
     * @brief Contrast Local Adaptive Histogram Equalization
     */
    cv::Ptr<cv::CLAHE> clahe_;

};



