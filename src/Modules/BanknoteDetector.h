/**
 * @file BanknoteDetector.h
 *
 * Declaration of module BanknoteDetector
 * A CUDA + local descriptor strategy to detect stuff
 *
 * @author Keno
 */

#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknoteDetections.h"
#include "Representations/Classification.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include "Representations/FrameInfo.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Math/iou.h"
#include "Tools/Math/Pose2D.h"
#include "Tools/Math/Random.h"

#include "opencv2/core/cuda.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/xfeatures2d/cuda.hpp"
#include "opencv2/highgui.hpp"

#include <Eigen/Eigen>

MODULE(BanknoteDetector,
{,
    REQUIRES(GrayScaleImage),
    REQUIRES(GrayScaleImageEq),
    REQUIRES(FrameInfo),
    PROVIDES(BanknoteDetections),
});

ENUM(CornerID,
    TopLeft,
    TopRight,
    BottomRight,
    BottomLeft,
    numOfRealCorners,
    MiddleMiddle = numOfRealCorners,
    MiddleRight
);

bool compareAngle(IOU::Point p1, IOU::Point p2) { return (std::atan2(p1.y, p1.x) < std::atan2(p2.y, p2.x)); }

class Model
{
public:
    cv::cuda::GpuMat gpuImage;
    cv::Mat image;
    cv::Mat mask;
    Features features;
    Eigen::Vector3f corners[CornerID::numOfCornerIDs];
};

class Hypothesys
{
public:

    Hypothesys();
    bool isValid() const;

    std::vector<cv::DMatch> matches;
    Eigen::Matrix3f transform; /* From the model (a.k.a train image) to the camera image (a.k.a query image) */
    Pose2D pose;
    Eigen::Vector3f graspPoint;
    int ransacVotes;
    bool validTransform;
    bool validNms;
    bool validPolygon;
};

class ClassDetections
{
  public:

    ClassDetections();
    void clear();

    std::vector<cv::DMatch> matches;
    std::vector<cv::DMatch> houghFilteredMatches;
    std::vector<Hypothesys> hypotheses;
};



class BanknoteDetector : public BanknoteDetectorBase
{
public:
    BanknoteDetector();

    void update(BanknoteDetections& detections);

protected:

    /* Main Filters */
    void hough4d(const Model& model, ClassDetections& detections);
    void ransac(const Model& model, ClassDetections& detections);
    void estimateTransforms(const Model& model, ClassDetections& detections);
    void nonMaximumSupression(const Model& model, ClassDetections& detections);

    /* Math Related */
    void resizeImage(cv::Mat& image);

    Eigen::Matrix3f getTransformAsMatrix(const cv::KeyPoint& src, const cv::KeyPoint& dst);
    inline void getTransform(const cv::KeyPoint& src, const cv::KeyPoint& dst, float& tx, float& ty, float& angleDegrees, float& e);

    int getRansacConsensus(const Eigen::Matrix3f& transform, const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& trainKeypoints, const std::vector<cv::KeyPoint>& queryKeypoints, float maxError);
    void getRansacInliers(const Eigen::Matrix3f& transform, const std::vector<cv::DMatch>& matches, std::vector<cv::DMatch>& acceptedMatchesfloat, const std::vector<cv::KeyPoint>& trainKeypoints, const std::vector<cv::KeyPoint>& queryKeypoints, float maxError, Eigen::VectorXi& acceptedStatus);

    void drawAcceptedHough();
    void drawAcceptedRansac();
    void drawAcceptedHypotheses();

    cv::Ptr<cv::cuda::DescriptorMatcher> matcher;
    cv::cuda::SURF_CUDA surf;
    cv::Ptr<cv::CLAHE> clahe;

    /** Gpu Buffers */
    cv::cuda::GpuMat gpuImage;
    cv::cuda::GpuMat gpuImageKeypoints;
    cv::cuda::GpuMat gpuImageDescriptors;
    cv::cuda::GpuMat gpuImageMask;

    /** RAM Buffers */
    std::vector<cv::KeyPoint> imageKeypoints;

    /** Models */
    Model models[Classification::numOfBanknotes - 2];

    /** Matches Buffer. Should really use an alias or somethin'. Shape is classes x matches x 2 (2 is the KNN criteria)*/
    ClassDetections classDetections[Classification::numOfBanknotes - 2];

    /** Module Parameters */
    bool resizeModels; /* This is a must when using scanned images */
    int trainBanknoteHeight; /* hardcoded parameter in order to resize*/

    ColorRGBA debugColors[Classification::numOfBanknotes - 2];
};
