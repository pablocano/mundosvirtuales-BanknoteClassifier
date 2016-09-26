
#pragma once

#include "Representations/ColorModel/ColorModel.h"
#include "Tools/Math/Range.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Streamable.h"

class BallPerception : public Streamable
{
public:
    using Color = ColorModel::Colors;
    
    void draw() const;
	
    Vector2<int> positionInImage;
    Vector2<int> position;
    int radius;
    bool wasSeen;
};
