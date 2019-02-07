#pragma once

#include "Tools/Streams/Streamable.h"
#include "Representations/Classification.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/core.hpp>
#include <vector>
#include "Tools/Math/Eigen.h"
#include "Tools/Math/Pose2D.h"

class BanknotePosition : public Streamable
{
public:

    BanknotePosition() : banknote(Classification::NONE){}

    void getColorAndStyle(ColorRGBA& color, Drawings::PenStyle &style) const;

    virtual void draw() const;

    Classification::Banknote banknote;

    std::vector<Vector2f> corners;

    Pose2D position;

    cv::Mat homography;

    Vector2f grabPos;


};

class PreviousBanknotePosition : public BanknotePosition
{};

class BanknotePositionFiltered : public BanknotePosition
{
public:
    void draw() const;

    BanknotePositionFiltered():BanknotePosition(),valid(0){}

    //To check if is valid to send to the robot
    int valid;
};
