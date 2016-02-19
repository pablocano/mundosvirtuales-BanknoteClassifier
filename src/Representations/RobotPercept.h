#pragma once
#include "Representations/Image.h"
#include "Tools/Vector2.h"

class RobotPercept
{
public:

    struct Robot
    {
        Robot(const Vector2<int>& center, const Vector2<int> &leftUpper, const Vector2<int>& rightBottom) : center(center), leftUpper(leftUpper), rightBottom(rightBottom) {}
        Vector2<int> center;
        Vector2<int> leftUpper;
        Vector2<int> rightBottom;
    };

    void draw(ImageBGR* image) const;

    std::vector<Robot> robots;
};
