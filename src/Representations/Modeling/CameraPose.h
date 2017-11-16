#pragma once
#include "Tools/Streamable.h"
#include <opencv2/opencv.hpp>

class CameraPose : public Streamable
{
public:

    cv::Mat rvec,tvec;
};
