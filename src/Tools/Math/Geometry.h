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

    /**
     * @brief isLeft tests if a point is Left|On|Right of an infinite line.
     * @param point1 first point of a line
     * @param point2 second point of a line
     * @param testPoint the point to test
     * @return >0 for testPoint left of the line through point1 and point2
     *         =0 for testPoint on the line
     *         <0 for testPoint right of the line
     */
    static int isLeft(const Vector2<int> &point1, const Vector2<int> &point2, const Vector2<int> &testPoint);

    /**
     * @brief polygonArea Calculates the area of the polygon
     * @param polygon the Polygon which area is going to be calculated
     * @return the area of the polygon
     */
    static float polygonArea(Polygon &polygon);
};

