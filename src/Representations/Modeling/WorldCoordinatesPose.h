#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Math/Pose2f.h"
#include "Representations/BanknotePosition.h"

STREAMABLE_WITH_BASE(WorldCoordinatesPose, Pose2f,
{,
    (bool)(false) valid,
    (Vector2f) pickOffset,
    (Vector2f) dropOffset,
    (int)(0) banknote,
    (int)(0) timeStamp,
    (BanknotePosition::GraspZone) zone,
    (int)(0) needEstirator,
});

