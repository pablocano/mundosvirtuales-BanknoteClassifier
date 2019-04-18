#include "CorrectorCamera.h"

MAKE_MODULE(CorrectorCamera, BanknoteCorrector)

CorrectorCamera::CorrectorCamera()
{
  video = cv::VideoCapture(1);
}

void CorrectorCamera::update(CorrectorImage &image)
{
  if(!video.isOpened())
    return;

  do{
    video >> image;
    if (image.empty()) {
      video >> image;
    }
  }
  while(image.empty());

  image.timeStamp = theCorrectorFrameInfo.time;
}
