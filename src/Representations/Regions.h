
#pragma once

#include "Tools/Range.h"
#include "Tools/Vector2.h"
#include "Representations/Image.h"
#include "Representations/ColorModel/ColorModel.h"
#include <cv.h>

class Regions {

public:
    using Color = ColorModel::Colors;
	struct Line {
		Vector2 left;
		Vector2 right;
        Color color;
        Line(Vector2 left, Vector2 right, Color color) : left(left), right(right), color(color) {}
	};
    
    void draw(Image* image) const;
	
	std::vector<Line> regions;
};