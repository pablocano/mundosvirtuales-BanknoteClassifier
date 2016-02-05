
#pragma once
#include "Representations/Blackboard.h"
#include "Representations/Image.h"
#include "Tools/ModuleManager/Module.h"
#include <cv.h>
#include <highgui.h>

MODULE(Camera)
	PROVIDES(Image)
END_MODULE


class Camera : public CameraBase
{
public:
	Camera();
	
	void update(Image* image);
	//cv::VideoCapture video;
};
