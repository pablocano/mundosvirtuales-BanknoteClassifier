
#pragma once
#include "Representations/Blackboard.h"
#include "Representations/Image.h"
#include "Tools/ModuleManager/Module.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

MODULE(Camera)
    PROVIDES(ImageBGR)
    REQUIRES(ImageBGR)
    PROVIDES(Image)
END_MODULE


class Camera : public CameraBase
{
public:
	Camera();
	
	void update(Image* image);

    void update(ImageBGR* image);
	cv::VideoCapture video;
};
