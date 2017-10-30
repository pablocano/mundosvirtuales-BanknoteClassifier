#pragma once
#include "Representations/Image.h"
#include "Representations/CameraInfo.h"
#include "Representations/FrameInfo.h"
#include "Tools/ModuleManager/Module.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <opencv2/highgui.hpp>

MODULE(Camera,
{,
  REQUIRES(FrameInfo),
  PROVIDES(CameraInfo),
  PROVIDES(ImageBGR),
  REQUIRES(ImageBGR),
  PROVIDES(Image),
  PROVIDES(GrayScaleImage),
});

/**
 * @brief The Camera class
 * @class Camera
 * @author Pablo Cano
 *
 * Provides the images from the camera and the calibration of the camera
 */
class Camera : public CameraBase
{
public:
    /**
    * @brief Camera
    * Constructor of the class
    */
    Camera();
  
    /**
     * @brief Update function of the camera info representation
     * @param cameraInfo The camera info to be updated
     */
    void update(CameraInfo& cameraInfo);
  
    /**
     * @brief Update function of the image in YCbCr
     * @param image The image to be updated
     * Updates the images in YCbCr using the image in BGR delivered by the camera
     */
    void update(Image& image);

    /**
     * @brief Update function of the gray scale image
     * @param grayScaleImage The image to be updated
     * Updates the images in grayscale using the image in BGR delivered by the camera
     */
    void update(GrayScaleImage& grayScaleImage);
  
    /**
     * @brief Update function of the BGR image
     * @param image The image in to be updated
     * Updates the original image delivered by the camera, but after is been corrected
     */
    void update(ImageBGR& image);

    /**
     * @brief Access to the first camera
     */
    cv::VideoCapture video0;

    /**
     * @brief Access to the second camera
     */
    cv::VideoCapture video1;

    /**
     * @brief Array of all the cameras avaliable
     */
    cv::VideoCapture* cameras[2];

    /**
     * @brief The number of avalaible cameras
     */
    int numCameras;

    /**
     * @brief Which camera is been used
     */
    int index;
  
    /**
     * @brief Camera information of the first camera
     */
    CameraInfo cam1;

    /**
     * @brief Camera information of the second camera
     */
    CameraInfo cam2;

    /**
     * @brief Array of all the camerasInfo
     */
    CameraInfo* camerasInfo[2];
  
    /**
    * @brief The height of the image to be delivered
    */
    int height;

    /**
     * @brief width The width of the image to be delivered
     */
    int width;
  
private:
  enum ANGLES { CLOCKWISE = 90, COUNTERCLOCKWISE = -90 };
  void rotateImage90(cv::Mat &src, cv::Mat &dst, int angle);
};
