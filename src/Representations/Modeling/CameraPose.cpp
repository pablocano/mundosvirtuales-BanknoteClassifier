#include "CameraPose.h"
#include "Tools/Debugging/DebugDrawings.h"

void CameraPose::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:CameraPose","drawingOnImage");

    COMPLEX_DRAWING("representation:CameraPose", drawPose(););
}

void CameraPose::drawPose() const
{

}

void CameraPoseFiltered::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:CameraPoseFiltered","drawingOnImage");

    COMPLEX_DRAWING("representation:CameraPoseFiltered", drawPose(););
}

