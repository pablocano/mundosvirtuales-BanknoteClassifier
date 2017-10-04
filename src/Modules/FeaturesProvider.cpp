#include "FeaturesProvider.h"

MAKE_MODULE(FeaturesProvider, BanknoteClassifier)

FeaturesProvider::FeaturesProvider()
{
    surf_ = cv::xfeatures2d::SURF::create(400,3,3,true,false);

}

void FeaturesProvider::update(Features &features)
{
    features.keypoints.clear();

    surf_->detectAndCompute(theGrayScaleImageEq,cv::noArray(),features.keypoints,features.descriptors);

}
