/**
 * @file Line.cpp
 * Implementacion de la clase linea.
 * @author Pablo Cano Montecinos.
 */

#include "Line.h"

Line::Line(Vector2 point1, Vector2 point2): point1(point1), point2(point2)
{
    m = (point2.y - point1.y)/(point2.x - point1.x);
    n = point2.y - m*point2.x;
}

void Line::draw(cv::Mat &image, cv::Scalar color, float x1, float x2)
{
    float y1,y2;
    
    y1 = evaluate(x1);
    y2 = evaluate(x2);
    
    cv::line(image, cv::Point(x1,y1), cv::Point(x2,y2), color);
}

void Line::draw(cv::Mat &image, cv::Scalar color)
{
    cv::line(image, cv::Point(point1.x,point1.y), cv::Point(point2.x,point2.y), color);
}

bool Line::isDown(Vector2 point)
{
    return evaluate(point.x) < point.y;
}

float Line::evaluate(float x)
{
    return m*x + n;
}