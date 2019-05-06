#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Math/OpenCv.h"
#include <opencv2/features2d.hpp>
#include <opencv2/cudafeatures2d.hpp>

/**
 * @brief Struct to storage features
 * Struct that contains the keypoints and the descriptors of an image
 * @author Pablo Cano
 */

class Features : public Streamable
{
public:
    /**
     * @brief Features Constructor
     */
    Features();

    /**
     * @brief draw the representation
     */
    void draw() const;

    std::vector<cv::KeyPoint> keypoints;

    CvGpuMat keypointsGpu;
    CvGpuMat descriptors;

protected:

    void serialize(In* in, Out* out) override;
};
