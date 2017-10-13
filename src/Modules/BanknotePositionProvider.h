#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Classification.h"
#include "Representations/Features.h"
#include "Representations/FrameInfo.h"
#include "Representations/Image.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>
#include <map>

MODULE(BanknotePositionProvider,
{,
 REQUIRES(Classification),
 REQUIRES(Features),
 REQUIRES(FrameInfo),
 PROVIDES(BanknotePosition),
});



class BanknotePositionProvider : public BanknotePositionProviderBase
{
public:
    /**
     * @brief Default constructor
     */
    BanknotePositionProvider();

    /**
     * @brief update
     * @param banknotePosition
     */
    void update(BanknotePosition& banknotePosition);

    /**
     * @brief Resize the image and aplicates and clane equalization
     * @param image the image to resize
     */
    void resizeImage(cv::Mat& image);

    int compare(cv::Mat& resultHomography);

    void getColorAndStyle(Classification::Banknote banknote, ColorRGBA& color, Drawings::PenStyle &style);

    // Models features
    std::vector<cv::Mat> modelsImage;
    std::vector<Features> modelsFeatures;
    std::vector<cv::Point2f> modelsCorners;

    std::vector<cv::DMatch> matches;

    // Tools
    cv::BFMatcher matcher;
    cv::Ptr<cv::CLAHE> clahe;
    cv::Ptr<cv::xfeatures2d::SURF> surf;

};

