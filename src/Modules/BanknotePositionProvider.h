#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Features.h"
#include "Representations/FrameInfo.h"
#include "Representations/Image.h"
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>
#include <map>

MODULE(BanknotePositionProvider,
{,
 REQUIRES(Features),
 REQUIRES(FrameInfo),
 PROVIDES(BanknotePosition),
});



class BanknotePositionProvider : public BanknotePositionProviderBase
{
public:
    BanknotePositionProvider();

    std::map<std::string, cv::Mat> imageModel;
    std::map<std::string, std::vector<cv::KeyPoint>> dict_kp;
    std::map<std::string, cv::Mat> dict_des;
    std::map<std::string, std::vector<cv::Point2f>> dict_corners;

    std::vector<cv::KeyPoint> keyPointsModel1A;
    cv::Mat descriptorModel1A;
    std::vector<cv::DMatch> matches;
    std::vector<cv::Point2f> obj_corners;

    cv::BFMatcher matcher;
    cv::Ptr<cv::CLAHE> clahe;
    cv::Ptr<cv::xfeatures2d::SURF> surf;


    void update(BanknotePosition& banknotePosition);

};

