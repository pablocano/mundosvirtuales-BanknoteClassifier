#include "CameraPoseProvider.h"
#include "Platform/File.h"
#include <opencv2/calib3d.hpp>

MAKE_MODULE(CameraPoseProvider, BanknoteClassifier)

CameraPoseProvider::CameraPoseProvider() : set(true)
{
    cv::FileStorage file(std::string(File::getBCDir()) + "/Config/cameraPose.yml", cv::FileStorage::READ);

    file["rvec"] >> rvec;
    file["tvec"] >> tvec;

}

void CameraPoseProvider::update(CameraPose &cameraPose)
{
    if(set){
        cameraPose.rvec = rvec;
        cameraPose.tvec = tvec;

        // Calculate the ratational matrix and the inverse of the rotational matrix,
        // because the in this configuration the camera matrix does noy change
        cv::Rodrigues(rvec,cameraPose.rotationMatrix);
        cameraPose.rotationMatrixInv = cameraPose.rotationMatrix.inv();
        set = false;
    }
}

