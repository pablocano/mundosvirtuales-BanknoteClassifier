#pragma once
#include "Tools/Streams/AutoStreamable.h"

STREAMABLE(FrameInfo,
{
    inline int getTimeSince(unsigned aTime) const
    {
     return static_cast<int>(time - aTime);
    }
    ,
    (int)(0) time,
});
