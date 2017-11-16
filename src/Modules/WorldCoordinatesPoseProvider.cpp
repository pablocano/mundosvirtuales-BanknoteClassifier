#include "WorldCoordinatesPoseProvider.h"
#include <aruco/aruco.h>

MAKE_MODULE(WorldCoordinatesPoseProvider, BanknoteClassifier)

WorldCoordinatesPoseProvider::WorldCoordinatesPoseProvider()
{
    aruco::CameraParameters p;
    p.readFromXMLFile(std::string(File::getGTDir())+"/Config/cameraCalibration1.yml");

    kInv = p.CameraMatrix.inv();
}

void WorldCoordinatesPoseProvider::update(WorldCoordinatesPose &worldCoordinatesPose)
{

    worldCoordinatesPose.valid = false;

    if(theCameraPose.rvec.empty() || theCameraPose.tvec.empty() || theBanknotePositionFiltered.banknote == Classification::NONE)
        return;

    cv::Mat r,rInv;
    cv::Rodrigues(theCameraPose.rvec,r);
    rInv = r.inv();

    cv::Mat uvPoint = cv::Mat::ones(3,1,cv::DataType<float>::type);

    Vector2f center(0,0);
    for(const auto& corner : theBanknotePositionFiltered.corners)
    {
        center += corner;
    }

    center /= theBanknotePositionFiltered.corners.size();

    uvPoint.at<float>(0,0) = center.x();
    uvPoint.at<float>(1,0) = center.y();

    cv::Mat tempMat, tempMat2;
    float s, zConst = 0;

    cv::Mat tvec = theCameraPose.tvec.t();


    tempMat = rInv * kInv * uvPoint;
    tempMat2 = rInv * tvec;

    s = zConst + tempMat2.at<float>(2,0);
    s /= tempMat.at<float>(2,0);
    cv::Mat wcPoint = rInv * (s * kInv * uvPoint - tvec);

    worldCoordinatesPose.point = Vector2f(wcPoint.at<float>(0, 0), wcPoint.at<float>(1, 0));
    worldCoordinatesPose.valid = true;

    worldCoordinatesPose.point *= 1000;

    std::cout << worldCoordinatesPose.point.x() << "  " << worldCoordinatesPose.point.y() << std::endl;
}
