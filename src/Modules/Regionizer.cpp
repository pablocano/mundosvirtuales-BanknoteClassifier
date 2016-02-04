

#include "Regionizer.h"
#include <cv.h>
#include <highgui.h>

void Regionizer::update(Regions	*regions)
{
    regions->regions.clear();
	for (int i = step; i < theImage->rows; i += step) {
        ColorModel::Colors color = theColorModel->getColor(theImage->at<cv::Vec3b>(i,step));
        Vector2 left = Vector2(i, step);
		for (int j = 2*step; j < theImage->cols; j += step) {
            if (j >= theImage->cols - step) {
                if (left.y != j) {
                    Vector2 right(i,j);
                    regions->regions.push_back(Regions::Line(left,right,color));
                }
            }
            ColorModel::Colors color2 = theColorModel->getColor(theImage->at<cv::Vec3b>(i,j));
            if (color.colors != color2.colors) {
                Vector2 right;
                findRightBound(Vector2(i, j), right, color);
                regions->regions.push_back(Regions::Line(left,right,color));
                left = Vector2(i,right.y+1);
                color = theColorModel->getColor(theImage->at<cv::Vec3b>(i,left.y));
            }
		}
	}
}

void Regionizer::findRightBound(const Vector2 &initPoint, Vector2 &result, ColorModel::Colors color)
{
    for (int i = initPoint.y; i > initPoint.y - step; i--) {
        ColorModel::Colors color2 = theColorModel->getColor(theImage->at<cv::Vec3b>(initPoint.x,i));
        if (color.colors == color2.colors) {
            result = Vector2(initPoint.x, i);
            return;
        }
    }
    result = Vector2(initPoint.x, initPoint.y - step);
}