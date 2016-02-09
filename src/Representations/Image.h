
#pragma once
#include <cv.h>

class Image : public cv::Mat {};

class SegmentedImage : public cv::Mat {
public:
    SegmentedImage() : cv::Mat() {}

    SegmentedImage(const cv::Mat& m) : cv::Mat(m) {}
};

class BackgroundImage : public cv::Mat {};

class ImageBGR: public cv::Mat{};
