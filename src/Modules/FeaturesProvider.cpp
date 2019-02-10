#include "FeaturesProvider.h"
#include "Tools/Math/Geometry.h"
#include <opencv2/highgui.hpp>

MAKE_MODULE(FeaturesProvider, BanknoteClassifier)

FeaturesProvider::FeaturesProvider()
{
#ifndef BC_WITH_CUDA
    surf_ = cv::xfeatures2d::SURF::create(400,4,3,true,false);
#else
    surf_ = cv::cuda::SURF_CUDA(600,4,4,true);
#endif
}

void FeaturesProvider::update(Features &features)
{
    if(thePreviousBanknotePosition.banknote != Classification::NONE)
        return;
    OUTPUT_TEXT("Extracting features");

    if(mask.empty())
        mask = cv::Mat::zeros(theGrayScaleImageEq.rows, theGrayScaleImageEq.cols, CV_8U);

    cv::cuda::GpuMat grayScaleImageGpu(theGrayScaleImageEq);
}

void FeaturesProvider::setMask(const Blobs::Blob& blob)
{
    mask.setTo(cv::Scalar(0));
    Vector2i leftUpper, rightLower;
    Geometry::calculateRect(blob.borders, leftUpper, rightLower);

    int height, width;
    height = rightLower.x() - leftUpper.x();
    width = rightLower.y() - leftUpper.y();
    if(height < 400)
    {
        int diff = (400 - height)/2;
        if(leftUpper.x() - diff < 0)
        {
            diff = diff - leftUpper.x();
            leftUpper.x() = 0;
            rightLower.x() += diff;
        }
        else if(rightLower.x() + diff > theGrayScaleImageEq.cols)
        {
            diff = theGrayScaleImageEq.cols - rightLower.x();
            rightLower.x() = theGrayScaleImageEq.cols -1;
            leftUpper.x() -= diff;
        }
        else
        {
            leftUpper.x() -= diff;
            rightLower.x() += diff;
        }
    }

    if(width < 400)
    {
        int diff = (400 - width)/2;
        if(leftUpper.y() - diff < 0)
        {
            diff = diff - leftUpper.y();
            leftUpper.y() = 0;
            rightLower.y() += diff;
        }
        else if(rightLower.y() + diff > theGrayScaleImageEq.rows)
        {
            diff = theGrayScaleImageEq.rows - rightLower.y();
            rightLower.y() = theGrayScaleImageEq.rows -1;
            leftUpper.y() -= diff;
        }
        else
        {
            leftUpper.y() -= diff;
            rightLower.y() += diff;
        }
    }

    mask(cv::Rect(leftUpper.x(),leftUpper.y(),rightLower.x() - leftUpper.x(),rightLower.y() - leftUpper.y())) = 1;
}
