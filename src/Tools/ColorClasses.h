#pragma once
#include <opencv2/imgproc/imgproc.hpp>

enum Color {none, white, green, blue, red, orange, yellow, black, numOfColors};

class DrawingColors {
  
public:
  
  static cv::Scalar getDrawingColor(unsigned color);
};