

#include "Segmentator.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

MAKE_MODULE(Segmentator, Segmentation)

void Segmentator::update(SegmentedImage &image)
{
    cv::Mat empty = cv::Mat::zeros(theImage.size(),theImage.type());
    image = empty;
    for(int i = 0; i < theImage.rows; i++)
    {
        for(int j = 0; j < theImage.cols; j++)
        {
            Colors color = theColorModel.getColor(theImage.at<cv::Vec3b>(i,j));
            
            if (color.is(none)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(128,128,128);
            }
            else if (color.is(white)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(255,255,255);
            }
            else if (color.is(green)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,0);
            }
            else if (color.is(blue)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(255,0,0);
            }
            else if (color.is(red)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,255);
            }
            else if (color.is(orange)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,128,255);
            }
            else if (color.is(yellow)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,255);
            }
            else if (color.is(black)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
            }
        }
    }
}

