//
//  RobotPoseProvider.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-08-16.
//
//

#include "RobotPoseProvider.h"
#include "Tools/Math/Transformation.h"

MAKE_MODULE(RobotPoseProvider, GroundTruth)

void RobotPoseProvider::update(RobotsPoses &robotsPoses)
{
  robotsPoses.robotPoses.clear();
  for (const RobotPercept::Robot& robot : theRobotPercept.robots) {
    analizeBlob(robot);
    analizePosibleRobot();
    if (posibleRobot.valid) {
      calculatePose(robotsPoses);
    }
  }
}

void RobotPoseProvider::analizeBlob(const RobotPercept::Robot &robot)
{
  posibleRobot.clear();
  for (const Blobs::Blob &blob : theBlobs.blobs) {
    if(blob.center.x < robot.leftUpper.x - minDistance || blob.center.x > robot.rightBottom.x + minDistance || blob.center.y < robot.leftUpper.y - minDistance || blob.center.y > robot.rightBottom.y + minDistance)
      continue;
    if (!blob.color.is(none) && !blob.color.is(green) && !blob.color.is(white))
    {
      posibleRobot.shoulders.push_back(blob);
    }
  }
}

void RobotPoseProvider::analizePosibleRobot()
{
  if(posibleRobot.shoulders.size() != 2)
  {
    posibleRobot.valid = false;
    return;
  }
  for(const RobotsIdentifiers::Identifier& identifier : theRobotsIdentifiers.identifiers)
  {
    if(posibleRobot.shoulders[0].color.is(identifier.leftShoulder) && posibleRobot.shoulders[1].color.is(identifier.rightShoulder))
    {
      posibleRobot.valid = true;
      posibleRobot.team = identifier.team;
      posibleRobot.number = identifier.number;
      posibleRobot.leftShoulder = Transformation::imageToImageCorrected(posibleRobot.shoulders[0].center,theCameraInfo);
      posibleRobot.rightShoulder = Transformation::imageToImageCorrected(posibleRobot.shoulders[1].center,theCameraInfo);
      return;
    }
    if(posibleRobot.shoulders[0].color.is(identifier.rightShoulder) && posibleRobot.shoulders[1].color.is(identifier.leftShoulder))
    {
      posibleRobot.valid = true;
      posibleRobot.team = identifier.team;
      posibleRobot.number = identifier.number;
      posibleRobot.leftShoulder = Transformation::imageToImageCorrected(posibleRobot.shoulders[1].center,theCameraInfo);
      posibleRobot.rightShoulder = Transformation::imageToImageCorrected(posibleRobot.shoulders[0].center,theCameraInfo);
      return;
    }
  }
}


void RobotPoseProvider::calculatePose(RobotsPoses &robotsPoses)
{
  Vector2<> center = Vector2<>((posibleRobot.leftShoulder.x + posibleRobot.rightShoulder.x)/2.f,(posibleRobot.leftShoulder.y + posibleRobot.rightShoulder.y)/2.f);
  
  Vector2<> direction = Vector2<>(posibleRobot.rightShoulder.x - posibleRobot.leftShoulder.x, posibleRobot.rightShoulder.y - posibleRobot.leftShoulder.y).rotateLeft();
  
  Vector2<int> centerInImage = Transformation::imageCorrectedToImage(Vector2<int>(center.x, center.y),theCameraInfo);
  
  robotsPoses.robotPoses.push_back(RobotsPoses::RobotPose(direction.angle(),center,centerInImage));
  
}
