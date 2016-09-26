//
//  RobotPose.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 05-08-16.
//
//

#include "RobotPose.h"
#include "Tools/Debugging/DebugDrawings.h"

void RobotsPoses::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:RobotPose", "drawingOnImage");
  for (const RobotPose& robot : robotPoses) {
    Vector2<int> point = robot.centerInImage + Vector2<int>(50,0).rotate(robot.rotation).mirrorY();
    LINE("representation:RobotPose", robot.centerInImage.x, robot.centerInImage.y, point.x, point.y, 2, Drawings::ps_solid, ColorRGBA::blue);
  }
}
