#pragma once

#include "Tools/Streamable.h"
#include "Tools/Math/Eigen.h"

class GrabbingPosition : public Streamable
{
public:

    Vector2f point;

    void draw() const;

};
