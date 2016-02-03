
#pragma once

#include "Tools/Range.h"
#include "Tools/Vector2.h"
#include <cv.h>

class Regions {

public:
	struct Line {
		Vector2 left;
		Vector2 rigth;
	};
	
	std::vector<Line> regions;
};