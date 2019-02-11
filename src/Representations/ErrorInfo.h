#pragma once
#include "Tools/Streams/AutoStreamable.h"


STREAMABLE(ErrorInfo,
{,
    (int)(0) error,
    (int)(0) lastbanknote,
});
