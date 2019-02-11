//
//  Blobs.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-08-16.
//
//

#include "Blobs.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <ostream>


Blobs::Blob::Blob(const Vector2i& center, const std::vector<Vector2i> &borders,const float& area, const Color& color) : center(center), borders(borders), area(area), color(color) {}

void Blobs::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:Blobs", "drawingOnImage");
  for(auto& blob : blobs)
  {

    const std::vector<Vector2i>& borders = blob.borders;
    cv::Scalar color = ColorClasses::DrawingColors::getDrawingColor(blob.color.colors);

    for(int i = 0; i < borders.size(); i++)
    {
        int a = (i + 1)%borders.size();
        LINE("representation:Blobs", borders[i].x(), borders[i].y(), borders[(i + 1)%borders.size()].x(), borders[(i + 1)%borders.size()].y(), 5, Drawings::solidPen, ColorRGBA(color[2],color[1],color[0]));
    }
  }
}
