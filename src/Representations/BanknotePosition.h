#pragma once

#include "Tools/Streamable.h"
#include "Representations/Classification.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/core.hpp>
#include <vector>

class BanknotePosition : public Streamable
{
public:

    BanknotePosition() : banknote(Classification::NONE){}

    void getColorAndStyle(ColorRGBA& color, Drawings::PenStyle &style) const;

    void draw() const;

    Classification::Banknote banknote;

    std::vector<cv::Point2f> corners;
};

class PreviousBanknotePosition : public BanknotePosition
{};
