//
//  Image.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 08-09-16.
//
//

#include "Image.h"
#include "Tools/Debugging/DebugImages.h"


ImageBGR::ImageBGR(const cv::Mat& other) : CvMat(other) {}

void GrayScaleImageEq::draw() const
{
  SEND_DEBUG_IMAGE("EqualizedImage", *this);
}

void ImageBGR::draw() const
{
  SEND_DEBUG_IMAGE("ImageBGR",*this);
}
