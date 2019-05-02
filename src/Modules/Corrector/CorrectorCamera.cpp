#include "CorrectorCamera.h"
#include "Platform/Time.h"
#include "Platform/Linux/V4lCamera.h"


#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

MAKE_MODULE(CorrectorCamera, BanknoteCorrector)

thread_local CorrectorCamera* CorrectorCamera::theInstance = nullptr;

CorrectorCamera::CorrectorCamera()
{
  theInstance = this;
  setupCameras();

  thread.start(this, &CorrectorCamera::takeImages);
  takeNextImage.post();
  imageTaken.wait();
}

CorrectorCamera::~CorrectorCamera()
{
  thread.announceStop();
  takeNextImage.post();
  thread.stop();

  if(camera)
  {
    delete camera;
    camera = nullptr;
  }

  theInstance = nullptr;
}

void CorrectorCamera::update(CorrectorImage &image)
{
  if(camera->hasImage())
  {
    OUTPUT_TEXT("----- New Image -----");
    useImage(std::max(lastImageTimeStamp + 1, static_cast<unsigned>(camera->getTimeStamp() / 1000) - Time::getSystemTimeBase()),image);
    lastImageTimeStamp = image.timeStamp;
    OUTPUT_TEXT(lastImageTimeStamp);
  }
}

void CorrectorCamera::useImage(unsigned int timestamp, CorrectorImage &image)
{
  image = cv::Mat(1080,1920, CV_8UC2, (void*)camera->getImage());
  image.timeStamp = timestamp;
  lastImageTimeStampLL = camera->getTimeStamp();

  cv::cvtColor(image,image, cv::COLOR_YUV2BGR_YUYV);
}

void CorrectorCamera::setupCameras()
{
  camera = new V4lCamera("/dev/video0", theCorrectorCameraSettings);
}

bool CorrectorCamera::isFrameDataComplete()
{
  if(theInstance)
    return theInstance->camera->hasImage();
  else
    return true;
}

void CorrectorCamera::takeImages()
{
  BH_TRACE_INIT("CameraProvider");
  Thread::nameThread("CameraProvider");
  //thread.setPriority(11);
  while(thread.isRunning())
  {
    takeNextImage.wait();

    if(camera)
    {
      camera->releaseImage();
    }

    while(!camera->hasImage())
    {
      camera->captureNew();

      if(camera->hasImage() && camera->getTimeStamp() < lastImageTimeStampLL)
        camera->releaseImage();
    }

    imageTaken.post();
  }
}

void CorrectorCamera::waitForFrameData()
{
  if(theInstance)
  {
    theInstance->takeNextImage.post();
    theInstance->imageTaken.wait();
  }
}



