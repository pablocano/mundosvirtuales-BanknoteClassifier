//
//  RobotPoseProvider.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-08-16.
//
//

#include "RobotPoseProvider.h"

MAKE_MODULE(RobotPoseProvider, GroundTruth)

void RobotPoseProvider::update(RobotsPoses &robotsPoses)
{
  robotsPoses.robotPoses.clear();
  for (const RobotPercept::Robot& robot : theRobotPercept.robots) {
    analizeRobot(robot);
    if (posibleRobot.valid) {
      calculatePose(robotsPoses);
    }
  }
}

void RobotPoseProvider::analizeRobot(const RobotPercept::Robot &robot)
{
  posibleRobot = {Vector2<int>(),Vector2<int>(),false};
  for (const Blobs::Blob &blob : theBlobs.blobs) {
    if(blob.center.x < robot.leftUpper.x - minDistance || blob.center.x > robot.rightBottom.x + minDistance || blob.center.y < robot.leftUpper.y - minDistance || blob.center.y > robot.rightBottom.y + minDistance)
      continue;
    if (blob.color.is(red) && posibleRobot.leftShoulder == Vector2<int>()) {
      posibleRobot.leftShoulder = blob.center;
    }
    if (blob.color.is(blue) && posibleRobot.rightShoulder == Vector2<int>()) {
      posibleRobot.rightShoulder = blob.center;
    }
  }
  if (posibleRobot.leftShoulder != Vector2<int>() && posibleRobot.rightShoulder != Vector2<int>()) {
    posibleRobot.valid = true;
  }
}

void RobotPoseProvider::calculatePose(RobotsPoses &robotsPoses)
{
  Vector2<> center = Vector2<>((posibleRobot.leftShoulder.y + posibleRobot.rightShoulder.y)/2.f,(posibleRobot.leftShoulder.x + posibleRobot.rightShoulder.x)/2.f);
  
  Vector2<> direction = Vector2<>(posibleRobot.rightShoulder.y - posibleRobot.leftShoulder.y, posibleRobot.rightShoulder.x - posibleRobot.leftShoulder.x).rotateRight();
  
  robotsPoses.robotPoses.push_back(RobotsPoses::RobotPose(direction.angle(),center));
  
}
