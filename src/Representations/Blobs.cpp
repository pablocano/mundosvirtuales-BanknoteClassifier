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

void Blobs::draw(/*cv::Mat &image*/) const
{
  DECLARE_DEBUG_DRAWING("representation:Blobs", "drawingOnImage");
  for(auto& blob : blobs)
  {
    cv::Scalar color = DrawingColors::getDrawingColor(blob.color.colors);
    RECTANGLE("representation:Blobs", blob.leftUpper.x, blob.leftUpper.y, blob.rightBottom.x, blob.rightBottom.y, 5, Drawings::ps_solid, ColorRGBA(color[2],color[1],color[0]));
  }
}
