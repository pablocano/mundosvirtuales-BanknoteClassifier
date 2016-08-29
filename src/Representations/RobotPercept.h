#pragma once
#include "Representations/Image.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Streamable.h"

class RobotPercept : public Streamable
{
public:

    struct Robot
    {
        Robot(const Vector2<int>& centerInImage, const Vector2<int> &leftUpper, const Vector2<int>& rightBottom, const Vector2<int>& center) : centerInImage(centerInImage), leftUpper(leftUpper), rightBottom(rightBottom), center(center) {}
        Vector2<int> centerInImage;
        Vector2<int> leftUpper;
        Vector2<int> rightBottom;
        Vector2<int> center;
    };

    void draw(cv::Mat& image) const;

    std::vector<Robot> robots;
};
