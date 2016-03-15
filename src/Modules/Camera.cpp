#include "Camera.h"
#include <opencv2/imgproc/imgproc.hpp>

Camera::Camera(): video0(0),video1(1),index(0),
    upper(CameraInfo::upper, "Camera 1",Vector2<>(455.f,300.f),3300.f),
    lower(CameraInfo::lower, "Camera 2",Vector2<>(10.f,300.f),3300.f) //video("GroundTruthVideo3.avi")//: video(0)
{
    cameras[0] = &video0;
    cameras[1] = &video1;
    camerasInfo[0] = &upper;
    camerasInfo[1] = &lower;
    last = std::chrono::steady_clock::now();
    //video.set(CV_CAP_PROP_FRAME_HEIGHT, 180);
    //video.set(CV_CAP_PROP_FRAME_WIDTH, 270);
}

void Camera::update(FrameInfo *frameInfo)
{
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
    last = now;
    frameInfo->time += int(int_ms.count());
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
        cameras[index]->set(CV_CAP_PROP_POS_AVI_RATIO , 0);
        *cameras[index] >> *image;
	}
}

void Camera::update(Image *image)
{
    cv::cvtColor(*theImageBGR, *image, CV_BGR2YCrCb);
}
