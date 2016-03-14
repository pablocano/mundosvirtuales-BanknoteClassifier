#pragma once
#include "Tools/Vector2.h"
#include <string>

class CameraInfo
{
public:
    enum Type{
        upper,
        lower
    };

    CameraInfo() = default;
    CameraInfo(Type type, std::string name, Vector2<> position) : type(type), name(name), position(position)
    {}

    void operator=(const CameraInfo& other)
    {
        type = other.type;
        name = other.name;
        position = other.position;
    }
    Type type;
    std::string name;
    Vector2<> position;
};
