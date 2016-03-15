#include "Transformation.h"
#include "Representations/CameraInfo.h"

Vector2<> Transformation::imageToField(const Vector2<> &positionInImage, const CameraInfo& cameraInfo)
{
    Vector2<> res = (cameraInfo.position - positionInImage)*cameraInfo.factor;
    return res;
}
