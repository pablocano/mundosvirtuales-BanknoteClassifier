

#include "Segmentator.h"
#include "Tools/Debugging/Debugging.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

MAKE_MODULE(Segmentator, Segmentation)

void Segmentator::update(SegmentedImage &image)
{
  DEBUG_RESPONSE("representation:SegmentedImage",
  {
    cv::Mat empty = cv::Mat::zeros(theImage.size(),theImage.type());
    image = empty;
    for(int i = 0; i < theImage.rows; i++)
    {
      for(int j = 0; j < theImage.cols; j++)
      {
        Colors color = theColorModel.getColor(theImage.at<cv::Vec3b>(i,j));
        
        cv::Scalar paint = DrawingColors::getDrawingColor(color.colors);
        
        image.at<cv::Vec3b>(i,j) = cv::Vec3b(paint(0),paint(1),paint(2));
      }
    }
    image.timeStamp = theFrameInfo.time;
    OUTPUT(idJPEGImage, image);
  });
}

