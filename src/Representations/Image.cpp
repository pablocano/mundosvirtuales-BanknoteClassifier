//
//  Image.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 08-09-16.
//
//

#include "Image.h"
#include "Tools/Debugging/DebugDrawings.h"


ImageBGR::ImageBGR(const cv::Mat& other) : CvMat(other) {}

void MovementImage::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:MovementImage", "drawingOnImage");
  COMPLEX_DRAWING("representation:MovementImage")
  {
    const unsigned char* rgb = data;
    for(int i = 0; i < rows; i+=4)
    {
      for (int j = 0; j < cols; j+=4)
      {
        if (*rgb)
          DOT("representation:MovementImage", j, i, ColorRGBA::red, ColorRGBA::red);
        rgb+=4;
      }
      rgb+=3*cols;
    }
  };
}
