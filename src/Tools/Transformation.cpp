#include "Transformation.h"

Vector2<> Transformation::imageToField(const Vector2<> &positionInImage)
{
    Vector2<> center(455.f,300.f);
    float factor = 3300.f/640.f;
    Vector2<> res = (center - positionInImage)*factor;
    return res;
}
