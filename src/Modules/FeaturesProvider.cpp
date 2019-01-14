#include "FeaturesProvider.h"
#include "Tools/Math/Geometry.h"
#include <opencv2/highgui.hpp>

MAKE_MODULE(FeaturesProvider, BanknoteClassifier)

FeaturesProvider::FeaturesProvider()
{
#ifndef BC_WITH_CUDA
    surf_ = cv::xfeatures2d::SURF::create(400,4,3,true,false);
#else
    surf_ = cv::cuda::SURF_CUDA(100);
#endif
}

void FeaturesProvider::update(Features &features)
{
    if(thePreviousBanknotePosition.banknote != Classification::NONE)
        return;
    OUTPUT_TEXT("sacando features");

    features.keypoints.clear();

    if(mask.empty())
        mask = cv::Mat::zeros(theGrayScaleImageEq.rows, theGrayScaleImageEq.cols, CV_8U);
    else
        mask.setTo(cv::Scalar(0));

    if(theBestBlob.exists)
    {
        Vector2i leftUpper, rightLower;
        Geometry::calculateRect((theBestBlob.bestblob).borders, leftUpper, rightLower);

        int height, width;
        height = rightLower.x() - leftUpper.x();
        width = rightLower.y() - leftUpper.y();
        if(height < 200)
        {
            int diff = (200 - height)/2;
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

        if(width < 200)
        {
            int diff = (200 - width)/2;
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

#ifndef BC_WITH_CUDA
    surf_->detectAndCompute(theGrayScaleImageEq,mask,features.keypoints,features.descriptors);
#else
    cv::cuda::GpuMat grayScaleImageGpu(theGrayScaleImageEq);
    cv::cuda::GpuMat maskGpu(mask);
    //surf_(grayScaleImageGpu,maskGpu,features.keypointsGpu,features.descriptors);

    //surf_.downloadKeypoints(features.keypointsGpu,features.keypoints);
#endif

}
