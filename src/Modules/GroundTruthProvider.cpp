#include "GroundTruthProvider.h"
#include "Tools/Comm/GroundTruthMessageHandler.h"
#include "Tools/Math/Transformation.h"

MAKE_MODULE(GroundTruthProvider, GroundTruth)

void GroundTruthProvider::update(GroundTruthMessageOutput& groundTruthMessageOutput)
{
  
  for (const RobotsPoses::RobotPose& robotPose : theRobotsPoses.robotPoses) {
    GroundTruthRobot robot;
    robot.teamNumber = robotPose.team;
    robot.robotNumber = robotPose.number;
    robot.robotPose.position = Transformation::imageToField(Vector2<>(robotPose.position.x,robotPose.position.y), theCameraInfo);
    robot.robotPose.rotation = robotPose.rotation;
    SEND_MESSAGE(idGroundTruthMessageRobot, robot);
    printf("Robot PosX: %f, Robot PosY: %f, Robot Rotation: %f\n",robot.robotPose.position.x,robot.robotPose.position.y,robot.robotPose.rotation);
  }
  
  GroundTruthBall ball;
  if (theBallPerception.wasSeen) {
    ball.ballPosition = Transformation::imageToField(Vector2<>(theBallPerception.position.x,theBallPerception.position.y),theCameraInfo);
    SEND_MESSAGE(idGroundTruthMessageBall, ball);
    printf("Ball PosX: %f, Ball PosY: %f\n",ball.ballPosition.x,ball.ballPosition.y);
  }
}

