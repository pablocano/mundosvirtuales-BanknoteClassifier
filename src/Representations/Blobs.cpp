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

void Blobs::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:Blobs", "drawingOnImage");
  for(auto& blob : blobs)
  {

    const std::vector<Vector2i>& borders = blob.borders;
    cv::Scalar color = DrawingColors::getDrawingColor(blob.color.colors);

    for(int i = 0; i < borders.size(); i++)
    {
        int a = (i + 1)%borders.size();
        LINE("representation:Blobs", borders[i].x(), borders[i].y(), borders[(i + 1)%borders.size()].x(), borders[(i + 1)%borders.size()].y(), 5, Drawings::ps_solid, ColorRGBA(color[2],color[1],color[0]));
    }
  }
}
