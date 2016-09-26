#include "BallPerception.h"
#include "Tools/Debugging/DebugDrawings.h"

void BallPerception::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:BallPerception", "drawingOnImage");
  if (wasSeen)
  {
    CIRCLE("representation:BallPerception", positionInImage.x, positionInImage.y, radius, 1, Drawings::ps_solid, ColorRGBA::black, Drawings::bs_solid, ColorRGBA(255, 128, 0, 80));
  }
}
