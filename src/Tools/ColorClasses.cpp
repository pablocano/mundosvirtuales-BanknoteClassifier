//
//  ColorClasses.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 04-08-16.
//
//

#include "ColorClasses.h"

cv::Scalar DrawingColors::getDrawingColor(unsigned int color)
{
  static const unsigned baseColors[] =
  {
    0xffffffff, //white
    0xff00ff00, //green
    0xff0000ff, //blue
    0xffff0000, //red
    0xffff7f00, //orange
    0xffffff00, //yellow
    0xff000000  //black
  };
  
  static unsigned displayColors[1 << (Color::numOfColors - 1)];
  if(!displayColors[0])
  {
    union
    {
      unsigned color;
      unsigned char channels[4];
    } baseColor;
    
    displayColors[0] = 0xff7f7f7f; //grey
    for(int colors = 1; colors < 1 << (Color::numOfColors - 1); ++colors)
    {
      int count = 0;
      for(int i = 0; i < Color::numOfColors - 1; ++i)
        if(colors & 1 << i)
          ++count;
      unsigned mixed = 0;
      for(int i = 0; i < Color::numOfColors - 1; ++i)
        if(colors & 1 << i)
        {
          baseColor.color = baseColors[i];
          for(int j = 0; j < 4; ++j)
            baseColor.channels[j] /= count;
          mixed += baseColor.color;
        }
      displayColors[colors] = mixed;
    }
  }
  unsigned int displayColor = displayColors[color];
  
  unsigned r = (displayColor >> 16) & 0xff;
  unsigned g = (displayColor >> 8) & 0xff;
  unsigned b = displayColor & 0xff;
  
  return cv::Scalar(b,g,r);
}

