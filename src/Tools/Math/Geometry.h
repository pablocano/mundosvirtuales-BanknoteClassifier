#pragma once

#include "Tools/Math/Vector2.h"
#include <vector>

class Geometry
{
public:

    struct Polygon
    {
        std::vector<Vector2<int> > vertex;
    };


    static bool isPointInPolygon(const Polygon& polygon, const Vector2<int> point);

private:

    static int isLeft(const Vector2<int>);
};

