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
}
