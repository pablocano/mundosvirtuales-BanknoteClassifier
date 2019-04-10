#include "GrayImageProvider.h"
#include <opencv2/cudaimgproc.hpp>

MAKE_MODULE(GrayImageProvider, BaslerCamera)

void GrayImageProvider::update(GpuGrayImage &image)
{
  if(theImage.empty())
    return;

  if(lastImageRecived == theImage.timeStamp)
    return;

  lastImageRecived = theImage.timeStamp;

  cv::cuda::GpuMat gpuImage;
  gpuImage.upload(theImage);
  cv::cuda::cvtColor(gpuImage,image,cv::COLOR_BGR2GRAY);
}
