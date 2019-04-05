#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Representations/Classification.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Math/OpenCv.h"
#include <vector>
#include "Tools/Math/Eigen.h"
#include "Tools/Math/Pose2f.h"

STREAMABLE(BanknotePosition,
{
    ENUM(GraspZone,
    {,
        Left,
        Center,
        Right,
    });


    void getColorAndStyle(ColorRGBA& color, Drawings::PenStyle &style) const;

    virtual void draw() const;
    ,
    (Classification::Banknote)(Classification::NONE) banknote,
    (std::vector<Vector2f>) corners,
    (Pose2f) position,
    (CvMat) homography,
    (Vector2f) grabPos,
    (GraspZone) zone,
    (int)(0) needEstirator,
});


STREAMABLE_WITH_BASE(PreviousBanknotePosition, BanknotePosition,
{,
});

STREAMABLE_WITH_BASE(BanknotePositionFiltered, BanknotePosition,
{
    void draw() const;
    ,
    (int)(0) valid, //To check if is valid to send to the robot
});
