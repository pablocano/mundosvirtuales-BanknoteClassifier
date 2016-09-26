/**
 * @file CameraInfo.h
 * Representation that saves the configuration information of each camera
 * @author Pablo Cano Montecinos
 * @author Matías Mattamala
 */

#pragma once
#include "Tools/Math/Vector2.h"
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>
#include <string>

class CameraInfo : public Streamable
{
public:
    enum Type{
        eastCam,
        westCam
    };

    CameraInfo() = default;
    CameraInfo(Type type, std::string name, cv::Mat& K, cv::Mat& d, cv::Point& fieldCenter, float factor) : type(type), available(true), name(name), K(K), d(d), pix2World(factor)
    {
        fieldCenterInImage = Vector2<float>(fieldCenter.x, fieldCenter.y);
        opticalCenter = Vector2<float>(K.at<double>(0,2), K.at<double>(1,2));
        fx = (float)K.at<double>(0,0);
        fy = (float)K.at<double>(1,1);
        invFx = 1.0 / fx;
        invFy = 1.0 / fy;

    }

    void operator=(const CameraInfo& other)
    {
        type = other.type;
        available = other.available;
        name = other.name;
        opticalCenter = other.opticalCenter;
        fieldCenterInImage = other.fieldCenterInImage;
        fx = other.fx;
        fy = other.fy;
        invFx = other.invFx;
        invFy = other.invFy;
        K = other.K;
        d = other.d;
        pix2World = other.pix2World;
    }

    void draw() const;

    // variables
    Type type;                      // Cam 1 or Cam 2
    bool available;                 // If the camera is connected or not
    std::string name;               // The name of the camera
    Vector2<> opticalCenter;        // Optical center of the camera (usually is the center of the image)
    Vector2<> fieldCenterInImage;   // The image coordinates of the field center
    float fx, fy;                   // Focal distance
    float invFx, invFy;             // Inverse Focal distance
    cv::Mat K;                      // Camera Calibration Matrix
    cv::Mat d;                      // Distortion Coefficients
    float pix2World;                // the pixel-to-mm factor
};
