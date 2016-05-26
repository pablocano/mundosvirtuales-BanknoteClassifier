#pragma once
#include "Tools/Vector2.h"
#include <opencv2/core/core.hpp>
#include <string>

class CameraInfo
{
public:
    enum Type{
        cam1,
        cam2
    };

    CameraInfo() = default;
    CameraInfo(Type type, std::string name, Vector2<> position,float size) : type(type), name(name), position(position), factor(size/640.f)
    {}

    void operator=(const CameraInfo& other)
    {
        type = other.type;
        name = other.name;
        position = other.position;
        factor = other.factor;
    }

    void draw(cv::Mat& image) const;

    // variables
    Type type;              // Cam 1 or Cam 2
    bool available;         // If the camera is connected or not
    std::string name;       // The name of the camera
    Vector2<> position;     // The position (?)
    float factor;           // A temporal correcting factor
};
