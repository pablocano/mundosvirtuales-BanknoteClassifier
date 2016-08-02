#include "Regionizer.h"
#include <opencv2/core/core.hpp>

MAKE_MODULE(Regionizer, GroundTruth)

void Regionizer::update(Regions& regions)
{
  regions.regions.clear();
  int depth = 0;
  for (int i = step; i < theImage.rows; i += step) {
    ColorModel::Colors color = theColorModel.getColor(theImage.at<cv::Vec3b>(i,step));
    Vector2<int> left = Vector2<int>(i, step);
    float movementPixels = theMovementImage.at<u_int8_t>(i,step) ? 1.f : 0.f;
    for (int j = 2*step; j < theImage.cols; j += step) {
      if (j >= theImage.cols - step) {
        if (left.y != j) {
          Vector2<int> right(i,j);
          if((color.is(white) && movementPixels/((right - left).y/step) > 0.7) || color.is(orange))
            regions.regions.push_back(Regions::Line(left,right,depth,color));
          movementPixels = 0;
        }
      }
      ColorModel::Colors color2 = theColorModel.getColor(theImage.at<cv::Vec3b>(i,j));
      movementPixels += theMovementImage.at<u_int8_t>(i,j) ? 1 : 0;
      if (color.colors != color2.colors) {
        Vector2<int> right;
        findRightBound(Vector2<int>(i, j), right, color);
        if((color.is(white) && movementPixels/((right - left).y/step) > 0.7) || color.is(orange))
          regions.regions.push_back(Regions::Line(left,right,depth,color));
        left = Vector2<int>(i,right.y+1);
        color = theColorModel.getColor(theImage.at<cv::Vec3b>(i,left.y));
        movementPixels = theMovementImage.at<u_int8_t>(i,j) ? 1.f : 0.f;
      }
    }
    depth++;
  }
}

void Regionizer::findRightBound(const Vector2<int>& initPoint, Vector2<int>& result, ColorModel::Colors color)
{
  for (int i = initPoint.y; i > initPoint.y - step; i--) {
    ColorModel::Colors color2 = theColorModel.getColor(theImage.at<cv::Vec3b>(initPoint.x,i));
    if (color.colors == color2.colors) {
      result = Vector2<int>(initPoint.x, i);
      return;
    }
  }
  result = Vector2<int>(initPoint.x, initPoint.y - step);
}
