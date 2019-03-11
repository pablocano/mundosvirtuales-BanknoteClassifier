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
#include "Representations/Image.h"
#include "Representations/FrameInfo.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ModuleManager/Module.h"

//#define _GLIBCXX_USE_CXX11_ABI 0

//pytorch
#include <torch/script.h>
#include <torch/csrc/jit/import.h>
#include <torch/torch.h>
#include <torch/csrc/api/include/torch/jit.h>
//pytorchEnd

//#define _GLIBCXX_USE_CXX11_ABI 1

MODULE(SemanticSeg,
{,
    REQUIRES(Image),
    REQUIRES(FrameInfo),
    PROVIDES(SegmentedImage),
});




class SemanticSeg : public SemanticSegBase
{
public:
    SemanticSeg();
    std::shared_ptr<torch::jit::script::Module> moduleTorch;


private:

    void update(SegmentedImage &image);
    torch::DeviceType device_type;
    at::Tensor output;
    void transpose(cv::Mat src);
    void colored(cv::Mat src, cv::Mat colored);
    float* bufferImgIn;
    double alpha; double beta; double input;



};
