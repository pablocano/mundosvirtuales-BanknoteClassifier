#pragma once
#include "Representations/Image.h"
#include "Representations/CameraInfo.h"
#include "Representations/FrameInfo.h"
#include "Tools/ModuleManager/Module.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <opencv2/highgui.hpp>
#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>

MODULE(Camera,
{,
  REQUIRES(FrameInfo),
  PROVIDES(CameraInfo),
  PROVIDES(Image),
  REQUIRES(Image),
  PROVIDES(GrayScaleImage),
  PROVIDES(ImageBGR),
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
    * @brief Destructor
    * Destructor of the class
    */
    ~Camera();

    static Camera *theInstance;
  
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

	void update(ImageBGR& theImageBGR);
  
    /**
     * @brief currentImage
     */
    ImageBGR currentImage;

    ImageBGR grabbedImageBRG;
  
    /**
    * @brief The height of the image to be delivered
    */
    int height;

    /**
     * @brief width The width of the image to be delivered
     */
    int width;

    //Pylon::CBaslerUsbInstantCamera *camera;
    Pylon::CInstantCamera *camera;
    Pylon::CImageFormatConverter *fc;

    Pylon::CPylonImage *grabbedImage;

    CameraInfo info;

    bool cameraLoaded;

    static CameraInfo& getCameraInfo();
};
