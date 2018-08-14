#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/CameraInfo.h"
#include "Representations/FrameInfo.h"
#include "Representations/Image.h"
#include "Representations/Modeling/CameraPose.h"
#include <opencv2/aruco/charuco.hpp>

MODULE(ArucoPoseEstimator,
{,
 REQUIRES(CameraInfo),
 REQUIRES(FrameInfo),
 REQUIRES(GrayScaleImageEq),
 REQUIRES(ImageBGR),
 PROVIDES(CameraPose),
});

class ArucoPoseEstimator : public ArucoPoseEstimatorBase
{
private:

    static ArucoPoseEstimator *theInstance;

    void update(CameraPose& cameraPose);

    void draw(CameraPose& cameraPose);

    void saveCameraPose();

    //cv::aruco::MarkerDetector mDetector;

    //aruco::MarkerMapPoseTracker mPoseTracker;

    //aruco::MarkerMap mMapConfig;
	// create charuco board object
	cv::Ptr<cv::aruco::CharucoBoard> charucoBoard;

	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;

    //aruco::CameraParameters parameters;
	cv::Ptr<cv::aruco::Dictionary> arucoDictionary;

    float mMarkerSize;

    cv::Mat rvec,tvec;

public:

    ArucoPoseEstimator();

    static bool handleMessage(MessageQueue& message);

	bool readDetectorParameters(std::string filename, cv::Ptr<cv::aruco::DetectorParameters> &params);

	void calculatePosAndRot(CameraPose& cameraPose);
};
