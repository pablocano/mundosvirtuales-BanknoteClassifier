#include "Camera.h"
#include "Tools/SystemCall.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>

MAKE_MODULE(Camera, Common)

Camera::Camera(): index(0),
cam1(CameraInfo::cam1, "Camera 1",Vector2<>(428.f,302.f),3300.f),
cam2(CameraInfo::cam2, "Camera 2",Vector2<>(52.f,317.f),3300.f) //video("GroundTruthVideo3.avi")//: video(0)
{
  // open first camera
  video0 = cv::VideoCapture(0);
  if(!video0.isOpened())  // check if we succeeded
  {
    cam1.available = false;
    std::cout << "Camera 1 is unavailable"<< std::endl;
  }
  else // the camera is available
  {
    std::cout << "Camera 1 was successfully opened"<< std::endl;
    video0.set(CV_CAP_PROP_FRAME_HEIGHT, settings.height);
    video0.set(CV_CAP_PROP_FRAME_WIDTH, settings.width);
  }
  
  // open second camera
  video1 = cv::VideoCapture(1);
  if(!video1.isOpened())  // check if we succeeded
  {
    cam2.available = false;
    std::cout << "Camera 2 is unavailable"<< std::endl;
  }
  else // the camera is available
  {
    std::cout << "The Camera 2 was successfully opened"<< std::endl;
    video1.set(CV_CAP_PROP_FRAME_HEIGHT, settings.height);
    video1.set(CV_CAP_PROP_FRAME_WIDTH, settings.width);
  }
  
  numCameras = (cam1.available? 1 : 0) + (cam2.available? 1 : 0);
  // std::cout << "numCameras: " << numCameras << std::endl;
  
  // fill the arrays
  cameras[0] = &video0;
  cameras[1] = &video1;
  camerasInfo[0] = &cam1;
  camerasInfo[1] = &cam2;
  last = SystemCall::getCurrentSystemTime();
}

void Camera::update(FrameInfo& frameInfo)
{
  frameInfo.time += SystemCall::getTimeSince(last);
  last = SystemCall::getCurrentSystemTime();
}

void Camera::update(CameraInfo& cameraInfo)
{
  index = (index + 1)%numCameras;
  cameraInfo = *camerasInfo[index];
}

void Camera::update(ImageBGR& image)
{
  *cameras[index] >> image;
  if (image.empty()) {
    //cameras[index]->set(CV_CAP_PROP_POS_AVI_RATIO , 0);
    *cameras[index] >> image;
  }
}

void Camera::update(Image& image)
{
  cv::cvtColor(theImageBGR, image, CV_BGR2YCrCb);
}
