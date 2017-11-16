#include "ArucoPoseEstimator.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/File.h"

MAKE_MODULE(ArucoPoseEstimator, BanknoteClassifier)

ArucoPoseEstimator::ArucoPoseEstimator() : mMarkerSize(0.06f)
{
    mMapConfig.readFromFile(std::string(File::getGTDir()) + "/Config/markerMapConfig.yml");
    parameters.readFromXMLFile(std::string(File::getGTDir())+"/Config/cameraCalibration1.yml");

    if ( mMapConfig.isExpressedInPixels() && mMarkerSize > 0)
        mMapConfig = mMapConfig.convertToMeters(mMarkerSize);

    if (parameters.isValid() && mMapConfig.isExpressedInMeters())
        mPoseTracker.setParams(parameters,mMapConfig);

    mDetector.setDictionary(aruco::Dictionary::ARUCO_MIP_36h12);
    mDetector.setThresholdParams(7, 7);
    mDetector.setThresholdParamRange(2, 0);
}

void ArucoPoseEstimator::update(CameraPose &cameraPose)
{
    DECLARE_DEBUG_DRAWING("module:ArucoPoseEstimator:pose","drawingOnImage");

    std::vector<aruco::Marker> markers = mDetector.detect(theGrayScaleImageEq, parameters, mMarkerSize);

    if (mPoseTracker.isValid()){

        if ( mPoseTracker.estimatePose(markers)){
            cameraPose.rvec = mPoseTracker.getRvec();
            cameraPose.tvec = mPoseTracker.getTvec();

            COMPLEX_DRAWING("module:ArucoPoseEstimator:pose",{draw(cameraPose);});
        }
    }
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
    cv::projectPoints(objectPoints, cameraPose.rvec, cameraPose.tvec, parameters.CameraMatrix, parameters.Distorsion, imagePoints);

    LINE("module:ArucoPoseEstimator:pose",imagePoints[0].x, imagePoints[0].y, imagePoints[1].x, imagePoints[1].y, 3, Drawings::ps_solid,ColorRGBA::blue);
    LINE("module:ArucoPoseEstimator:pose",imagePoints[0].x, imagePoints[0].y, imagePoints[2].x, imagePoints[2].y, 3, Drawings::ps_solid,ColorRGBA::red);
    LINE("module:ArucoPoseEstimator:pose",imagePoints[0].x, imagePoints[0].y, imagePoints[3].x, imagePoints[3].y, 3, Drawings::ps_solid,ColorRGBA::green);
}
