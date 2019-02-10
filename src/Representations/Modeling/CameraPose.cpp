#include "CameraPose.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Modules/Camera.h"
#include <opencv2/calib3d.hpp>

void CameraPose::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:CameraPose","drawingOnImage");

    COMPLEX_DRAWING("representation:CameraPose")
    {
        drawPose();
    }
}

void CameraPose::drawPose() const
{
    float size = 0.04f;
    cv::Mat objectPoints(4, 3, CV_32FC1);
    objectPoints.at< float >(0, 0) = 0;
    objectPoints.at< float >(0, 1) = 0;
    objectPoints.at< float >(0, 2) = 0;
    objectPoints.at< float >(1, 0) = size;
    objectPoints.at< float >(1, 1) = 0;
    objectPoints.at< float >(1, 2) = 0;
    objectPoints.at< float >(2, 0) = 0;
    objectPoints.at< float >(2, 1) = size;
    objectPoints.at< float >(2, 2) = 0;
    objectPoints.at< float >(3, 0) = 0;
    objectPoints.at< float >(3, 1) = 0;
    objectPoints.at< float >(3, 2) = size;

    std::vector<cv::Point2f > imagePoints;
    cv::projectPoints(objectPoints, rvec, tvec, Camera::getCameraInfo().K, Camera::getCameraInfo().d, imagePoints);

    LINE("representation:CameraPose",imagePoints[0].x, imagePoints[0].y, imagePoints[1].x, imagePoints[1].y, 3, Drawings::solidPen,ColorRGBA::blue);
    LINE("representation:CameraPose",imagePoints[0].x, imagePoints[0].y, imagePoints[2].x, imagePoints[2].y, 3, Drawings::solidPen,ColorRGBA::red);
    LINE("representation:CameraPose",imagePoints[0].x, imagePoints[0].y, imagePoints[3].x, imagePoints[3].y, 3, Drawings::solidPen,ColorRGBA::green);
    LINE("representation:CameraPoseFiltered",imagePoints[0].x, imagePoints[0].y, imagePoints[1].x, imagePoints[1].y, 3, Drawings::solidPen,ColorRGBA::blue);
    LINE("representation:CameraPoseFiltered",imagePoints[0].x, imagePoints[0].y, imagePoints[2].x, imagePoints[2].y, 3, Drawings::solidPen,ColorRGBA::red);
    LINE("representation:CameraPoseFiltered",imagePoints[0].x, imagePoints[0].y, imagePoints[3].x, imagePoints[3].y, 3, Drawings::solidPen,ColorRGBA::green);
}

void CameraPoseFiltered::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:CameraPoseFiltered","drawingOnImage");

    COMPLEX_DRAWING("representation:CameraPoseFiltered")
    {
        drawPose();
    }
}

