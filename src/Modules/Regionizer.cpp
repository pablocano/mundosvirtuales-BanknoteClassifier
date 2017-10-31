#include "Regionizer.h"
#include <opencv2/core/core.hpp>

MAKE_MODULE(Regionizer, BanknoteClassifier)

void Regionizer::update(Regions& regions)
{
  if(thePreviousBanknotePosition.banknote != Classification::NONE)
        return;

  regions.regions.clear();
  int depth = 0;
  for (int i = step; i < theImage.rows; i += step) {
    ColorModel::Colors color = theColorModel.getColor(theImage.at<cv::Vec3b>(i,step));
    Vector2i left = Vector2i(step,i);
    for (int j = 2*step; j < theImage.cols; j += step) {
      if (j >= theImage.cols - step) {
        if (left.x() != j) {
          Vector2i right(j,i);
          if(!color.is(white) && !color.is(none))
            regions.regions.push_back(Regions::Line(left,right,depth,color));
        }
      }
      ColorModel::Colors color2 = theColorModel.getColor(theImage.at<cv::Vec3b>(i,j));
      if (color.colors != color2.colors) {
        Vector2i right;
        findRightBound(Vector2i(j,i), right, color);
        if(!color.is(white) && !color.is(none))
          regions.regions.push_back(Regions::Line(left,right,depth,color));
        left = Vector2i(right.x()+1,i);
        color = theColorModel.getColor(theImage.at<cv::Vec3b>(i,left.x()));
      }
    }
    depth++;
  }
}

void Regionizer::findRightBound(const Vector2i &initPoint, Vector2i &result, ColorModel::Colors color)
{
  for (int i = initPoint.x(); i > initPoint.x() - step; i--) {
    ColorModel::Colors color2 = theColorModel.getColor(theImage.at<cv::Vec3b>(initPoint.y(),i));
    if (color.colors == color2.colors) {
      result = Vector2i(i,initPoint.y());
      return;
    }
  }
  result = Vector2i(initPoint.x() - step,initPoint.y());
}
