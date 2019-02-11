#pragma once
#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Math/OpenCv.h"
STREAMABLE(CameraPose,
{
    void draw() const;

    void drawPose() const;
    ,
    (CvMat) rvec,
    (CvMat) tvec,

    (CvMat) rotationMatrix,
    (CvMat) rotationMatrixInv,

    (Vector3f) pos,
    (Vector3f) rot,
});

STREAMABLE_WITH_BASE(CameraPoseFiltered, CameraPose,
{
    void draw() const,
});
