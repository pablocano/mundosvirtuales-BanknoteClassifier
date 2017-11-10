#pragma once

#include "Tools/Streamable.h"
#include "Representations/Classification.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/core.hpp>
#include <vector>
#include "Tools/Math/Eigen.h"

class BanknotePosition : public Streamable
{
public:

    BanknotePosition() : banknote(Classification::NONE){}

    void getColorAndStyle(ColorRGBA& color, Drawings::PenStyle &style) const;

    virtual void draw() const;

    Classification::Banknote banknote;

    std::vector<Vector2f> corners;

    cv::Mat homography;


};

class PreviousBanknotePosition : public BanknotePosition
{};

class BanknotePositionFiltered : public BanknotePosition
{
public:
    void draw() const;
};
