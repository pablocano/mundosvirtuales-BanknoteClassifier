#include "Equalizer.h"
#include "Tools/Debugging/DebugImages.h"

MAKE_MODULE(Equalizer, BaslerCamera)

Equalizer::Equalizer()
{
  clahe = cv::cuda::createCLAHE(clipLimit, cv::Size(sizeWindows,sizeWindows));
}

void Equalizer::update(GpuGrayImageEq& image)
{
  if(theGpuGrayImage.empty())
    return;
#ifdef CALIBRATION_TOOL
  clahe = cv::cuda::createCLAHE(clipLimit, cv::Size(sizeWindows,sizeWindows));
#endif

  clahe->apply(theGpuGrayImage,image);

  COMPLEX_IMAGE("EqualizedImage")
  {
    CvMat debugImage;
    image.download(debugImage);
    SEND_DEBUG_IMAGE("EqualizedImage",debugImage);
  }
}
