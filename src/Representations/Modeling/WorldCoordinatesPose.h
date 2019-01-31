#pragma once

#include "Tools/Streamable.h"
#include "Tools/Math/Pose2D.h"


class WorldCoordinatesPose : public Streamable, public Pose2D
{
public:

    WorldCoordinatesPose();

    bool valid;

    Vector2f pickOffset;

    Vector2f dropOffset;

    int banknote;

    int timeStamp;

};

