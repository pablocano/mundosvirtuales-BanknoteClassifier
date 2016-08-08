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
    cv::rectangle(image,cv::Point2i(blob.leftUpper.y, blob.leftUpper.x),cv::Point2i(blob.rightBottom.y, blob.rightBottom.x), DrawingColors::getDrawingColor(blob.color.colors));
  }
}
