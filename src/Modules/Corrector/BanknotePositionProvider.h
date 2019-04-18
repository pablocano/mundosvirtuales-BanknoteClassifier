#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Classification.h"
#include "Representations/FrameInfo.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include "Representations/Modeling/WorldCoordinatesPose.h"

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>
#include <map>

#include "opencv2/core/cuda.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/xfeatures2d/cuda.hpp"

MODULE(BanknotePositionProvider,
{,
 REQUIRES(WorldCoordinatesPose),
 REQUIRES(CorrectorImage),
 PROVIDES(BanknotePosition),
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
     * @brief Default destructor
     */
    ~BanknotePositionProvider();

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
    static bool analyzeArea(cv::Mat &homography, std::vector<Vector2f>& corners, Pose2f& pose, int banknote);

    /**
     * @brief compare the current image with the acoording template using the Classification representation
     * @param resultHomography the resulting homography between the template and the current image
     * @param fisrt the fisrt banknote to compare
     * @param last the last banknote to compare
     * @param the center of mass of the inliers
     * @return the banknote detected
     */
    static int compare(const Features& features, cv::Mat& resultHomography, int first, int last, Vector2f& massCenter);

    std::vector<cv::cuda::GpuMat> modelsImage;
    std::vector<Features> modelsFeatures;
    std::vector<std::vector<Vector3d> > modelsCorners;

    std::vector<cv::DMatch> matches;

    cv::Ptr<cv::cuda::DescriptorMatcher> matcher;
    cv::Ptr<cv::CLAHE> clahe;
    cv::cuda::SURF_CUDA surf;

    std::vector<cv::Mat> cannys;

    // Constants
    double minAreaPolygon;
    double maxAreaPolygon;

    //Aux
    int error;
    int lastbanknote;

    //Parameters
    int trainBanknoteHeight;


};

