

#include "Segmentator.h"
#include <cv.h>
#include <highgui.h>

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
            
            if (color.is(ColorModel::none)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(128,128,128);
            }
            else if (color.is(ColorModel::white)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(255,255,255);
            }
            else if (color.is(ColorModel::green)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,0);
            }
            else if (color.is(ColorModel::blue)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(255,0,0);
            }
            else if (color.is(ColorModel::red)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,255);
            }
            else if (color.is(ColorModel::orange)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,128,255);
            }
            else if (color.is(ColorModel::yellow)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,255);
            }
            else if (color.is(ColorModel::black)) {
                image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
            }
        }
    }
}

