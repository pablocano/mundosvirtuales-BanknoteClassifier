

#include "Blackboard.h"
#include "Representations/BallPerception.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Image.h"
#include "Representations/Regions.h"
#include <cstdlib>
#include <cstring>

Blackboard* Blackboard::theInstance = 0;

Blackboard::Blackboard() :
    theBackgroundImage(new BackgroundImage()),
    theBallPerception(new BallPerception()),
	theColorModel(new ColorModel()),
	theImage(new Image()),
	theRegions(new Regions()),
    theSegmentedImage(new SegmentedImage()){}

void Blackboard::operator=(const Blackboard& other)
{
	memcpy(this, &other, sizeof(Blackboard));
}
