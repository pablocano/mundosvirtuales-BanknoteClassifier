
#pragma once

#include "Tools/Range.h"
#include "Tools/Vector2.h"
#include "Representations/Image.h"
#include "Representations/ColorModel/ColorModel.h"
#include <cv.h>

class BallPerception {

public:
    using Color = ColorModel::Colors;
    
    void draw(Image* image) const;
	
    Vector2<int> position;
    int radius;
    bool wasSeen;
};
