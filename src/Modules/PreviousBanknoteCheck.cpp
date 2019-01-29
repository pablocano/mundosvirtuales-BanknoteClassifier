#include "PreviousBanknoteCheck.h"
#include "Modules/BanknotePositionProvider.h"
#include "Tools/Math/Geometry.h"

MAKE_MODULE(PreviousBanknoteCheck, BanknoteClassifier)

PreviousBanknoteCheck::PreviousBanknoteCheck()
{
#ifndef BC_WITH_CUDA
    surf_ = cv::xfeatures2d::SURF::create(500,4,3,true,false);
#else
    surf_ = cv::cuda::SURF_CUDA(500,4,4,true);
#endif

    error = 0;
    lastbanknote = 0;
    newSearch = 1;

}


void PreviousBanknoteCheck::update(PreviousBanknotePosition &previousBanknotePosition)
{

    if (theRegState.getbanknote)
    {
        OUTPUT_TEXT("start prev pos");
        previousBanknotePosition.banknote = Classification::NONE;

        if(theBanknotePositionFiltered.valid && !theWorldCoordinatesPose.valid)
        {
            newSearch = 1;
            return;
        }

        if(theBanknotePosition.banknote != Classification::NONE && !newSearch)
        {
            OUTPUT_TEXT("trying prev pos");
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

#ifndef BC_WITH_CUDA
            surf_->detectAndCompute(theGrayScaleImageEq,mask,features.keypoints,features.descriptors);
#else
            cv::cuda::GpuMat grayScaleImageGpu(theGrayScaleImageEq);
            cv::cuda::GpuMat maskGpu(mask);
            surf_(grayScaleImageGpu,maskGpu,features.keypointsGpu,features.descriptors);
            surf_.downloadKeypoints(features.keypointsGpu,features.keypoints);
#endif

            cv::Mat H;
            Vector2f massCenter;

            int banknote = BanknotePositionProvider::compare(features, H, theBanknotePosition.banknote, theBanknotePosition.banknote, massCenter);

            if (!H.empty() && banknote == theBanknotePosition.banknote){
                Pose2D pose;
                std::vector<Vector2f> scene_corners;
                OUTPUT_TEXT("previous banknote test");
                OUTPUT_TEXT((Classification::Banknote)banknote);
                if(BanknotePositionProvider::analyzeArea(H, scene_corners, pose, banknote))
                {
                    error = 0;
                    previousBanknotePosition.banknote = (Classification::Banknote)banknote;
                    previousBanknotePosition.homography = H;
                    scene_corners.push_back(scene_corners.front());
                    previousBanknotePosition.corners = scene_corners;
                    previousBanknotePosition.position = pose;
                    previousBanknotePosition.massCenter = massCenter;
                }
                else{
                    error = 1;
                    lastbanknote = theBanknotePosition.banknote;
                }
            }
        }
        else
            newSearch = 0;
    }
    else
    {
        newSearch = 1;
        previousBanknotePosition.banknote = Classification::STOP;
    }

}



