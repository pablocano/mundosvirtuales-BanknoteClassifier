#pragma once

#include "Tools/Math/Eigen.h"
#include <opencv2/core.hpp>
#include <vector>

class Geometry
{
public:

    struct Polygon
    {
        std::vector<Vector2i > vertex;
    };


    static bool isPointInPolygon(const Polygon& polygon, const Vector2i point);

    /**
     * @brief isLeft tests if a point is Left|On|Right of an infinite line.
     * @param point1 first point of a line
     * @param point2 second point of a line
     * @param testPoint the point to test
     * @return >0 for testPoint left of the line through point1 and point2
     *         =0 for testPoint on the line
     *         <0 for testPoint right of the line
     */
    static int isLeft(const Vector2i &point1, const Vector2i &point2, const Vector2i &testPoint);

    /**
     * @brief polygonArea Calculates the area of the polygon
     * @param polygon the Polygon which area is going to be calculated
     * @return the area of the polygon
     */
    static float polygonArea(Polygon &polygon);

    /**
     * @brief calculateRect calculate the boundary rect of a list of points
     * @param points the list of points
     * @param leftUpper the left upper corner of the rect
     * @param rightLower the right lower corner of the rect
     */
    static void calculateRect(const std::vector<Vector2i >& points, Vector2i& leftUpper, Vector2i& rightLower);

    /**
     * @brief calculateRect calculate the boundary rect of a list of points
     * @param points the list of points
     * @param leftUpper the left upper corner of the rect
     * @param rightLower the right lower corner of the rect
     */
    static void calculateRect(const std::vector<Vector2f>& points, Vector2i& leftUpper, Vector2i& rightLower);
};

