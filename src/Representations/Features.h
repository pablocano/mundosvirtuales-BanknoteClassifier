#pragma once

#include "Tools/Streamable.h"
#include <opencv2/features2d.hpp>

class Features : public Streamable
{
public:

    void draw() const;

    std::vector<cv::KeyPoint> keypoints;

    cv::Mat descriptors;
};
