#pragma once

#include "Tools/Streamable.h"
#include "Tools/Math/Pose2D.h"


class WorldCoordinatesPose : public Streamable, public Pose2D
{
public:

    bool valid;
};

