#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Blobs.h"
#include "Representations/Classification.h"
#include "Representations/ErrorInfo.h"
#include "Representations/Features.h"
#include "Representations/FrameInfo.h"
#include "Representations/Image.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>
#include <map>

MODULE(BanknotePositionProvider,
{,
 REQUIRES(Blobs),
 REQUIRES(Classification),
 REQUIRES(Features),
 REQUIRES(FrameInfo),
 //REQUIRES(ImageBGR),
 REQUIRES(PreviousBanknotePosition),
 PROVIDES(BanknotePosition),
 PROVIDES(ErrorInfo),
});



class BanknotePositionProvider : public BanknotePositionProviderBase
{
public:

    static BanknotePositionProvider *theInstance;

    /**
     * @brief Default constructor
     */
    BanknotePositionProvider();

    /**
     * @brief update
     * @param banknotePosition
     */
    void update(BanknotePosition& banknotePosition);

    void update(ErrorInfo& errorinfo);

    /**
     * @brief Resize the image and aplicates and clane equalization
     * @param image the image to resize
     */
    void resizeImage(cv::Mat& image);

    /**
     * @brief compare the current image with the acoording template using the Classification representation
     * @param resultHomography the resulting homography between the template and the current image
     * @return the banknote detected
     */
    int compare(cv::Mat& resultHomography, int start, int amount);

    /**
     * @brief analyze the resulting area given by the perpective transformation
     * @param corners the corners of the area to analyse
     * @return if the area is valid
     */
    static bool analyzeArea(cv::Mat &homography, std::vector<Vector2f>& corners, Pose2D& pose);

    /**
     * @brief compare the current image with the acoording template using the Classification representation
     * @param resultHomography the resulting homography between the template and the current image
     * @param fisrt the fisrt banknote to compare
     * @param last the last banknote to compare
     * @return the banknote detected
     */
    static int compare(const Features& features, cv::Mat& resultHomography, int first, int last);

    // Models features
    std::vector<cv::Mat> modelsImage;
    std::vector<Features> modelsFeatures;
    std::vector<Vector3d> modelsCorners;

    std::vector<cv::DMatch> matches;

    // Tools
    cv::BFMatcher matcher;
    cv::Ptr<cv::CLAHE> clahe;
    cv::Ptr<cv::xfeatures2d::SURF> surf;

    std::vector<cv::Mat> cannys;

    // Constants
    double minAreaPolygon;
    double maxAreaPolygon;

    //Aux
    int error;
    int lastbanknote;


};

