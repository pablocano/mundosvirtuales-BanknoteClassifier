

#include "BallPerception.h"
#include <opencv2/imgproc/imgproc.hpp>

void BallPerception::draw(ImageBGR *image) const
{
    if (wasSeen)
    {
        cv::Mat copy;
        image->copyTo(copy);
        cv::circle(copy, cv::Point(position.y,position.x), radius, cv::Scalar(0,128,255), -1);
        cv::circle(copy, cv::Point(position.y,position.x), radius, cv::Scalar(0,0,0));
        cv::addWeighted(*image, 0.3, copy, 0.7, 0, *image);
    }
}
