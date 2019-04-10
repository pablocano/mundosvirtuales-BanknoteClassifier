//
//  Image.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 08-09-16.
//
//

#include "Image.h"
#include "Tools/Debugging/DebugImages.h"


Image::Image(const cv::Mat& other) : CvMat(other) {}

void Image::draw() const
{
  SEND_DEBUG_IMAGE("Image",*this);
}

void GrayScaleImage::draw() const
{
  SEND_DEBUG_IMAGE("GrayScaleImage", *this);
}
