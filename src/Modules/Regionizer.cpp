

#include "Regionizer.h"
#include <cv.h>
#include <highgui.h>

void Regionizer::update(Regions	*regions)
{
	int step = 10;
	for (int i = step; i < theImage->rows; i += step) {
        ColorModel::Colors color = theColorModel->getColor(theImage->at<cv::Vec3b>(i,step));
        Vector2 left = Vector2(i, step);
		for (int j = 2*step; j < theImage->cols; j += step) {
            if (j == theImage->cols - 1) {
                if (left.y != j) {
                    Vector2 right(i,j);
                    regions->regions.push_back(Regions::Line(left,right));
                }
            }
            ColorModel::Colors color2 = theColorModel->getColor(theImage->at<cv::Vec3b>(i,j));
            if (color.colors != color2.colors) {
                Vector2 right;
                findLowerBound(Vector2(i, j), right);
                regions->regions.push_back(Regions::Line(left,right));
                right = Vector2(i,right.y+1);
            }
		}
	}
}

void Regionizer::findLowerBound(const Vector2 &initPoint, Vector2 &result)
{
    
}