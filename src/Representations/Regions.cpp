#include "Regions.h"
#include <opencv2/imgproc/imgproc.hpp>

void Regions::draw(cv::Mat &image) const
{
  for (auto& region : regions) {
    cv::Point pt1 = cv::Point(region.right.y, region.right.x);
    cv::Point pt2 = cv::Point(region.left.y, region.left.x);
    cv::line(image, pt1, pt2, DrawingColors::getDrawingColor(region.color.colors));
  }
}
