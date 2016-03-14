#pragma once
#include "Vector2.h"

//class CameraInfo;

class Transformation
{
public:
    static Vector2<> imageToField(const Vector2<> &positionInImage);
};
