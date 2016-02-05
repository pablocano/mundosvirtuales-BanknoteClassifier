

#include "Camera.h"


Camera::Camera() :video("GroundTruthVideo3.avi")//: video(0)
{
	//video.set(CV_CAP_PROP_FRAME_HEIGHT, 180);
	//video.set(CV_CAP_PROP_FRAME_WIDTH, 270);
}

void Camera::update(Image *image)
{
	video >> *image;
	if (image->empty()) {
		video.set(CV_CAP_PROP_POS_AVI_RATIO , 0);
		video >> *image;
	}
    //*image = cv::imread("team.jpg", CV_LOAD_IMAGE_COLOR);;
}

