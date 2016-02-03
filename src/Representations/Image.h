
#pragma once
#include <cv.h>

class Image : public cv::Mat {
public:
	Image() : cv::Mat() {}
};