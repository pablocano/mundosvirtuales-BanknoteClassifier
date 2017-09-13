#include "FeaturesProvider.h"

MAKE_MODULE(FeaturesProvider, BanknoteClassifier)

FeaturesProvider::FeaturesProvider()
{
    detector = cv::FastFeatureDetector::create();
    extractor = cv::xfeatures2d::FREAK::create();
}

void FeaturesProvider::update(Features &features)
{
    features.keypoints.clear();

    detector->detect(theGrayScaleImage,features.keypoints);

    extractor->compute(theGrayScaleImage, features.keypoints, features.descriptors);
}
