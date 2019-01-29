#include "WorldCoordinatesPoseProvider.h"
#include <opencv2/aruco.hpp>

MAKE_MODULE(WorldCoordinatesPoseProvider, BanknoteClassifier)

WorldCoordinatesPoseProvider::WorldCoordinatesPoseProvider()
{
    //aruco::CameraParameters p;
    //p.readFromXMLFile(std::string(File::getGTDir())+"/Config/cameraCalibration1.yml");

    //kInv = p.CameraMatrix.inv();

	kInv = theCameraInfo.K.inv();
}

void WorldCoordinatesPoseProvider::update(WorldCoordinatesPose &worldCoordinatesPose)
{
    // Initialice validity as false
    worldCoordinatesPose.valid = false;

    if (theBanknotePositionFiltered.valid)
    {
        // The world position only if valid if the camera pose is known and if a banknote is detected
        if(theCameraPose.rvec.empty() || theCameraPose.tvec.empty() || theBanknotePositionFiltered.banknote == Classification::NONE)
            return;

        cv::Mat r,rInv;
        // Create a rotation matrix, and invert it
        if(theCameraPose.rotationMatrix.empty()){
            cv::Rodrigues(theCameraPose.rvec,r);
            rInv = r.inv();
        }
        else
            rInv =theCameraPose.rotationMatrixInv;

        // Initialice the center point;
        cv::Mat uvPoint = cv::Mat::ones(3,1,cv::DataType<float>::type);

        uvPoint.at<float>(0,0) = theBanknotePositionFiltered.position.translation.x();
        uvPoint.at<float>(1,0) = theBanknotePositionFiltered.position.translation.y();

        // Auxiliary variables
        cv::Mat tempMat, tempMat2;
        float s, zConst = 0;

        //cv::Mat tvec = theCameraPose.tvec.t();
        cv::Mat tvec = theCameraPose.tvec;

        // Calculate a ray and intersect it with the ground plane
        tempMat = rInv * kInv * uvPoint;
        tempMat2 = rInv * tvec;
        s = zConst + tempMat2.at<float>(2,0);
        s /= tempMat.at<float>(2,0);
        cv::Mat wcPoint = rInv * (s * kInv * uvPoint - tvec);

        // Extrac the resulting point
        worldCoordinatesPose.translation = Vector2f(wcPoint.at<float>(0, 0), wcPoint.at<float>(1, 0));
        worldCoordinatesPose.valid = true;

        // Transform into milimeters
        worldCoordinatesPose.translation *= 1000;

        // Calculate the direction of the banknote
        Vector2f direction = theBanknotePositionFiltered.position.translation + Vector2f(100,0).rotate(theBanknotePositionFiltered.position.rotation);

        // Reinitialice the test point with the direction
        uvPoint.at<float>(0,0) = direction.x();
        uvPoint.at<float>(1,0) = direction.y();

        // Calculate the intersection of the ray with the ground plane
        tempMat = rInv * kInv * uvPoint;
        tempMat2 = rInv * tvec;
        s = zConst + tempMat2.at<float>(2,0);
        s /= tempMat.at<float>(2,0);
        wcPoint = rInv * (s * kInv * uvPoint - tvec);

        // Direction of the banknote in world coordinates
        direction = Vector2f(wcPoint.at<float>(0, 0), wcPoint.at<float>(1, 0));

        direction = direction*1000 - worldCoordinatesPose.translation;

        worldCoordinatesPose.rotation = direction.angle();


        // Calculate the offset for the gripper
        // Reinitialice the test point with the mass center
        uvPoint.at<float>(0,0) = theBanknotePositionFiltered.massCenter.x();
        uvPoint.at<float>(1,0) = theBanknotePositionFiltered.massCenter.y();

        // Calculate the intersection of the ray with the ground plane
        tempMat = rInv * kInv * uvPoint;
        tempMat2 = rInv * tvec;
        s = zConst + tempMat2.at<float>(2,0);
        s /= tempMat.at<float>(2,0);
        wcPoint = rInv * (s * kInv * uvPoint - tvec);

        // Offset of the gripper position in world coordinates
        worldCoordinatesPose.pickOffset = Vector2f(wcPoint.at<float>(0, 0), wcPoint.at<float>(1, 0))*1000 - worldCoordinatesPose.translation;

        worldCoordinatesPose.dropOffset = Eigen::Rotation2D<float>(-worldCoordinatesPose.rotation) * worldCoordinatesPose.pickOffset;

        if(std::abs(worldCoordinatesPose.dropOffset.x()) > 35 || std::abs(worldCoordinatesPose.dropOffset.y()) > 14)
        {
            worldCoordinatesPose.valid = false;
            return;
        }

        std::stringstream ss;

        ss << "Banknote" << (Classification::Banknote)theBanknotePositionFiltered.banknote <<  " \nPos:\n\t x: " << worldCoordinatesPose.translation.x() << "\n\t y: " << worldCoordinatesPose.translation.y() << "\n\t rot: " << worldCoordinatesPose.rotation.toDegrees() << "\nOffset:\n\tx: " << worldCoordinatesPose.pickOffset.x() << "\n\ty: " << worldCoordinatesPose.pickOffset.y() << "\n\DropOffset:\n\tx: " << worldCoordinatesPose.dropOffset.x() << "\n\ty: " << worldCoordinatesPose.dropOffset.y() << "\n";
        OUTPUT_TEXT(ss.str());

    }


}
