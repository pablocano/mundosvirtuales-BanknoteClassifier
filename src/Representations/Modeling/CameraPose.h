#pragma once
#include "Tools/Streamable.h"
#include "Tools/Math/Eigen.h"
#include <opencv2/opencv.hpp>

class CameraPose : public Streamable
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
};
