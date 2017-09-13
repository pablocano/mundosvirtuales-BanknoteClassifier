/**
 * @file Line.cpp
 * Implementacion de la clase linea.
 * @author Pablo Cano Montecinos.
 */

#include "Line.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

Line::Line(Vector2<int> point1, Vector2<int> point2): point1(point1), point2(point2)
{
    init = Vector2<>((float)point1.x,(float)point1.y);
    dir = (Vector2<>((float)point2.x,(float)point2.y) - init).normalize();
}

void Line::draw(cv::Mat &image, cv::Scalar color) const
{
    cv::line(image, cv::Point(point1.y,point1.x), cv::Point(point2.y,point2.x), color);
}
