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
#include "Tools/Math/Geometry.h"
#include "Tools/Math/iou.h"
#include "Tools/Math/Pose2f.h"
#include "Tools/Math/Random.h"

#include "opencv2/core/cuda.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/xfeatures2d/cuda.hpp"
#include "opencv2/highgui.hpp"

#include <Eigen/Eigen>

#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/CoordinateArraySequence.h>

MODULE(BanknoteDetector,
{,
    REQUIRES(GrayScaleImage),
    REQUIRES(GrayScaleImageEq),
    REQUIRES(FrameInfo),
    PROVIDES(BanknoteDetections),
});

ENUM(CornerID,
{,
    TopLeft,
    TopRight,
    BottomRight,
    BottomLeft,
    numOfRealCorners,
    MiddleMiddle = numOfRealCorners,
    MiddleRight,
});

STREAMABLE(ClassParameters,
{,
    (float)(45) houghXYStep, /* in pixels */
    (float)(30) houghAngleStep, /* in degrees */
    (int)(9) houghVotesThresh,
    (float)(0.8f) minAllowedScale,
    (float)(1.2f) maxAllowedScale,
    (float)(20.f) ransacMaxError,
    (float)(30.f) ransacMaxError2,
    (int)(15) ransacMinConsensus,
    (int)(50) ransacNumberOfTrials,
    (float)(0.6f) maxAllowedIOU,
    (float)(60) graspRadius,
});


class Model
{
public:
    cv::cuda::GpuMat gpuImage;
    cv::Mat image;
    cv::Mat mask;
    Features features;
    Vector3f corners[CornerID::numOfCornerIDs];
};

class Hypothesys
{
public:

    Hypothesys();
    ~Hypothesys();
    bool isValid() const;

    /* Buffer with detection related points */
    std::vector<cv::DMatch> matches;
    std::vector<Vector3f> queryPoints;
    std::vector<Vector3f> trainPoints;
    Vector3f queryCorners[CornerID::numOfRealCorners];

    /* Detection representation*/
    Matrix3f transform; /* From the model (a.k.a train image) to the camera image (a.k.a query image) */
    Pose2f pose; /* 2D Pose of the hypothesis in the image space */
    Vector3f graspPoint; /* Estimated grasping point */

    /* Detection statistics */
    int ransacVotes;
    float graspScore;
    float maxIOU;
    int layer; /* 0 = foreground. 1,2,... represent the "depth" */

    /* Status flags */
    bool validTransform;
    bool validNms;
    bool validGrasp;

    /* Geometry objects representing the hypothesys */
    geos::geom::Geometry* validGeometry;
    geos::geom::Geometry* geometry;
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
    ~BanknoteDetector();

    void update(BanknoteDetections& detections);

protected:

    /* Main Filters */

    /**
     * @brief hough4d
     *
     * 1st filter in the pipeline (after keypoint and matching)
     *
     * Consistent matches should indicate similar transforms.
     * We use hough4d to make matches vote in the transform space and
     * select only those who seem consistent
     *
     * If set too strict, only valid matches will remain, but may be too few.
     * If set too lose, falses will remain, but it still may be  possible to filter them with ransac
     *
     * @param model: The BankNote model
     * @param detections: the output Detections
     */
    void hough4d(const Model& model, const ClassParameters& parameters, ClassDetections& detections);

    /**
     * @brief ransac
     *
     * 2nd filter in the pipeline (after keypoint and matching)
     *
     * Consistent matches should indicate similar transforms.
     * We expect a high ratio os inliers vs outliers.
     * We use a transform model for the ransac hypothesis and use the reprojection
     * error from query (video) to train (banknote model) to calculate consensus.
     *
     * If set too lose, ransac will only cluster hypotheses, and may cause duplicates.
     * It set tight, this will also filter false positives, but if set too tight, will also filter true positives.
     *
     * @param model: The BankNote model
     * @param detections: the output Detections
     */
    void ransac(const Model& model, const ClassParameters& parameters,  ClassDetections& detections);

    /**
     * @brief estimateTransforms
     *
     * For each hypothesys, we calculate the proper transform (3x3 matrix. Affine or similitude, havent decided yet).
     * May be able to filter according to the transform.
     * Also computes the BankNote pose and grasping point
     *
     * @param model: The BankNote model
     * @param detections: the output Detections
     */
    void estimateTransforms(const Model& model, const ClassParameters& parameters,  ClassDetections& detections);

    /**
     * @brief nonMaximumSupression
     *
     * Removes duplicates between the same class using an IOU criteria,
     * and the decision is made using the ransac votes as a score.
     *
     * @param model: The BankNote model
     * @param detections: the output Detections
     */
    void nonMaximumSupression(const Model& model, const ClassParameters& parameters, ClassDetections& detections);

    /**
     * @brief nonMaximumSupression
     *
     * Estimates the order based on overlapping descriptores over the hypotheses.
     *
     * @param model: The BankNote model
     * @param detections: the output Detections
     */
    void foregroundEstimation(const Model& model, ClassDetections& detections);

    /**
     * @brief evaluateGraspingScore
     *
     * Evaluates the selected grasping point
     * according to geometric criteria
     *
     * @param model: The BankNote model
     * @param detections: the output Detections
     */
    void evaluateGraspingScore(const Model& model, ClassDetections& detections);

    /* Math Related */
    void resizeImage(cv::Mat& image);

    Matrix3f getTransformAsMatrix(const cv::KeyPoint& src, const cv::KeyPoint& dst);
    inline void getTransform(const cv::KeyPoint& src, const cv::KeyPoint& dst, float& tx, float& ty, float& angleDegrees, float& e);

    int getRansacConsensus(const Matrix3f& transform, const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& trainKeypoints, const std::vector<cv::KeyPoint>& queryKeypoints, float maxError, const VectorXi& acceptedStatus);
    void getRansacInliers(const Matrix3f& transform, const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& inputTrainKeypoints, const std::vector<cv::KeyPoint>& inputQueryKeypoints, float maxError, float maxError2, VectorXi& acceptedStatus, std::vector<cv::DMatch>& acceptedMatches, std::vector<Vector3f>& outputTrainKeypoints, std::vector<Vector3f>& outputQueryKeypoints);

    void compareForeground(Hypothesys& h1, Hypothesys& s2);

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

    /** Global Module Parameters */
    ClassParameters parameters;
    bool resizeModels; /* This is a must when using scanned images */
    int trainBanknoteHeight; /* hardcoded parameter in order to resize*/
    float graspRadius; /* In pixels. This should be computed with the real grasp radius and the camera transform */

    /** GEOS Stuff */
    geos::geom::GeometryFactory::Ptr factory;
    geos::geom::Point* aux_point;

    ColorRGBA debugColors[Classification::numOfBanknotes - 2];
};
