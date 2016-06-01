#include "RobotPoseProvider.h"
#include "Tools/Math/Line.h"

void RobotPoseProvider::update(RobotPose *robotPose)
{
    robotPose->ls.clear();
    for(auto const& robot : theRobotPercept->robots)
    {
        Line l1(robot.leftUpper,robot.rightBottom);
        Line l2(Vector2<int>(robot.leftUpper.x,robot.rightBottom.y),Vector2<int>(robot.rightBottom.x,robot.leftUpper.y));
        float whiteRatioL1 = 0;
        Vector2<float> point = l1.init;
        while(point.x <= l1.point2.x)
        {
            Vector2<int> last((int)point.x + 0.5f,(int)point.y + 0.5f);
            point += l1.dir;
            Vector2<int> eval((int)point.x + 0.5f,(int)point.y + 0.5f);
            if(eval == last)
                continue;
            if(theColorModel->getColor(theImage->at<cv::Vec3b>(point.x,point.y)).is(ColorModel::white))
            {
                whiteRatioL1++;
            }
        }
        whiteRatioL1 /= (l1.point2 - l1.point1).abs();

        float whiteRatioL2 = 0;
        point = l2.init;
        while(point.x <= l2.point2.x)
        {
            Vector2<int> last((int)point.x + 0.5f,(int)point.y + 0.5f);
            point += l2.dir;
            Vector2<int> eval((int)point.x + 0.5f,(int)point.y + 0.5f);
            if(eval == last)
                continue;
            if(theColorModel->getColor(theImage->at<cv::Vec3b>(point.x,point.y)).is(ColorModel::white))
            {
                whiteRatioL2++;
            }
        }
        whiteRatioL2 /= (l2.point2 - l2.point1).abs();

        robotPose->ls.push_back(whiteRatioL1 > whiteRatioL2 ? l1 : l2);
    }
}
