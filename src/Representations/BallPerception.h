
#pragma once

#include "Tools/Math/Range.h"
#include "Tools/Math/Vector2.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/Streamable.h"

class BallPerception : public Streamable
{
public:
    using Color = ColorModel::Colors;
    
    void draw(cv::Mat& image) const;
	
    Vector2<int> position;
    int radius;
    bool wasSeen;
};
