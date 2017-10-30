#include "FeaturesProvider.h"
#include "Tools/Math/Geometry.h"
#include <opencv2/highgui.hpp>
#include <iostream>

MAKE_MODULE(FeaturesProvider, BanknoteClassifier)

FeaturesProvider::FeaturesProvider()
{
    surf_ = cv::xfeatures2d::SURF::create(500,4,3,true,false);
}

void FeaturesProvider::update(Features &features)
{
    if(thePreviousBanknotePosition.banknote != Classification::NONE)
        return;

    features.keypoints.clear();

    if(mask.empty())
        mask = cv::Mat::zeros(theGrayScaleImageEq.rows, theGrayScaleImageEq.cols, CV_8U);
    else
        mask.setTo(cv::Scalar(0));

    if(!theBlobs.blobs.empty())
    {
        const Blobs::Blob& biggestBlob = theBlobs.blobs[0];
        Vector2<int> leftUpper, rightLower;
        Geometry::calculateRect(biggestBlob.borders, leftUpper, rightLower);
        mask(cv::Rect(leftUpper.x,leftUpper.y,rightLower.x - leftUpper.x,rightLower.y - leftUpper.y)) = 1;
    }

    surf_->detectAndCompute(theGrayScaleImageEq,mask,features.keypoints,features.descriptors);

}
