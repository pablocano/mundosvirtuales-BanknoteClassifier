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

MODULE(CameraProvider,
{,
  REQUIRES(FrameInfo),
  PROVIDES(CameraInfo),
  PROVIDES(Image),
});

/**
 * @brief The Camera class
 * @class Camera
 * @author Pablo Cano
 *
 * Provides the images from the camera and the calibration of the camera
 */
class CameraProvider : public CameraProviderBase
{
public:
    /**
    * @brief CameraProvider
    * Constructor of the class
    */
    CameraProvider();

    /**
    * @brief Destructor
    * Destructor of the class
    */
    ~CameraProvider();

    static thread_local CameraProvider *theInstance;
  
    /**
     * @brief Update function of the camera info representation
     * @param cameraInfo The camera info to be updated
     */
    void update(CameraInfo& cameraInfo);
  
    /**
     * @brief Update function of the image in BGR
     * @param image The image to be updated
     * Updates the image using the one delivered by the camera
     */
    void update(Image& image);
  
    /**
     * @brief currentImage
     */
    Image currentImage;

    Image grabbedImageBRG;
  
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
