#include "RobotPose.h"

void RobotPose::draw(cv::Mat& image) const
{
    for(auto &l : ls)
        l.draw(image,cv::Scalar(0,0,255));
}
