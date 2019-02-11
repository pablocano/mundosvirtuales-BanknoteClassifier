/**
 * @file CameraInfo.h
 * Representation that saves the configuration information of each camera
 * @author Pablo Cano Montecinos
 * @author Matías Mattamala
 */

#pragma once
#include "Tools/Math/Vector2.h"
#include "Tools/Streams/Enum.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Math/OpenCv.h"
#include "Tools/Streams/AutoStreamable.h"
#include "Representations/Image.h"
#include <opencv2/core/core.hpp>
#include <string>

STREAMABLE(CameraInfo,
{
    ENUM(CameraType,
    {,
     eastCam,
     westCam,
    });

    CameraInfo() = default;

    CameraInfo(CameraType type, std::string name, cv::Mat& K, cv::Mat& d, Vector2f& fieldCenter, float factor);

    void operator=(const CameraInfo& other);

    void draw() const;
    ,
    (CameraType) type,
    (bool)(true) available,                 // If the camera is connected or not
    (std::string) name,             // The name of the camera
    (Vector2f) opticalCenter,        // Optical center of the camera (usually is the center of the image)
    (Vector2f) fieldCenterInImage,   // The image coordinates of the field center
    (float) fx,
    (float) fy,                   // Focal distance
    (float) invFx,
    (float) invFy,             // Inverse Focal distance
    (CvMat) K,                      // Camera Calibration Matrix
    (CvMat) d,                      // Distortion Coefficients
    (float) pix2World,
});
