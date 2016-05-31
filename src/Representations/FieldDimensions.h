#pragma once

#include "Tools/Vector2.h"
#include <opencv2/core/core.hpp>
#include <string>


class FieldDimensions
{
public:
    FieldDimensions();

    // draws the soccer field
    void draw(cv::Mat &image) const;

    // sets scale factor for drawing
    void setDrawingScale(float factor) { scale = factor; }

    // set image center
    void setDrawingCenter(int x, int y) { imgCenter.x = x; imgCenter.y = y; }


    // drawing stuff
    float scale;
    Vector2<int> imgCenter;


    // coordinates of the main elements in the field (in mm)
    int xLimit;             // x coordinate of the corner in the I quadrant
    int yLimit;             // y coordinate of the corner in the I quadrant
    int xPenaltyAreaLimit;  // x coordinate of the penalty area
    int yPenaltyAreaLimit;  // y coordinate of the penalty area
    int lineWidth;          // line width
    int circleRadius;       // radius of the central circle

    // the external corners of the field
    Vector2<int> p1;    // first quadrant:  x>0, y>0
    Vector2<int> p2;    // second quadrant: x<0, y>0
    Vector2<int> p3;    // third quadrant:  x<0, y<0
    Vector2<int> p4;    // fourth quadrant: x>0, y>0
    Vector2<int> p5;    // extra point x=0, y>0
    Vector2<int> p6;    // extra point x=0, y<0
};
