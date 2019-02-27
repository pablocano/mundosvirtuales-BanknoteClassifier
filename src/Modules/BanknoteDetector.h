/**
 * @file BanknoteDetector.h
 *
 * Declaration of module BanknoteDetector
 * A CUDA + local descriptor strategy to detect banknotes
 *
 * This module excecutes the following steps:
 *  - Calculates keypoints and descriptors over the working area (in GPU)
 *  - Calculates matches between the query image (from the camera)  to each of the banknote templates (in GPU)
 *  - Uses the Hough transform to discard false-positive hypotheses
 *  - Uses RANSAC to group matches into hypotheses and also discard false positives
 *  - Calculates the transform for each hypothesys (currently using RANSAC again, although at this point least squares or any other method can be used)
 *  - Uses Non maximum supression to avoid duplicated detection
 *  - Format hypotheses into BanknoteDetection format
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

#include <Eigen/Eigen>

#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/CoordinateArraySequence.h>

#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/xfeatures2d/cuda.hpp>
#include <opencv2/highgui.hpp>

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
    void hough4d(const BanknoteModel& model, const BanknoteDetectionParameters& parameters, ClassDetections& detections);
    void ransac(const BanknoteModel& model, const BanknoteDetectionParameters& parameters,  ClassDetections& detections);
    void estimateTransforms(const BanknoteModel& model, const BanknoteDetectionParameters& parameters,  ClassDetections& detections);
    void nonMaximumSupression(const BanknoteModel& model, const BanknoteDetectionParameters& parameters, ClassDetections& detections);

    void prepareImageMask();

    /* Math Related */
    void resizeImage(cv::Mat& image);

    Matrix3f getTransformAsMatrix(const cv::KeyPoint& src, const cv::KeyPoint& dst);
    inline void getTransform(const cv::KeyPoint& src, const cv::KeyPoint& dst, float& tx, float& ty, float& angleDegrees, float& e);
    int getRansacConsensus(const Matrix3f& transform, const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& trainKeypoints, const std::vector<cv::KeyPoint>& queryKeypoints, float maxError, const VectorXi& acceptedStatus);
    void getRansacInliers(const Matrix3f& transform, const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& inputTrainKeypoints, const std::vector<cv::KeyPoint>& inputQueryKeypoints, float maxError, float maxError2, VectorXi& acceptedStatus, std::vector<cv::DMatch>& acceptedMatches, std::vector<Vector3f>& outputTrainKeypoints, std::vector<Vector3f>& outputQueryKeypoints);

    /* Debug Related */
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
    bool resizeModels; /* This is a must when using scanned images */
    int trainBanknoteHeight; /* hardcoded parameter in order to resize*/

    /** GEOS Stuff */
    geos::geom::GeometryFactory::Ptr factory;
    geos::geom::Point* aux_point;

    ColorRGBA debugColors[Classification::numOfRealBanknotes];
};
