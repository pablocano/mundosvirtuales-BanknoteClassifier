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

class Model
{
public:
    cv::cuda::GpuMat gpuImage;
    cv::Mat image;
    cv::Mat mask;
    Features features;
    std::vector<Eigen::Vector3f> corners;
};

class Hypothesys
{
public:

    std::vector<cv::DMatch> matches;
    Eigen::Matrix3f transform; /* From the model (a.k.a train image) to the camera image (a.k.a query image) */
    Eigen::Matrix3f pose;
    Eigen::Matrix3f graspPose;
    float houghVotes;

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

    void resizeImage(cv::Mat& image);
    cv::Mat getTransformAsMat(const cv::KeyPoint& src, const cv::KeyPoint& dst);
    inline void getTransform(const cv::KeyPoint& src, const cv::KeyPoint& dst, float& tx, float& ty, float& angleDegrees, float& e);
    void hough4d(const Model& model, ClassDetections& detections);

    void drawAcceptedHough();

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
