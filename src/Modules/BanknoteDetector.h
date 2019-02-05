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
    REQUIRES(GrayScaleImageEq),
    REQUIRES(FrameInfo),
    PROVIDES(BanknoteDetections),
});

class BanknoteDetector : public BanknoteDetectorBase
{
public:
    BanknoteDetector();

    void update(BanknoteDetections& detections);

protected:

    void resizeImage(cv::Mat& image);
    cv::Mat getTransformAsMat(const cv::KeyPoint& src, const cv::KeyPoint& dst);
    inline void getTransform(const cv::KeyPoint& src, const cv::KeyPoint& dst, float& tx, float& ty, float& angleDegrees, float& e);
    void hough4d(std::vector<cv::DMatch>& matches, std::vector<cv::KeyPoint>& imageKeypoints, std::vector<cv::KeyPoint>& modelKeypoints, const cv::Mat& mask, std::vector<cv::DMatch>& accepted);

    cv::Ptr<cv::cuda::DescriptorMatcher> matcher;
    cv::cuda::SURF_CUDA surf;
    cv::Ptr<cv::CLAHE> clahe;

    /** Gpu Buffers */
    cv::cuda::GpuMat GpuImage;
    cv::cuda::GpuMat GpuImageKeypoints;
    cv::cuda::GpuMat GpuImageDescriptors;
    cv::cuda::GpuMat GpuImageMask;

    /** RAM Buffers */
    std::vector<cv::KeyPoint> imageKeypoints;

    /** Matches Buffer. Should really use an alias or somethin'. Shape is classes x matches x 2 (2 is the KNN criteria)*/
    std::vector<std::vector<std::vector<cv::DMatch>>> matches;
    std::vector<std::vector<cv::DMatch>> filteredMatches;
    std::vector<std::vector<cv::DMatch>> houghFilteredMatches;

    /** Hardcoded parameters */


    /** Module Parameters */
    bool resizeModels;
    int trainBanknoteHeight; /* hardcoded parameter in order to resize*/

    /** Model Buffers */
    std::vector<cv::cuda::GpuMat> modelGpuImages;
    std::vector<cv::Mat> modelImages;
    std::vector<cv::Mat> modelMasks;
    std::vector<Features> modelFeatures;
    std::vector<std::vector<Eigen::Vector3f>> modelCorners;

    ColorRGBA debugColors[Classification::numOfBanknotes - 2];
};
