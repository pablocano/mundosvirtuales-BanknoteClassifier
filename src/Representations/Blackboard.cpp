

#include "Blackboard.h"
#include "Representations/BallPerception.h"
#include "Representations/CameraInfo.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/FrameInfo.h"
#include "Representations/Image.h"
#include "Representations/Regions.h"
#include "Representations/RobotPercept.h"
#include "Representations/RobotPose.h"
#include "Representations/SendInformation.h"
#include <cstdlib>
#include <cstring>

Blackboard* Blackboard::theInstance = 0;

Blackboard::Blackboard():
theBallPerception(new BallPerception()),
theCameraInfo(new CameraInfo()),
theColorModel(new ColorModel()),
theFrameInfo(new FrameInfo()),
theImage(new Image()),
theImageBGR(new ImageBGR()),
theMovementImage(new MovementImage()),
theRegions(new Regions()),
theRobotPercept(new RobotPercept()),
theRobotPose(new RobotPose()),
theSegmentedImage(new SegmentedImage()),
theSendInformation(new SendInformation()){}

void Blackboard::operator=(const Blackboard& other)
{
  memcpy(this, &other, sizeof(Blackboard));
}
