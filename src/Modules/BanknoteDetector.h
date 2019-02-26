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
#include "Representations/BanknoteDetectionParameters.h"
#include "Representations/BanknoteModel.h"
#include "Representations/Classification.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include "Representations/FrameInfo.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Math/Random.h"

#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/xfeatures2d/cuda.hpp>
#include <opencv2/highgui.hpp>

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
    REQUIRES(SegmentedImage),
    REQUIRES(FrameInfo),
    PROVIDES(BanknoteDetections),
    DEFINES_PARAMETERS(
    {,
        (BanknoteDetectionParameters[Classification::numOfRealBanknotes]) parameters, /* Just in case, each class has its own set of parameters. This should allow a fine-grained  tuning capability */
        (int)(320) minMaskX, /* These hardcoded mask coordinates indicate where in the image, processing should be perfomed. Ideally, the FOV of the camera should match the working area, but untill that happens, please cope up with the masks */
        (int)(1600) maxMaskX,
        (int)(140) minMaskY,
        (int)(1900) maxMaskY,
    }),
});


/**
 * @brief The ClassDetections class
 *
 * This class represents a buffer for partial detections of a particular Banknote class
 */
class ClassDetections
{
  public:

    ClassDetections();
    void clear();

    std::vector<std::vector<cv::DMatch>> matches;
    std::vector<cv::DMatch> houghFilteredMatches;
    std::vector<BanknoteDetection> detections;
};


/**
 * @brief The BanknoteDetector class
 */
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
    void hough4d(const BanknoteModel& model, const BanknoteDetectionParameters& parameters, ClassDetections& detections);

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
    void ransac(const BanknoteModel& model, const BanknoteDetectionParameters& parameters,  ClassDetections& detections);

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
    void estimateTransforms(const BanknoteModel& model, const BanknoteDetectionParameters& parameters,  ClassDetections& detections);

    /**
     * @brief nonMaximumSupression
     *
     * Removes duplicates between the same class using an IOU criteria,
     * and the decision is made using the ransac votes as a score.
     *
     * @param model: The BankNote model
     * @param detections: the output Detections
     */
    void nonMaximumSupression(const BanknoteModel& model, const BanknoteDetectionParameters& parameters, ClassDetections& detections);

    /**
     * @brief nonMaximumSupression
     *
     * Estimates the order based on overlapping descriptores over the hypotheses.
     *
     * @param model: The BankNote model
     * @param detections: the output Detections
     */
    void foregroundEstimation(const BanknoteModel& model, ClassDetections& detections);

    /**
     * @brief evaluateGraspingScore
     *
     * Evaluates the selected grasping point
     * according to geometric criteria
     *
     * @param model: The BankNote model
     * @param detections: the output Detections
     */
    void evaluateGraspingScore(const BanknoteModel& model, const BanknoteDetectionParameters& params, ClassDetections& detections);

    void prepareImageMask();

    /* Math Related */
    void resizeImage(cv::Mat& image);

    Matrix3f getTransformAsMatrix(const cv::KeyPoint& src, const cv::KeyPoint& dst);
    inline void getTransform(const cv::KeyPoint& src, const cv::KeyPoint& dst, float& tx, float& ty, float& angleDegrees, float& e);

    int getRansacConsensus(const Matrix3f& transform, const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& trainKeypoints, const std::vector<cv::KeyPoint>& queryKeypoints, float maxError, const VectorXi& acceptedStatus);
    void getRansacInliers(const Matrix3f& transform, const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& inputTrainKeypoints, const std::vector<cv::KeyPoint>& inputQueryKeypoints, float maxError, float maxError2, VectorXi& acceptedStatus, std::vector<cv::DMatch>& acceptedMatches, std::vector<Vector3f>& outputTrainKeypoints, std::vector<Vector3f>& outputQueryKeypoints);

    void compareForeground(BanknoteDetection& d1, BanknoteDetection& d2);

    void drawAcceptedHough();
    void drawAcceptedRansac();
    void drawAcceptedHypotheses();

    cv::Ptr<cv::cuda::DescriptorMatcher> matcher;
    cv::cuda::SURF_CUDA surf;
    cv::Ptr<cv::CLAHE> clahe;

    cv::Mat imageMask;

    /** Gpu Buffers */
    cv::cuda::GpuMat gpuImage;
    cv::cuda::GpuMat gpuImageKeypoints;
    cv::cuda::GpuMat gpuImageDescriptors;
    cv::cuda::GpuMat gpuImageMask;

    /** RAM Buffers */
    std::vector<cv::KeyPoint> imageKeypoints;

    /** Models */
    BanknoteModel models[Classification::numOfRealBanknotes];

    ClassDetections classDetections[Classification::numOfRealBanknotes];

    /** Global Module Parameters */
    //ClassParameters parameters;
    bool resizeModels; /* This is a must when using scanned images */
    int trainBanknoteHeight; /* hardcoded parameter in order to resize*/

    /** GEOS Stuff */
    geos::geom::GeometryFactory::Ptr factory;
    geos::geom::Point* aux_point;

    ColorRGBA debugColors[Classification::numOfRealBanknotes];

    //BanknoteDetectionParameters parameters[Classification::numOfRealBanknotes];
};
