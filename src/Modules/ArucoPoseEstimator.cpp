#include "ArucoPoseEstimator.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/File.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Math/Constants.h"

MAKE_MODULE(ArucoPoseEstimator, CameraPose)

ArucoPoseEstimator* ArucoPoseEstimator::theInstance = 0;

ArucoPoseEstimator::ArucoPoseEstimator() : mMarkerSize(0.059f)
{
    theInstance = this;

	arucoDictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	charucoBoard = cv::aruco::CharucoBoard::create(8, 6, 0.025, 0.015, arucoDictionary);

	detectorParams = cv::aruco::DetectorParameters::create();
	//if (parser.has("dp")) {
	bool readOk = readDetectorParameters(std::string(File::getGTDir()) + "/Config/detector_params.yml", detectorParams);


    /*mMapConfig.readFromFile(std::string(File::getGTDir()) + "/Config/markerMapConfig.yml");
    parameters.readFromXMLFile(std::string(File::getGTDir())+"/Config/cameraCalibration1.yml");

    if ( mMapConfig.isExpressedInPixels() && mMarkerSize > 0)
        mMapConfig = mMapConfig.convertToMeters(mMarkerSize);

    if (parameters.isValid() && mMapConfig.isExpressedInMeters())
        mPoseTracker.setParams(parameters,mMapConfig);

    mDetector.setDictionary(aruco::Dictionary::ARUCO_MIP_36h12);
    mDetector.setThresholdParams(7, 7);
    mDetector.setThresholdParamRange(2, 0);*/
}

void ArucoPoseEstimator::update(CameraPose &cameraPose)
{
    DECLARE_DEBUG_DRAWING("module:ArucoPoseEstimator:pose","drawingOnImage");

	std::vector<int> markerIds, charucoIds;
	std::vector<std::vector<cv::Point2f>> markerCorners, rejectedMarkers;
	std::vector<cv::Point2f> charucoCorners;

	cameraPose.rotationMatrix.release();

	// detect markers
	cv::aruco::detectMarkers(theGrayScaleImageEq, arucoDictionary, markerCorners, markerIds, detectorParams,
		rejectedMarkers);

	// refind strategy to detect more markers
	cv::aruco::refineDetectedMarkers(theGrayScaleImageEq, charucoBoard, markerCorners, markerIds, rejectedMarkers, theCameraInfo.K, theCameraInfo.d);

	// interpolate charuco corners
	int interpolatedCorners = 0;
	if (markerIds.size() > 0)
		interpolatedCorners = cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, theGrayScaleImageEq, charucoBoard,
			charucoCorners, charucoIds, theCameraInfo.K, theCameraInfo.d);

	if (charucoIds.size() > 0/*mPoseTracker.isValid()*/) {

		if (cv::aruco::estimatePoseCharucoBoard(charucoCorners,charucoIds,charucoBoard,theCameraInfo.K,theCameraInfo.d,rvec,tvec)/*mPoseTracker.estimatePose(markers)*/) {
			cameraPose.rvec = rvec;
			cameraPose.tvec = tvec;

			cv::Rodrigues(rvec, cameraPose.rotationMatrix);
			cameraPose.rotationMatrixInv = cameraPose.rotationMatrix.inv();

			calculatePosAndRot(cameraPose);

			COMPLEX_DRAWING("module:ArucoPoseEstimator:pose",{draw(cameraPose);});

            cv::aruco::drawAxis(theImageBGR,theCameraInfo.K,theCameraInfo.d,rvec,tvec,0.3f);

        }
	}

    DEBUG_RESPONSE_ONCE("module:ArucoPoseEstimator:saveCameraPose", saveCameraPose(););

    cv::imshow("",theImageBGR);
}

void ArucoPoseEstimator::saveCameraPose()
{
    cv::FileStorage file(std::string(File::getGTDir()) + "/Config/cameraPose.yml", cv::FileStorage::WRITE);

    file << "rvec" << rvec;
    file << "tvec" << tvec;
}

