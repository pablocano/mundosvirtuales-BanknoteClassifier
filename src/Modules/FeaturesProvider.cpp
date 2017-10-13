#include "FeaturesProvider.h"
#include <opencv2/highgui.hpp>

MAKE_MODULE(FeaturesProvider, BanknoteClassifier)

FeaturesProvider::FeaturesProvider()
{
    surf_ = cv::xfeatures2d::SURF::create(500,4,3,true,false);
}

void FeaturesProvider::update(Features &features)
{
    features.keypoints.clear();

    if(mask.empty())
        mask = cv::Mat::zeros(theGrayScaleImageEq.rows, theGrayScaleImageEq.cols, CV_8U);
    else
        mask.setTo(cv::Scalar(0));

    if(!theBlobs.blobs.empty())
    {
        auxPolygon.clear();
        for(const auto& vertex : theBlobs.blobs[0].borders)
        {
            auxPolygon.push_back(cv::Point(vertex.x, vertex.y));
        }
        cv::fillConvexPoly( mask, &auxPolygon[0], auxPolygon.size(), cv::Scalar(255) );
    }

    surf_->detectAndCompute(theGrayScaleImageEq,mask,features.keypoints,features.descriptors);

}
