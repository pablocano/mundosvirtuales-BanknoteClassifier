#include "CameraInfo.h"
#include "Tools/Debugging/DebugDrawings.h"

void CameraInfo::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:CameraInfo", "drawingOnImage");
  CROSS("representation:CameraInfo", fieldCenterInImage.x, fieldCenterInImage.y, 5, 5, Drawings::ps_solid, ColorRGBA::blue);
}