void ArucoPoseEstimator::draw(CameraPose &cameraPose)
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
    cv::projectPoints(objectPoints, cameraPose.rvec, cameraPose.tvec, theCameraInfo.K, theCameraInfo.d, imagePoints);

    LINE("module:ArucoPoseEstimator:pose",imagePoints[0].x, imagePoints[0].y, imagePoints[1].x, imagePoints[1].y, 3, Drawings::ps_solid,ColorRGBA::blue);
    LINE("module:ArucoPoseEstimator:pose",imagePoints[0].x, imagePoints[0].y, imagePoints[2].x, imagePoints[2].y, 3, Drawings::ps_solid,ColorRGBA::red);
    LINE("module:ArucoPoseEstimator:pose",imagePoints[0].x, imagePoints[0].y, imagePoints[3].x, imagePoints[3].y, 3, Drawings::ps_solid,ColorRGBA::green);
}

bool ArucoPoseEstimator::handleMessage(MessageQueue &message)
{
    return false;
}

bool ArucoPoseEstimator::readDetectorParameters(std::string filename, cv::Ptr<cv::aruco::DetectorParameters> &params) {
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["adaptiveThreshWinSizeMin"] >> params->adaptiveThreshWinSizeMin;
	fs["adaptiveThreshWinSizeMax"] >> params->adaptiveThreshWinSizeMax;
	fs["adaptiveThreshWinSizeStep"] >> params->adaptiveThreshWinSizeStep;
	fs["adaptiveThreshConstant"] >> params->adaptiveThreshConstant;
	fs["minMarkerPerimeterRate"] >> params->minMarkerPerimeterRate;
	fs["maxMarkerPerimeterRate"] >> params->maxMarkerPerimeterRate;
	fs["polygonalApproxAccuracyRate"] >> params->polygonalApproxAccuracyRate;
	fs["minCornerDistanceRate"] >> params->minCornerDistanceRate;
	fs["minDistanceToBorder"] >> params->minDistanceToBorder;
	fs["minMarkerDistanceRate"] >> params->minMarkerDistanceRate;
	fs["cornerRefinementMethod"] >> params->cornerRefinementMethod;
	fs["cornerRefinementWinSize"] >> params->cornerRefinementWinSize;
	fs["cornerRefinementMaxIterations"] >> params->cornerRefinementMaxIterations;
	fs["cornerRefinementMinAccuracy"] >> params->cornerRefinementMinAccuracy;
	fs["markerBorderBits"] >> params->markerBorderBits;
	fs["perspectiveRemovePixelPerCell"] >> params->perspectiveRemovePixelPerCell;
	fs["perspectiveRemoveIgnoredMarginPerCell"] >> params->perspectiveRemoveIgnoredMarginPerCell;
	fs["maxErroneousBitsInBorderRate"] >> params->maxErroneousBitsInBorderRate;
	fs["minOtsuStdDev"] >> params->minOtsuStdDev;
	fs["errorCorrectionRate"] >> params->errorCorrectionRate;
	return true;
}

void ArucoPoseEstimator::calculatePosAndRot(CameraPose& cameraPose)
{
	cameraPose.pos = Eigen::Vector3f(cameraPose.tvec.at<double>(0, 0) * 1000, cameraPose.tvec.at<double>(1, 0) * 1000, cameraPose.tvec.at<double>(2, 0) * (-1000) + 2000);

	float sy = std::sqrt(cameraPose.rotationMatrix.at<double>(0, 0) * cameraPose.rotationMatrix.at<double>(0, 0) + cameraPose.rotationMatrix.at<double>(1, 0) * cameraPose.rotationMatrix.at<double>(1, 0));

	bool singular = sy < 1e-6; // If

	if (!singular)
	{
		cameraPose.rot = Eigen::Vector3f(atan2(cameraPose.rotationMatrix.at<double>(2, 1), cameraPose.rotationMatrix.at<double>(2, 2)), atan2(-cameraPose.rotationMatrix.at<double>(2, 0), sy), atan2(cameraPose.rotationMatrix.at<double>(1, 0), cameraPose.rotationMatrix.at<double>(0, 0)));
	}
	else
	{
		cameraPose.rot = Eigen::Vector3f(atan2(-cameraPose.rotationMatrix.at<double>(1, 2), cameraPose.rotationMatrix.at<double>(1, 1)), atan2(-cameraPose.rotationMatrix.at<double>(2, 0), sy), 0);
	}
}
