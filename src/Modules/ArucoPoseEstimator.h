#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/CameraInfo.h"
#include "Representations/FrameInfo.h"
#include "Representations/Image.h"
#include "Representations/Modeling/CameraPose.h"
#include <aruco/aruco.h>

MODULE(ArucoPoseEstimator,
{,
 REQUIRES(CameraInfo),
 REQUIRES(FrameInfo),
 REQUIRES(GrayScaleImageEq),
 PROVIDES(CameraPose),
});

class ArucoPoseEstimator : public ArucoPoseEstimatorBase
{
private:

    static ArucoPoseEstimator *theInstance;

    void update(CameraPose& cameraPose);

    void draw(CameraPose& cameraPose);

    void saveCameraPose();

    aruco::MarkerDetector mDetector;

    aruco::MarkerMapPoseTracker mPoseTracker;

    aruco::MarkerMap mMapConfig;

    aruco::CameraParameters parameters;

    float mMarkerSize;

    cv::Mat rvec,tvec;

public:

    ArucoPoseEstimator();

    static bool handleMessage(MessageQueue& message);
};
