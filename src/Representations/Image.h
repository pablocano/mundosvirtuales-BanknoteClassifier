
#pragma once
#include <cv.h>

class Image : public cv::Mat {
public:
	Image() : cv::Mat() {}
    
    Image(const cv::Mat& m) : cv::Mat(m) {}
};

class SegmentedImage : public Image {
public:
    SegmentedImage() : Image() {}
    
    SegmentedImage(const cv::Mat& m) : Image(m) {}
};

class BackgroundImage : public cv::Mat {
public:
    BackgroundImage() : cv::Mat() {}
};
