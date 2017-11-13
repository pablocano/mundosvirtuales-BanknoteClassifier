#include "PreviousBanknoteCheck.h"
#include "Modules/BanknotePositionProvider.h"
#include "Tools/Math/Geometry.h"
#include <iostream>

MAKE_MODULE(PreviousBanknoteCheck, BanknoteClassifier)

PreviousBanknoteCheck::PreviousBanknoteCheck()
{
    surf_ = cv::xfeatures2d::SURF::create(500,4,3,true,false);
    error = 0;
    lastbanknote = 0;
}


void PreviousBanknoteCheck::update(PreviousBanknotePosition &previousBanknotePosition)
{
    previousBanknotePosition.banknote = Classification::NONE;

    if(theBanknotePosition.banknote != Classification::NONE)
    {
        if(mask.empty())
            mask = cv::Mat::zeros(theGrayScaleImageEq.rows, theGrayScaleImageEq.cols, CV_8U);
        else
            mask.setTo(cv::Scalar(0));

        Vector2i leftUpper, rightLower;
        Geometry::calculateRect(theBanknotePosition.corners, leftUpper, rightLower);

        leftUpper.x() = leftUpper.x() < 0 ? 0 : (leftUpper.x() >= theGrayScaleImageEq.cols ? theGrayScaleImageEq.cols - 1 : leftUpper.x());
        leftUpper.y() = leftUpper.y() < 0 ? 0 : (leftUpper.y() >= theGrayScaleImageEq.rows ? theGrayScaleImageEq.rows - 1 : leftUpper.y());
        rightLower.x() = rightLower.x() < 0 ? 0 : (rightLower.x() >= theGrayScaleImageEq.cols ? theGrayScaleImageEq.cols - 1 : rightLower.x());
        rightLower.y() = rightLower.y() < 0 ? 0 : (rightLower.y() >= theGrayScaleImageEq.rows ? theGrayScaleImageEq.rows - 1 : rightLower.y());

        mask(cv::Rect(leftUpper.x(),leftUpper.y(),rightLower.x() - leftUpper.x(),rightLower.y() - leftUpper.y())) = 1;

        surf_->detectAndCompute(theGrayScaleImageEq,mask,features.keypoints,features.descriptors);

        cv::Mat H;

        int banknote = BanknotePositionProvider::compare(features, H, theBanknotePosition.banknote, theBanknotePosition.banknote);

        if (!H.empty() && banknote == theBanknotePosition.banknote){
            std::vector<Vector2f> scene_corners;
            std::cout<<"previous banknote test"<<std::endl;
            std::cout<<(Classification::Banknote)banknote<<std::endl;
            if(BanknotePositionProvider::analyzeArea(H, scene_corners))
	    {
                error = 0;
                previousBanknotePosition.banknote = (Classification::Banknote)banknote;
		previousBanknotePosition.homography = H;
                scene_corners.push_back(scene_corners.front());
                previousBanknotePosition.corners = scene_corners;
            }
            else{
                error = 1;
                lastbanknote = theBanknotePosition.banknote;
            }
        }
    }
}

void PreviousBanknoteCheck::update(ErrorInfo &errorinfo){
    errorinfo.error = error;
    errorinfo.lastbanknote = lastbanknote;
}

