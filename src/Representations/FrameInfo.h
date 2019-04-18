#pragma once
#include "Tools/Streams/AutoStreamable.h"

STREAMABLE(FrameInfo,
{
    inline int getTimeSince(unsigned aTime) const
    {
     return static_cast<int>(time - aTime);
    }
    ,
    (unsigned)(0) time,
});

STREAMABLE_WITH_BASE(CorrectorFrameInfo, FrameInfo,
{,});
