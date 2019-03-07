#include "FeaturesProvider.h"
#include "Tools/Math/Geometry.h"
#include <opencv2/highgui.hpp>

MAKE_MODULE(FeaturesProvider, BanknoteClassifier)

FeaturesProvider::FeaturesProvider()
{
  surf = cv::cuda::SURF_CUDA(hessianThreshold, nOctaves, nOctaveLayers, extended);
}

void FeaturesProvider::update(Features &features)
{
  if(mask.empty())
  {
    cv::Mat temp = cv::Mat::zeros(theGpuGrayImageEq.rows,theGpuGrayImageEq.cols,CV_8U);
    temp(cv::Rect(maskX,maskY,maskWidth,maskHeight)) = 1;
    mask.upload(temp);
  }

#ifdef CALIBRATION_TOOL
  cv::Mat temp = cv::Mat::zeros(theGpuGrayImageEq.rows,theGpuGrayImageEq.cols,CV_8U);
  temp(cv::Rect(maskX,maskY,maskWidth,maskHeight)) = 1;
  mask.upload(temp);
#endif

  surf(theGpuGrayImageEq,mask,features.keypointsGpu,features.descriptors);

  surf.downloadKeypoints(features.keypointsGpu, features.keypoints);
}
