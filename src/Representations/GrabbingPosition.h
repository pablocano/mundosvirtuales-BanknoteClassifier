#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Math/Eigen.h"

STREAMABLE(GrabbingPosition,
{
    void draw() const;
    ,
    (Vector2f) pos,
});
