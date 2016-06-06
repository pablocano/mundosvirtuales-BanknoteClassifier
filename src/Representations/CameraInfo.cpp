#include "CameraInfo.h"
#include <opencv2/imgproc/imgproc.hpp>

void CameraInfo::draw(cv::Mat &image) const
{
    cv::circle(image, cv::Point(fieldCenterInImage.x,fieldCenterInImage.y), 5, cv::Scalar(255,0,0));
}
