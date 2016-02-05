

#include "BallPerception.h"

void BallPerception::draw(Image* image) const
{
    if (wasSeen)
    {
        cv::Mat copy;
        image->copyTo(copy);
        cv::circle(copy, cv::Point(position.y,position.x), 15, cv::Scalar(0,128,255), -1);
        cv::circle(copy, cv::Point(position.y,position.x), 15, cv::Scalar(0,0,0));
        cv::addWeighted(*image, 0.3, copy, 0.7, 0, *image);
    }
}