

#include "Camera.h"
#include <opencv2/imgproc/imgproc.hpp>

Camera::Camera(): video(0)//:video("GroundTruthVideo3.avi")//: video(0)
{
    //video.set(CV_CAP_PROP_FRAME_HEIGHT, 180);
    //video.set(CV_CAP_PROP_FRAME_WIDTH, 270);
}

void Camera::update(ImageBGR *image)
{
	video >> *image;
	if (image->empty()) {
		video.set(CV_CAP_PROP_POS_AVI_RATIO , 0);
		video >> *image;
	}
    //*image = cv::imread("team.jpg", CV_LOAD_IMAGE_COLOR);;
}

void Camera::update(Image *image)
{
    cv::cvtColor(*theImageBGR, *image, CV_BGR2YCrCb);
}

/*el pablo es un gordo*/
/*muy gordo*/
/*se comio todos los tuits*/
