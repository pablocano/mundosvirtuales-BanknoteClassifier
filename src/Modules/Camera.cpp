

#include "Camera.h"


Camera::Camera(): video(0)
{
	video.set(CV_CAP_PROP_FRAME_HEIGHT, 180);
	video.set(CV_CAP_PROP_FRAME_WIDTH, 270);
}

void Camera::update(Image *image)
{
	video >> *image;
}

