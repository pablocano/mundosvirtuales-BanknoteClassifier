#pragma once
#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Math/CvMat.h"

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


/*class CameraPose : public Streamable
{
public:

    void draw() const;

    void drawPose() const;

    cv::Mat rvec,tvec;

    cv::Mat rotationMatrix, rotationMatrixInv;

	Eigen::Vector3f pos, rot;
};

class CameraPoseFiltered : public CameraPose
{
    void draw() const;
};*/
