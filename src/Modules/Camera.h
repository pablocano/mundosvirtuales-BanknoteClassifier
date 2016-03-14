
#pragma once
#include "Representations/Blackboard.h"
#include "Representations/Image.h"
#include "Representations/CameraInfo.h"
#include "Tools/ModuleManager/Module.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

MODULE(Camera)
    PROVIDES(CameraInfo)
    PROVIDES(ImageBGR)
    REQUIRES(ImageBGR)
    PROVIDES(Image)
END_MODULE


class Camera : public CameraBase
{
public:
	Camera();

    void update(CameraInfo *cameraInfo);
	
	void update(Image* image);

    void update(ImageBGR* image);
    cv::VideoCapture video0;
    cv::VideoCapture video1;
    cv::VideoCapture* cameras[2];
    int index;

    CameraInfo upper;
    CameraInfo lower;
    CameraInfo* camerasInfo[2];
};
