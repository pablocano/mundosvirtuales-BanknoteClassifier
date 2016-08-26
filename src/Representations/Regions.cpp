#include "Regions.h"
#include <opencv2/imgproc/imgproc.hpp>

void Regions::draw(cv::Mat &image) const
{
  for (auto& region : regions) {
    cv::Point pt1 = cv::Point(region.right.x, region.right.y);
    cv::Point pt2 = cv::Point(region.left.x, region.left.y);
    cv::line(image, pt1, pt2, DrawingColors::getDrawingColor(region.color.colors));
  }
}
