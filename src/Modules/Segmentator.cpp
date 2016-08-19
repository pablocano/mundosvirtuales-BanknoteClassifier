

#include "Segmentator.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

MAKE_MODULE(Segmentator, Segmentation)

Segmentator* Segmentator::theInstance = 0;

Segmentator::Segmentator() : segment(false)
{
  theInstance = this;
}

void Segmentator::update(SegmentedImage &image)
{
  if (!segment) {
    return;
  }
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
}

void Segmentator::setSegmentation(bool set)
{
  if (theInstance) {
    theInstance->segment = set;
  }
}

