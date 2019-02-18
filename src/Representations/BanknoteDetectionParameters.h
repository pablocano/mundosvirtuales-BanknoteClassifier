
#pragma once

#include "Tools/Streams/AutoStreamable.h"

STREAMABLE(BanknoteDetectionParameters,
{,
    (float)(45.f) houghXYStep, /* in pixels */
    (float)(30.f) houghAngleStep, /* in degrees */
    (int)(9) houghVotesThresh,
    (float)(0.8f) minAllowedScale,
    (float)(1.2f) maxAllowedScale,
    (float)(20.f) ransacMaxError,
    (float)(30.f) ransacMaxError2,
    (int)(15) ransacMinConsensus,
    (int)(50) ransacNumberOfTrials,
    (float)(0.6f) maxAllowedIOU,
});
