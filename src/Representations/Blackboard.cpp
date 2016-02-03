

#include "Blackboard.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Image.h"
#include <cstdlib>
#include <cstring>

Blackboard* Blackboard::theInstance = 0;

Blackboard::Blackboard() :
	theColorModel(new ColorModel()),
	theImage(new Image()) {}

void Blackboard::operator=(const Blackboard& other)
{
	memcpy(this, &other, sizeof(Blackboard));
}