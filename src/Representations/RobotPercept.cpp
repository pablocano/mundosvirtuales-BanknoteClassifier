#include "RobotPercept.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

void RobotPercept::draw(ImageBGR* image) const
{
    for(auto& robot : robots)
    {
        cv::rectangle(*image,cv::Point2i(robot.leftUpper.y, robot.leftUpper.x),cv::Point2i(robot.rightBottom.y, robot.rightBottom.x), cv::Scalar(255,0,0));
    }
}
