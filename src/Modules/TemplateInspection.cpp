#include "TemplateInspection.h"
#include "Modules/BanknotePositionProvider.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/imgproc.hpp>

MAKE_MODULE(TemplateInspection, BanknoteClassifier)

TemplateInspection::TemplateInspection()
{

}

void TemplateInspection::update(GrabbingPosition &grabbingPosition)
{
    if(theBanknotePosition.banknote == Classification::NONE)
        return;

    Vector2i leftUpper, rightLower;
    Geometry::calculateRect(theBanknotePosition.corners, leftUpper, rightLower);

    leftUpper.x() = leftUpper.x() < 0 ? 0 : (leftUpper.x() >= theGrayScaleImageEq.cols ? theGrayScaleImageEq.cols - 1 : leftUpper.x());
    leftUpper.y() = leftUpper.y() < 0 ? 0 : (leftUpper.y() >= theGrayScaleImageEq.rows ? theGrayScaleImageEq.rows - 1 : leftUpper.y());
    rightLower.x() = rightLower.x() < 0 ? 0 : (rightLower.x() >= theGrayScaleImageEq.cols ? theGrayScaleImageEq.cols - 1 : rightLower.x());
    rightLower.y() = rightLower.y() < 0 ? 0 : (rightLower.y() >= theGrayScaleImageEq.rows ? theGrayScaleImageEq.rows - 1 : rightLower.y());

    const cv::Mat banknoteDetection = theGrayScaleImageEq(cv::Rect(leftUpper.x(),leftUpper.y(),rightLower.x() - leftUpper.x(),rightLower.y() - leftUpper.y()));

    cv::Mat cannyOutput;
    cv::Canny(banknoteDetection, cannyOutput, 100, 300, 3, true);

    cannyOutput = cannyOutput > 0;

    cv::Mat output;

    cv::warpPerspective(BanknotePositionProvider::theInstance->cannys[theBanknotePosition.banknote], output, theBanknotePosition.homography, cv::Size(theGrayScaleImageEq.cols, theGrayScaleImageEq.rows));

    DRAW_IMAGE("module:TemplateInspection:CannyCurrentBanknote", cannyOutput, theFrameInfo.time);


    cv::Mat cannyTemplateProjectedRoi =  output(cv::Rect(leftUpper.x(),leftUpper.y(),rightLower.x() - leftUpper.x(),rightLower.y() - leftUpper.y())).clone() > 0;
    DRAW_IMAGE("module:TemplateInspection:CannyTemplate", cannyTemplateProjectedRoi, theFrameInfo.time);

    cv::Mat comparision;

    cv::bitwise_xor(cannyTemplateProjectedRoi,cannyOutput,comparision);

    DRAW_IMAGE("module:TemplateInspection:Comparision", comparision, theFrameInfo.time);

}

