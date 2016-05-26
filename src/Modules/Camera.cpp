#include "Camera.h"
#include "Tools/SystemCall.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>

Camera::Camera(): video0(0),video1(1),index(0),
upper(CameraInfo::upper, "Camera 1",Vector2<>(428.f,302.f),3300.f),
lower(CameraInfo::lower, "Camera 2",Vector2<>(52.f,317.f),3300.f) //video("GroundTruthVideo3.avi")//: video(0)
{
  cameras[0] = &video0;
  cameras[1] = &video1;
  camerasInfo[0] = &upper;
  camerasInfo[1] = &lower;
  last = SystemCall::getCurrentSystemTime();
  video0.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  video0.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  video1.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  video1.set(CV_CAP_PROP_FRAME_WIDTH, 640);
}

void Camera::update(FrameInfo *frameInfo)
{
  frameInfo->time += SystemCall::getTimeSince(last);
  last = SystemCall::getCurrentSystemTime();
}

void Camera::update(CameraInfo *cameraInfo)
{
  index = (index + 1)%2;
  *cameraInfo = *camerasInfo[index];
}

void Camera::update(ImageBGR *image)
{
  *cameras[index] >> *image;
  if (image->empty()) {
    //cameras[index]->set(CV_CAP_PROP_POS_AVI_RATIO , 0);
    *cameras[index] >> *image;
  }
}

void Camera::update(Image *image)
{
  cv::cvtColor(*theImageBGR, *image, CV_BGR2YCrCb);
}
