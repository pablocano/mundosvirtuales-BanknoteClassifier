#include "FeaturesProvider.h"
#include "Tools/Math/Geometry.h"
#include <opencv2/highgui.hpp>
#include <iostream>

MAKE_MODULE(FeaturesProvider, BanknoteClassifier)

FeaturesProvider::FeaturesProvider()
{
    surf_ = cv::xfeatures2d::SURF::create(400,3,3,true,false);
}

void FeaturesProvider::update(Features &features)
{
    if(thePreviousBanknotePosition.banknote != Classification::NONE)
        return;
    std::cout<<"sacando features"<<std::endl;

    features.keypoints.clear();

    if(mask.empty())
        mask = cv::Mat::zeros(theGrayScaleImageEq.rows, theGrayScaleImageEq.cols, CV_8U);
    else
        mask.setTo(cv::Scalar(0));

    if(theBestBlob.exists)
    {
        Vector2i leftUpper, rightLower;
        Geometry::calculateRect((theBestBlob.bestblob).borders, leftUpper, rightLower);
        mask(cv::Rect(leftUpper.x(),leftUpper.y(),rightLower.x() - leftUpper.x(),rightLower.y() - leftUpper.y())) = 1;
    }

    surf_->detectAndCompute(theGrayScaleImageEq,mask,features.keypoints,features.descriptors);

}
