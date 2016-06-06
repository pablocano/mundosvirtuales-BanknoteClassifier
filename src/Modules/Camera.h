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
    PROVIDES(FrameInfo),
    PROVIDES(CameraInfo),
    PROVIDES(ImageBGR),
    REQUIRES(ImageBGR),
    PROVIDES(Image),
});


class Camera : public CameraBase
{
    class Settings
    {
      public:
      Settings()
    {
      cv::FileStorage file(std::string(File::getGTDir())+"/Config/cameraConfig.xml", cv::FileStorage::READ);
        if(!file.isOpened())
        {
          std::cout << "Could not open the camera configuration file"<< std::endl;
        }
        file["imgWidth" ] >> width;
        file["imgHeight"] >> height;
        file.release();
    }
      int height;
      int width;
    };

public:
	Camera();

    void update(FrameInfo& frameInfo);

    void update(CameraInfo& cameraInfo);
	
    void update(Image& image);

    void update(ImageBGR& image);
    cv::VideoCapture video0;
    cv::VideoCapture video1;
    cv::VideoCapture* cameras[2];
    int numCameras;         // the number of available cameras
    int index;              // the current camera used

    CameraInfo cam1;        // the first camera
    CameraInfo cam2;        // the second camera
    CameraInfo* camerasInfo[2];
    Settings settings;

    unsigned last;
};
