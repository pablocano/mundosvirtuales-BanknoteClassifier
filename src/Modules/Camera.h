#pragma once
#include "Representations/Image.h"
#include "Representations/CameraInfo.h"
#include "Representations/FrameInfo.h"
#include "Tools/ModuleManager/Module.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

MODULE(Camera,
{,
  REQUIRES(FrameInfo),
  PROVIDES(CameraInfo),
  PROVIDES(ImageBGR),
  REQUIRES(ImageBGR),
  PROVIDES(Image),
  PROVIDES(GrayScaleImage),
});


class Camera : public CameraBase
{
public:
  Camera();
  
  void update(CameraInfo& cameraInfo);
  
  void update(Image& image);

  void update(GrayScaleImage& grayScaleImage);
  
  void update(ImageBGR& image);
  cv::VideoCapture video0;
  cv::VideoCapture video1;
  cv::VideoCapture* cameras[2];
  int numCameras;         // the number of available cameras
  int index;              // the current camera used
  
  CameraInfo cam1;        // the first camera
  CameraInfo cam2;        // the second camera
  CameraInfo* camerasInfo[2];
  
  int height;
  int width;
  
private:
  enum ANGLES { CLOCKWISE = 90, COUNTERCLOCKWISE = -90 };
  void rotateImage90(cv::Mat &src, cv::Mat &dst, int angle);
};
