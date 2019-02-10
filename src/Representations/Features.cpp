#include "Features.h"
#include "Tools/Debugging/DebugDrawings.h"


Features::Features()
{
}

void Features::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:Features", "drawingOnImage");
    for(auto keypoint : keypoints)
    {
        DOT("representation:Features",keypoint.pt.x, keypoint.pt.y, ColorRGBA::blue, ColorRGBA::blue);
    }
}
