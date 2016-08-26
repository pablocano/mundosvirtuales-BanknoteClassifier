//
//  Blobs.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-08-16.
//
//

#include "Blobs.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <ostream>

void Blobs::draw(cv::Mat &image) const
{
  for(auto& blob : blobs)
  {
    cv::rectangle(image,cv::Point2i(blob.leftUpper.x, blob.leftUpper.y),cv::Point2i(blob.rightBottom.x, blob.rightBottom.y), DrawingColors::getDrawingColor(blob.color.colors));
  }
}
