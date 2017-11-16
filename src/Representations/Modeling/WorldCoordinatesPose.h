#pragma once

#include "Tools/Streamable.h"
#include "Tools/Math/Eigen.h"


class WorldCoordinatesPose : public Streamable
{
public:

    Vector2f point;

    bool valid;
};

