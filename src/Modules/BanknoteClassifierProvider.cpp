#include "BanknoteClassifierProvider.h"
#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "Tools/Math/Transformation.h"
#include "Tools/Comm/Comm.h"

MAKE_MODULE(BanknoteClassifierProvider, BanknoteClassifier)


BanknoteClassifierProvider::BanknoteClassifierProvider()
{
    cv::FileStorage file( std::string(File::getGTDir())+"/Data/keypoints/ckp1000_Cara.yaml", cv::FileStorage::READ);

    cv::Mat descriptors;
    file["descriptors"] >> descriptors;

    int a = 0;
}

void BanknoteClassifierProvider::update(Classification &classification)
{
}

