

#include "Blackboard.h"
#include "Representations/BallPerception.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Image.h"
#include "Representations/Regions.h"
#include "Representations/RobotPercept.h"
#include <cstdlib>
#include <cstring>

Blackboard* Blackboard::theInstance = 0;

Blackboard::Blackboard() :
    theBallPerception(new BallPerception()),
	theColorModel(new ColorModel()),
	theImage(new Image()),
    theImageBGR(new ImageBGR()),
    theMovementImage(new MovementImage()),
	theRegions(new Regions()),
    theRobotPercept(new RobotPercept()),
    theSegmentedImage(new SegmentedImage()){}

void Blackboard::operator=(const Blackboard& other)
{
	memcpy(this, &other, sizeof(Blackboard));
}
