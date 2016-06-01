#include "GroundTruthProvider.h"
#include "Tools/Comm/GroundTruthMessageHandler.h"

void GroundTruthProvider::update(GroundTruthMessageOutput *groundTruthMessageOutput)
{
  GroundTruthRobot robot;
  robot.robotNumber = 1;
  robot.teamNumber = 21;
  if (theRobotPercept->robots.size() != 0) {
    robot.robotPose.position = theRobotPercept->robots[0].posInField;
    robot.robotPose.rotation = 1.f;
    SEND_MESSAGE(idGroundTruthMessageRobot, robot);
  }
  GroundTruthBall ball;
  if (theBallPerception->wasSeen) {
    ball.ballPosition = Vector2<>(theBallPerception->position.x,theBallPerception->position.y);
    SEND_MESSAGE(idGroundTruthMessageBall, ball);
  }
}

