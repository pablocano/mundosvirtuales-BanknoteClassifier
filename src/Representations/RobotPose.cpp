//
//  RobotPose.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 05-08-16.
//
//

#include "RobotPose.h"
#include <opencv2/imgproc/imgproc.hpp>

void RobotsPoses::draw(cv::Mat &image) const
{
  for (const RobotPose& robot : robotPoses) {
    cv::Point pt1 = cv::Point(robot.position.x, robot.position.y);
    Vector2<> point = robot.position + Vector2<>(50.f,0.f).rotate(robot.rotation);
    cv::Point pt2 = cv::Point(point.x, point.y);
    cv::line(image, pt1, pt2, cv::Scalar(255,0,0));
  }
}