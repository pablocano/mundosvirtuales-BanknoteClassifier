#pragma once
#include "Representations/Image.h"
#include "Tools/Vector2.h"

class RobotPercept
{
public:

    struct Robot
    {
        Robot(const Vector2<int>& center, const Vector2<int> &leftUpper, const Vector2<int>& rightBottom, const Vector2<>& posInField) : center(center), leftUpper(leftUpper), rightBottom(rightBottom), posInField(posInField) {}
        Vector2<int> center;
        Vector2<int> leftUpper;
        Vector2<int> rightBottom;
        Vector2<> posInField;
    };

    void draw(cv::Mat& image) const;

    std::vector<Robot> robots;
};
