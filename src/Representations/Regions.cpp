#include "Regions.h"
#include "Tools/Debugging/DebugDrawings.h"

using Color = ColorModel::Colors;

Regions::Line::Line(Vector2i left, Vector2i right,int depth, Color color) : left(left), right(right), depth(depth), color(color)
{}

void Regions::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:Region", "drawingOnImage");
  for (auto& region : regions) {
    cv::Scalar color = ColorClasses::DrawingColors::getDrawingColor(region.color.colors);
    LINE("representation:Region", region.right.x(), region.right.y(), region.left.x(), region.left.y(), 1, Drawings::solidPen, ColorRGBA(color[2],color[1],color[0]));
  }
}
