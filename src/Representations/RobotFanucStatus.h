#pragma once
#include "Tools/Streams/AutoStreamable.h"
#include "Representations/RobotFanucRegisters.h"

STREAMABLE(RobotFanucStatus,
{
    ENUM(Step,
    {,
     undefined,
     waiting,
     picking,
     leaving,
     droping,
    });
    ,
    (Step)(undefined) step,
    (bool)(true) visionAreaClear,
});
