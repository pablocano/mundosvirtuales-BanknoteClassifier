#include "Camera.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>

Camera::Camera(): index(1),
    cam1(CameraInfo::cam1, "Camera 1",Vector2<>(428.f,302.f),3300.f),
    cam2(CameraInfo::cam2, "Camera 2",Vector2<>(52.f,317.f),3300.f) //video("GroundTruthVideo3.avi")//: video(0)
{
    // Read image configuration
    cv::FileStorage file("../../Config/cameraConfig.xml", cv::FileStorage::READ);
    if(!file.isOpened())
    {
      std::cout << "Could not open the camera configuration file"<< std::endl;
    }
    file["imgWidth" ] >> width;
    file["imgHeight"] >> height;
    file.release();


    // open first camera
    video0 = cv::VideoCapture(2);
    if(!video0.isOpened())  // check if we succeeded
    {
        cam1.available = false;
        std::cout << "Camera 1 is unavailable"<< std::endl;
    }
    else // the camera is available
    {
        std::cout << "Camera 1 was successfully opened"<< std::endl;
        cam1.available = true;
        video0.set(CV_CAP_PROP_FRAME_HEIGHT, height);
        video0.set(CV_CAP_PROP_FRAME_WIDTH, width);
    }

    // open second camera
    video1 = cv::VideoCapture(3);
    if(!video1.isOpened())  // check if we succeeded
    {
        cam2.available = false;
        std::cout << "Camera 2 is unavailable"<< std::endl;
    }
    else // the camera is available
    {
        std::cout << "Camera 2 was successfully opened"<< std::endl;
        cam2.available = true;
        video1.set(CV_CAP_PROP_FRAME_HEIGHT, height);
        video1.set(CV_CAP_PROP_FRAME_WIDTH, width);
    }

    numCameras = (cam1.available? 1 : 0) + (cam2.available? 1 : 0);
    // std::cout << "numCameras: " << numCameras << std::endl;

    // fill the arrays
    cameras[0] = &video0;
    cameras[1] = &video1;
    camerasInfo[0] = &cam1;
    camerasInfo[1] = &cam2;
    last = std::chrono::steady_clock::now();
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
    index = (index + 1)%numCameras;
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
