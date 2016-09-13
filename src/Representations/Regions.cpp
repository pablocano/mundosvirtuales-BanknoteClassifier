#include "Regions.h"
#include "Tools/Debugging/DebugDrawings.h"

void Regions::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:Region", "drawingOnImage");
  for (auto& region : regions) {
    cv::Scalar color = DrawingColors::getDrawingColor(region.color.colors);
    LINE("representation:Region", region.right.x, region.right.y, region.left.x, region.left.y, 2, Drawings::ps_solid, ColorRGBA(color[2],color[1],color[0]));
  }
}
