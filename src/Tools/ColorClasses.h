#pragma once
#include <opencv2/imgproc/imgproc.hpp>
#include "Tools/Debugging/ColorRGBA.h"
#include "Tools/Streams/Enum.h"

ENUM(Color,
{,
     none,
     white,
     green,
     blue,
     red,
     orange,
     yellow,
     black,
});

class DrawingColors {
  
public:
  
  static cv::Scalar getDrawingColor(unsigned color);
  
  static const ColorRGBA& getColorRGBA(Color colorClass)
  {
    static const ColorRGBA colors[numOfColors] =
    {
      ColorRGBA::gray,
      ColorRGBA::white,
      ColorRGBA::green,
      ColorRGBA::blue,
      ColorRGBA::red,
      ColorRGBA::orange,
      ColorRGBA::yellow,
      ColorRGBA::black
    };
    return colors[colorClass];
  }
};
