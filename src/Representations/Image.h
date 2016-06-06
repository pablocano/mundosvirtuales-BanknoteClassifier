
#pragma once
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>

class Image : public cv::Mat, public Streamable {};

class SegmentedImage : public cv::Mat, public Streamable {
public:
    SegmentedImage() : cv::Mat() {}

    SegmentedImage(const cv::Mat& m) : cv::Mat(m) {}
};

class MovementImage : public cv::Mat, public Streamable {
public:
    MovementImage() : cv::Mat() {}

    MovementImage(const cv::Mat& m) : cv::Mat(m) {}
};

class ImageBGR : public cv::Mat, public Streamable
{
public:
    ImageBGR() : cv::Mat() {}

    ImageBGR(const cv::Mat& m) : cv::Mat(m) {}
};
